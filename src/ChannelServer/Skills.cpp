/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "GMPacket.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Mist.h"
#include "MobHandler.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "Players.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "SkillsPacket.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void Skills::addSkill(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t skillid = packet.get<int32_t>();
	if (!GameLogicUtilities::isBeginnerSkill(skillid)) {
		if (player->getStats()->getBaseStat(Stats::Sp) == 0) {
			// hacking
			return;
		}
		if (!player->isGm() && !GameLogicUtilities::skillMatchesJob(skillid, player->getStats()->getJob())) {
			// hacking
			return;
		}
	}
	if (player->getSkills()->addSkillLevel(skillid, 1) && !GameLogicUtilities::isBeginnerSkill(skillid)) {
		player->getStats()->setBaseStat(Stats::Sp, player->getStats()->getBaseStat(Stats::Sp) - 1);
	}
}

void Skills::cancelSkill(Player *player, PacketReader &packet) {
	stopSkill(player, packet.get<int32_t>());
}

void Skills::stopSkill(Player *player, int32_t skillid, bool fromTimer) {
	switch (skillid) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::FPArchMage::BigBang:
		case Jobs::ILArchMage::BigBang:
		case Jobs::Bishop::BigBang:
		case Jobs::Corsair::RapidFire: // Special skills
			SkillsPacket::endSpecialSkill(player, player->getSpecialSkillInfo());
			player->setSpecialSkill(SpecialSkillInfo());
			break;
		default:
			if (skillid == Jobs::SuperGm::Hide) { // GM Hide
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
	packet.skipBytes(4); // Ticks
	int32_t skillid = packet.get<int32_t>();
	int16_t addedinfo = 0;
	uint8_t level = packet.get<int8_t>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player->getSkills()->getSkillLevel(skillid) != level) {
		// Hacking
		return;
	}
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillid, level);
	switch (skillid) {
		case Jobs::Brawler::MpRecovery: {
			int16_t modhp = player->getStats()->getMHp() * skill->x / 100;
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
		case Jobs::Page::Threaten:
		case Jobs::FPWizard::Slow:
		case Jobs::ILWizard::Slow:
		case Jobs::FPMage::Seal:
		case Jobs::ILMage::Seal:
		case Jobs::Priest::Doom:
		case Jobs::Hermit::ShadowWeb:
		case Jobs::Shadower::NinjaAmbush:
		case Jobs::NightLord::NinjaAmbush: {
			uint8_t mobs = packet.get<int8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(packet.get<int32_t>())) {
					MobHandler::handleMobStatus(player->getId(), mob, skillid, level, 0, 0);
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
		case Jobs::Priest::Dispel: {
			int8_t affected = packet.get<int8_t>();
			player->getActiveBuffs()->useDispel();
			Party *party = player->getParty();
			if (party != 0) {
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != 0 && cmem != player && cmem->getMap() == player->getMap()) {
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
		case Jobs::Cleric::Heal: { // Heal
			Party *party = player->getParty();
			int16_t intfactor = player->getStats()->getBaseStat(Stats::Int) > 1000 ? 4000 : player->getStats()->getBaseStat(Stats::Int) * 4;
			int16_t lukfactor = player->getStats()->getBaseStat(Stats::Luk) > 1000 ? 4000 : player->getStats()->getBaseStat(Stats::Luk) * 4;
			int16_t maxhealing = (intfactor * 2 + lukfactor * 3) * level / 30;
			int16_t minhealing = maxhealing * 6 / 10;
			int16_t healing = Randomizer::Instance()->randShort(maxhealing - minhealing) + minhealing;
			int8_t targets = 1;
			if (party != 0) {
				int8_t affected = packet.get<int8_t>();
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				targets = (int8_t)(members.size());
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != 0 && cmem != player && cmem->getMap() == player->getMap()) {
						SkillsPacket::showSkill(cmem, skillid, level, direction, true, true);
						SkillsPacket::showSkill(cmem, skillid, level, direction, true);
						cmem->getStats()->modifyHp(healing / targets);
					}
				}
			}
			player->getStats()->modifyHp(healing / targets);
			break;
		}
		case Jobs::Fighter::Rage:
		case Jobs::Spearman::IronWill:
		case Jobs::Spearman::HyperBody:
		case Jobs::FPWizard::Meditation:
		case Jobs::ILWizard::Meditation:
		case Jobs::Cleric::Bless:
		case Jobs::Priest::HolySymbol:
		case Jobs::Bishop::Resurrection:
		case Jobs::Bishop::HolyShield:
		case Jobs::Bowmaster::SharpEyes:
		case Jobs::Marksman::SharpEyes:
		case Jobs::Assassin::Haste:
		case Jobs::Hermit::MesoUp:
		case Jobs::Bandit::Haste:
		case Jobs::Buccaneer::SpeedInfusion:
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
			if (skillid == Jobs::Buccaneer::TimeLeap)
				player->getSkills()->removeAllCooldowns();
			if (party != 0) {
				int8_t affected = packet.get<int8_t>();
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != 0 && cmem != player && cmem->getMap() == player->getMap()) {
						SkillsPacket::showSkill(cmem, skillid, level, direction, true, true);
						SkillsPacket::showSkill(cmem, skillid, level, direction, true);
						Buffs::addBuff(cmem, skillid, level, addedinfo);
						if (skillid == Jobs::Buccaneer::TimeLeap)
							cmem->getSkills()->removeAllCooldowns();
					}
				}
			}
			break;
		}
		case Jobs::Beginner::EchoOfHero:
		case Jobs::SuperGm::HealPlusDispel:
		case Jobs::SuperGm::Haste:
		case Jobs::SuperGm::HolySymbol:
		case Jobs::SuperGm::Bless:
		case Jobs::SuperGm::Resurrection:
		case Jobs::SuperGm::HyperBody: {
			uint8_t players = packet.get<int8_t>();
			for (uint8_t i = 0; i < players; i++) {
				int32_t playerid = packet.get<int32_t>();
				Player *target = Players::Instance()->getPlayer(playerid);
				if (target != 0 && target != player) { // ???
					SkillsPacket::showSkill(target, skillid, level, direction, true, true);
					SkillsPacket::showSkill(target, skillid, level, direction, true);
					Buffs::addBuff(target, skillid, level, addedinfo);
					if (skillid == Jobs::SuperGm::Resurrection && target->getStats()->getHp() <= 0)
						target->getStats()->setHp(target->getStats()->getMHp());
					else if (skillid == Jobs::SuperGm::HealPlusDispel && target->getStats()->getHp() > 0) {
						target->getStats()->setHp(target->getStats()->getMHp());
						target->getStats()->setMp(target->getStats()->getMMp());
						target->getActiveBuffs()->useDispel();
					}
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
	if (skillid != Jobs::Cleric::Heal)
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
	int16_t moneycon = skill->moneycon;
	int32_t item = skill->item;
	if (mpuse > 0) {
		if (player->getActiveBuffs()->getActiveSkillLevel(Jobs::Bowmaster::Concentrate) > 0) { // Reduced MP usage for Concentration
			int16_t mprate = SkillDataProvider::Instance()->getSkill(Jobs::Bowmaster::Concentrate, player->getActiveBuffs()->getActiveSkillLevel(Jobs::Bowmaster::Concentrate))->x;
			int16_t mploss = (mpuse * mprate) / 100;
			player->getStats()->modifyMp(-mploss, true);
		}
		else if (elementalamp && player->getSkills()->hasElementalAmp()) {
			int32_t sid = player->getSkills()->getElementalAmp();
			player->getStats()->modifyMp(-1 * (mpuse * SkillDataProvider::Instance()->getSkill(sid, player->getSkills()->getSkillLevel(sid))->x / 100), true);
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
		Inventory::takeItem(player, item, skill->itemcount);
	if (cooltime > 0 && skillid != Jobs::Corsair::Battleship)
		startCooldown(player, skillid, cooltime);
	if (moneycon > 0) {
		int16_t mesos_min = moneycon - (80 + level * 5);
		int16_t mesos_max = moneycon + (80 + level * 5);
		int16_t difference = mesos_max - mesos_min; // Randomize up to this, add minimum for range
		int16_t amount = Randomizer::Instance()->randShort(difference) + mesos_min;
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
	if (skillid != Jobs::All::RegularAttack && SkillDataProvider::Instance()->getSkill(skillid, level)->bulletcon > 0)
		hits = SkillDataProvider::Instance()->getSkill(skillid, level)->bulletcon;
	if (player->getActiveBuffs()->hasShadowPartner())
		hits *= 2;
	if (pos > 0 && !(player->getActiveBuffs()->hasShadowStars() || player->getActiveBuffs()->hasSoulArrow()))
		// If they don't have Shadow Stars or Soul Arrow, take the items
		Inventory::takeItemSlot(player, Inventories::UseInventory, pos, hits);
}

void Skills::heal(Player *player, int16_t value, int32_t skillid) {
	if (player->getStats()->getHp() < player->getStats()->getMHp() && player->getStats()->getHp() > 0) {
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