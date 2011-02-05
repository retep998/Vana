/*
Copyright (C) 2008-2011 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "Skills.h"
#include "Buffs.h"
#include "GameLogicUtilities.h"
#include "GmPacket.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mist.h"
#include "MobHandler.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "Randomizer.h"
#include "SkillConstants.h"
#include "SkillDataProvider.h"
#include "SkillsPacket.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void Skills::addSkill(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int32_t skillid = packet.get<int32_t>();
	if (!GameLogicUtilities::isBeginnerSkill(skillid)) {
		if (player->getStats()->getSp() == 0) {
			// Hacking
			return;
		}
		if (!player->isGm() && !GameLogicUtilities::skillMatchesJob(skillid, player->getStats()->getJob())) {
			// Hacking
			return;
		}
	}
	if (player->getSkills()->addSkillLevel(skillid, 1) && !GameLogicUtilities::isBeginnerSkill(skillid)) {
		player->getStats()->setSp(player->getStats()->getSp() - 1);
	}
}

void Skills::cancelSkill(Player *player, PacketReader &packet) {
	stopSkill(player, packet.get<int32_t>());
}

void Skills::stopSkill(Player *player, int32_t skillid, bool fromTimer) {
	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::WindArcher::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::FPArchMage::BigBang:
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang:
		case Jobs::Corsair::RapidFire:
			SkillsPacket::endSpecialSkill(player, player->getSpecialSkillInfo());
			player->setSpecialSkill(SpecialSkillInfo());
			break;
		default:
			if (player->getActiveBuffs()->getActiveSkillLevel(skillid) == 0) {
				// Hacking
				return;
			}
			if (skillid == Jobs::SuperGm::Hide) {
				MapPacket::showPlayer(player);
				GmPacket::endHide(player);
			}
			player->getActiveBuffs()->removeBuff(skillid, fromTimer);
			if (GameLogicUtilities::isMobSkill(skillid))
				Buffs::endDebuff(player, (uint8_t)(skillid));
			else
				Buffs::endBuff(player, skillid);
			break;
	}
}

const vector<Player *> Skills::getAffectedPartyMembers(Party *party, int8_t affected, int8_t members) {
	vector<Player *> ret;
	if (affected & GameLogicUtilities::getPartyMember1(members))
		ret.push_back(party->getMemberByIndex(1));
	if (affected & GameLogicUtilities::getPartyMember2(members))
		ret.push_back(party->getMemberByIndex(2));
	if (affected & GameLogicUtilities::getPartyMember3(members))
		ret.push_back(party->getMemberByIndex(3));
	if (affected & GameLogicUtilities::getPartyMember4(members))
		ret.push_back(party->getMemberByIndex(4));
	if (affected & GameLogicUtilities::getPartyMember5(members))
		ret.push_back(party->getMemberByIndex(5));
	if (affected & GameLogicUtilities::getPartyMember6(members))
		ret.push_back(party->getMemberByIndex(6));
	return ret;
}

void Skills::useSkill(Player *player, PacketReader &packet) {
	if (!player->updateTickCount(packet.get<int32_t>())) {
		// Tickcount was the same or less than 100 of the difference.
		return;
	}
	int32_t skillid = packet.get<int32_t>();
	int16_t addedinfo = 0;
	uint8_t level = packet.get<uint8_t>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player->getSkills()->getSkillLevel(skillid) != level) {
		// Hacking
		return;
	}
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	switch (skillid) {
		case Jobs::Brawler::MpRecovery: {
			int16_t modhp = player->getStats()->getMaxHp() * skill->x / 100;
			int16_t healmp = modhp * skill->y / 100;
			player->getStats()->modifyHp(-modhp);
			player->getStats()->modifyMp(healmp);
			break;
		}
		case Jobs::Shadower::Smokescreen: {
			int16_t x = packet.get<int16_t>();
			int16_t y = packet.get<int16_t>();
			Pos origin = Pos(x, y);
			Mist *m = new Mist(player->getMap(), player, origin, skill, skillid, level);
			break;
		}
		case Jobs::Corsair::Battleship:
			if (player->getActiveBuffs()->getBattleshipHp() == 0)
				player->getActiveBuffs()->resetBattleshipHp();
			break;
		case Jobs::Crusader::ArmorCrash:
		case Jobs::WhiteKnight::MagicCrash:
		case Jobs::DragonKnight::PowerCrash: {
			packet.skipBytes(4); // Might be CRC too O.o?
			int8_t mobs = packet.get<int8_t>();
			for (int8_t k = 0; k < mobs; k++) {
				int32_t mapmobid = packet.get<int32_t>();
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid)) {
					if (Randomizer::Instance()->randShort(99) < skill->prop) {
						mob->doCrashSkill(skillid);
					}
				}
			}
			break;
		}
		case Jobs::Hero::MonsterMagnet:
		case Jobs::Paladin::MonsterMagnet:
		case Jobs::DarkKnight::MonsterMagnet: {
			int32_t mobs = packet.get<int32_t>();
			for (int8_t k = 0; k < mobs; k++) {
				int32_t mapmobid = packet.get<int32_t>();
				uint8_t success = packet.get<int8_t>();
				SkillsPacket::showMagnetSuccess(player, mapmobid, success);
			}
			direction = packet.get<uint8_t>();
			break;
		}
		case Jobs::FPWizard::Slow:
		case Jobs::ILWizard::Slow:
		case Jobs::BlazeWizard::Slow:
		case Jobs::Page::Threaten:
			packet.skipBytes(4); // Might be CRC too O.o?
		case Jobs::FPMage::Seal:
		case Jobs::ILMage::Seal:
		case Jobs::BlazeWizard::Seal:
		case Jobs::Priest::Doom:
		case Jobs::Hermit::ShadowWeb:
		case Jobs::NightWalker::ShadowWeb:
		case Jobs::Shadower::NinjaAmbush:
		case Jobs::NightLord::NinjaAmbush: {
			uint8_t mobs = packet.get<int8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(packet.get<int32_t>())) {
					MobHandler::handleMobStatus(player, mob, skillid, level, 0, 0);
				}
			}
			break;
		}
		case Jobs::Bishop::HerosWill:
		case Jobs::ILArchMage::HerosWill:
		case Jobs::FPArchMage::HerosWill:
		case Jobs::DarkKnight::HerosWill:
		case Jobs::Hero::HerosWill:
		case Jobs::Paladin::HerosWill:
		case Jobs::NightLord::HerosWill:
		case Jobs::Shadower::HerosWill:
		case Jobs::Bowmaster::HerosWill:
		case Jobs::Marksman::HerosWill:
		case Jobs::Buccaneer::PiratesRage:
		case Jobs::Corsair::SpeedInfusion:
			player->getActiveBuffs()->removeDebuff(MobSkills::Seduce);
			break;
		case Jobs::Priest::MysticDoor: {
			if (Door *door = player->getDoor()) {
				door->closeDoor(true);
			}
			int16_t x = packet.get<int16_t>();
			int16_t y = packet.get<int16_t>();
			new Door(player, Pos(x, y), SkillDataProvider::Instance()->getSkill(skillid, level)->time);
			break;
		}
		case Jobs::Priest::Dispel: {
			int8_t affected = packet.get<int8_t>();
			player->getActiveBuffs()->useDispel();
			Party *party = player->getParty();
			if (party != nullptr) {
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != nullptr && cmem != player && cmem->getMap() == player->getMap()) {
						if (Randomizer::Instance()->randShort(99) < skill->prop) {
							SkillsPacket::showSkill(cmem, skillid, level, direction, true, true);
							SkillsPacket::showSkill(cmem, skillid, level, direction, true);
							cmem->getActiveBuffs()->useDispel();
						}
					}
				}
			}
			packet.skipBytes(2);
			affected = packet.get<int8_t>();
			for (int8_t k = 0; k < affected; k++) {
				int32_t mapmobid = packet.get<int32_t>();
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(mapmobid)) {
					if (Randomizer::Instance()->randShort(99) < skill->prop) {
						mob->dispelBuffs();
					}
				}
			}
			break;
		}
		case Jobs::Cleric::Heal: {
			uint16_t healrate = skill->hpProp;
			if (healrate > 100)
				healrate = 100;

			Party *party = player->getParty();
			int8_t partyPlayers = (party != nullptr ? party->getMembersCount() : 1);
			int32_t expIncreasement = 0;

			int16_t heal = (healrate * player->getStats()->getMaxHp() / 100) / partyPlayers;

			if (party != nullptr) {
				vector<Player *> members = party->getPartyMembers(player->getMap());
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					int16_t chp = cmem->getStats()->getHp();
					if (chp != 0 && chp < cmem->getStats()->getMaxHp()) {
						cmem->getStats()->modifyHp(heal);
						if (player != cmem) {
							expIncreasement += 20 * (cmem->getStats()->getHp() - chp) / (8 * cmem->getStats()->getLevel() + 190);
						}
					}
				}
				if (expIncreasement > 0) {
					player->getStats()->giveExp(expIncreasement);
				}
			}
			else {
				player->getStats()->modifyHp(heal);
			}

		}
		case Jobs::Fighter::Rage:
		case Jobs::DawnWarrior::Rage:
		case Jobs::Spearman::IronWill:
		case Jobs::Spearman::HyperBody:
		case Jobs::FPWizard::Meditation:
		case Jobs::ILWizard::Meditation:
		case Jobs::BlazeWizard::Meditation:
		case Jobs::Cleric::Bless:
		case Jobs::Priest::HolySymbol:
		case Jobs::Bishop::Resurrection:
		case Jobs::Bishop::HolyShield:
		case Jobs::Bowmaster::SharpEyes:
		case Jobs::Marksman::SharpEyes:
		case Jobs::Assassin::Haste:
		case Jobs::NightWalker::Haste:
		case Jobs::Hermit::MesoUp:
		case Jobs::Bandit::Haste:
		case Jobs::Buccaneer::SpeedInfusion:
		case Jobs::ThunderBreaker::SpeedInfusion:
		case Jobs::Buccaneer::TimeLeap: 
		case Jobs::Hero::MapleWarrior:
		case Jobs::Paladin::MapleWarrior:
		case Jobs::DarkKnight::MapleWarrior:
		case Jobs::FPArchMage::MapleWarrior:
		case Jobs::ILArchMage::MapleWarrior:
		case Jobs::Bishop::MapleWarrior:
		case Jobs::Bowmaster::MapleWarrior:
		case Jobs::Marksman::MapleWarrior:
		case Jobs::NightLord::MapleWarrior:
		case Jobs::Shadower::MapleWarrior:
		case Jobs::Buccaneer::MapleWarrior:
		case Jobs::Corsair::MapleWarrior: {
			Party *party = player->getParty();
			if (skillid == Jobs::Buccaneer::TimeLeap) {
				player->getSkills()->removeAllCooldowns();
			}
			if (party != nullptr) {
				int8_t affected = packet.get<int8_t>();
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != nullptr && cmem != player && cmem->getMap() == player->getMap()) {
						SkillsPacket::showSkill(cmem, skillid, level, direction, true, true);
						SkillsPacket::showSkill(cmem, skillid, level, direction, true);
						Buffs::addBuff(cmem, skillid, level, addedinfo);
						if (skillid == Jobs::Buccaneer::TimeLeap) {
							cmem->getSkills()->removeAllCooldowns();
						}
					}
				}
			}
			break;
		}
		case Jobs::Beginner::EchoOfHero:
		case Jobs::Noblesse::EchoOfHero:
		case Jobs::SuperGm::Haste:
		case Jobs::SuperGm::HolySymbol:
		case Jobs::SuperGm::Bless:
		case Jobs::SuperGm::HyperBody: {
			uint8_t players = packet.get<int8_t>();
			for (uint8_t i = 0; i < players; i++) {
				int32_t playerid = packet.get<int32_t>();
				Player *target = PlayerDataProvider::Instance()->getPlayer(playerid);
				if (target != nullptr && target != player) {
					SkillsPacket::showSkill(target, skillid, level, direction, true, true);
					SkillsPacket::showSkill(target, skillid, level, direction, true);
					Buffs::addBuff(target, skillid, level, addedinfo);
				}
			}
			break;
		}
		case Jobs::SuperGm::HealPlusDispel: {
			uint8_t players = packet.get<int8_t>();
			for (uint8_t i = 0; i < players; i++) {
				int32_t playerid = packet.get<int32_t>();
				Player *target = PlayerDataProvider::Instance()->getPlayer(playerid);
				if (target != nullptr && target != player && target->getStats()->getHp() > 0) {
					SkillsPacket::showSkill(target, skillid, level, direction, true, true);
					SkillsPacket::showSkill(target, skillid, level, direction, true);
					target->getStats()->setHp(target->getStats()->getMaxHp());
					target->getStats()->setMp(target->getStats()->getMaxMp());
					target->getActiveBuffs()->useDispel();
				}
			}
			break;
		}
		case Jobs::SuperGm::Resurrection: {
			uint8_t players = packet.get<int8_t>();
			for (uint8_t i = 0; i < players; i++) {
				int32_t playerid = packet.get<int32_t>();
				Player *target = PlayerDataProvider::Instance()->getPlayer(playerid);
				if (target != nullptr && target != player && target->getStats()->getHp() <= 0) {
					SkillsPacket::showSkill(target, skillid, level, direction, true, true);
					SkillsPacket::showSkill(target, skillid, level, direction, true);
					target->getStats()->setHp(target->getStats()->getMaxHp());
				}
			}
			break;
		}
		case Jobs::SuperGm::Hide:
			MapPacket::removePlayer(player);
			GmPacket::beginHide(player);
			break;
		default:
			type = packet.get<int8_t>();
			switch (type) {
				case 0x80:
					addedinfo = packet.get<int16_t>();
					break;
			}
			break;
	}
	applySkillCosts(player, skillid, level);
	SkillsPacket::showSkill(player, skillid, level, direction);
	if (Buffs::addBuff(player, skillid, level, addedinfo))
		return;
	if (GameLogicUtilities::isSummon(skillid))
		Summons::useSummon(player, skillid, level);
}

void Skills::applySkillCosts(Player *player, int32_t skillid, uint8_t level, bool elementalamp) {
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	int16_t cooltime = skill->cooltime;
	int16_t mpuse = skill->mp;
	int16_t hpuse = skill->hp;
	int16_t moneycon = skill->moneyConsume;
	int32_t item = skill->item;
	if (mpuse > 0) {
		if (SkillLevelInfo *conc = player->getActiveBuffs()->getActiveSkillInfo(Jobs::Bowmaster::Concentrate)) {
			int16_t mprate = conc->x;
			int16_t mploss = (mpuse * mprate) / 100;
			player->getStats()->modifyMp(-mploss, true);
		}
		else if (elementalamp && player->getSkills()->hasElementalAmp()) {
			player->getStats()->modifyMp(-1 * (mpuse * player->getSkills()->getSkillInfo(player->getSkills()->getElementalAmp())->x / 100), true);
		}
		else {
			player->getStats()->modifyMp(-mpuse, true);
		}
	}
	else
		player->getStats()->setMp(player->getStats()->getMp(), true);
	if (hpuse > 0)
		player->getStats()->modifyHp(-hpuse);
	if (item > 0)
		Inventory::takeItem(player, item, skill->itemCount);
	if (cooltime > 0 && skillid != Jobs::Corsair::Battleship)
		startCooldown(player, skillid, cooltime);
	if (moneycon > 0) {
		int16_t mesos_min = moneycon - (80 + level * 5);
		int16_t mesos_max = moneycon + (80 + level * 5);
		int16_t amount = Randomizer::Instance()->randShort(mesos_max, mesos_min);
		int32_t mesos = player->getInventory()->getMesos();
		if (mesos - amount > -1)
			player->getInventory()->modifyMesos(-amount);
		else {
			// Hacking
			return;
		}
	}
}

void Skills::useAttackSkill(Player *player, int32_t skillid) {
	if (skillid != Jobs::All::RegularAttack) {
		uint8_t level = player->getSkills()->getSkillLevel(skillid);
		if (!SkillDataProvider::Instance()->isSkill(skillid) || level == 0)
			return;
		applySkillCosts(player, skillid, level, true);
	}
}

void Skills::useAttackSkillRanged(Player *player, int32_t skillid, int16_t pos) {
	uint8_t level = 0;
	if (skillid != Jobs::All::RegularAttack) {
		level = player->getSkills()->getSkillLevel(skillid);
		if (!SkillDataProvider::Instance()->isSkill(skillid) || level == 0)
			return;
		applySkillCosts(player, skillid, level);
	}
	uint16_t hits = 1;
	if (skillid != Jobs::All::RegularAttack) {
		uint16_t bullets = SkillDataProvider::Instance()->getSkill(skillid, level)->bulletConsume;
		if (bullets > 0) {
			hits = bullets;
		}
	}
	if (player->getActiveBuffs()->hasShadowPartner()) {
		hits *= 2;
	}
	if (pos > 0 && !(player->getActiveBuffs()->hasShadowStars() || player->getActiveBuffs()->hasSoulArrow())) {
		// If they don't have Shadow Stars or Soul Arrow, take the items
		Inventory::takeItemSlot(player, Inventories::UseInventory, pos, hits);
	}
}

void Skills::heal(Player *player, int16_t value, int32_t skillid) {
	if (player->getStats()->getHp() < player->getStats()->getMaxHp() && player->getStats()->getHp() > 0) {
		player->getStats()->modifyHp(value);
		SkillsPacket::healHP(player, value);
	}
}

void Skills::hurt(Player *player, int16_t value, int32_t skillid) {
	if (player->getStats()->getHp() - value > 1) {
		player->getStats()->modifyHp(-value);
		SkillsPacket::showSkillEffect(player, skillid);
	}
	else {
		Buffs::endBuff(player, skillid);
	}
}

void Skills::startCooldown(Player *player, int32_t skillid, int16_t cooltime, bool initialload) {
	if (isCooling(player, skillid)) {
		// Hacking
		return;
	}
	if (!initialload) {
		SkillsPacket::sendCooldown(player, skillid, cooltime);
		player->getSkills()->addCooldown(skillid, cooltime);
	}
	new Timer::Timer(bind(&Skills::stopCooldown, player, skillid),
		Timer::Id(Timer::Types::CoolTimer, skillid, 0),
		player->getTimers(), Timer::Time::fromNow(cooltime * 1000));
}

void Skills::stopCooldown(Player *player, int32_t skillid) {
	player->getSkills()->removeCooldown(skillid);
	SkillsPacket::sendCooldown(player, skillid, 0);
	if (skillid == Jobs::Corsair::Battleship) {
		player->getActiveBuffs()->resetBattleshipHp();
	}

	Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
	if (player->getTimers()->checkTimer(id) > 0) {
		player->getTimers()->removeTimer(id);
	}
}

bool Skills::isCooling(Player *player, int32_t skillid) {
	Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
	return player->getTimers()->checkTimer(id) > 0;
}

int16_t Skills::getCooldownTimeLeft(Player *player, int32_t skillid) {
	int16_t cooltime = 0;
	if (isCooling(player, skillid)) {
		Timer::Id id(Timer::Types::CoolTimer, skillid, 0);
		cooltime = static_cast<int16_t>(player->getTimers()->checkTimer(id));
	}
	return cooltime;
}