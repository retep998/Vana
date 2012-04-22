/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "SummonHandler.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include <functional>

using std::bind;

void Skills::addSkill(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int32_t skillId = packet.get<int32_t>();
	if (!GameLogicUtilities::isBeginnerSkill(skillId)) {
		if (player->getStats()->getSp() == 0) {
			// Hacking
			return;
		}
		if (!player->isGm() && !GameLogicUtilities::skillMatchesJob(skillId, player->getStats()->getJob())) {
			// Hacking
			return;
		}
	}
	if (player->getSkills()->addSkillLevel(skillId, 1) && !GameLogicUtilities::isBeginnerSkill(skillId)) {
		player->getStats()->setSp(player->getStats()->getSp() - 1);
	}
}

void Skills::cancelSkill(Player *player, PacketReader &packet) {
	stopSkill(player, packet.get<int32_t>());
}

void Skills::stopSkill(Player *player, int32_t skillId, bool fromTimer) {
	switch (skillId) {
		case Jobs::Bowmaster::Hurricane:
		case Jobs::WindArcher::Hurricane:
		case Jobs::Marksman::PiercingArrow:
		case Jobs::FpArchMage::BigBang:
		case Jobs::IlArchMage::BigBang:
		case Jobs::Bishop::BigBang:
		case Jobs::Corsair::RapidFire:
			SkillsPacket::endSpecialSkill(player, player->getSpecialSkillInfo());
			player->setSpecialSkill(SpecialSkillInfo());
			break;
		default:
			if (player->getActiveBuffs()->getActiveSkillLevel(skillId) == 0) {
				// Hacking
				return;
			}
			if (skillId == Jobs::SuperGm::Hide) {
				MapPacket::showPlayer(player);
				GmPacket::endHide(player);
			}
			player->getActiveBuffs()->removeBuff(skillId, fromTimer);
			if (GameLogicUtilities::isMobSkill(skillId)) {
				Buffs::endDebuff(player, static_cast<uint8_t>(skillId));
			}
			else {
				Buffs::endBuff(player, skillId);
			}
			break;
	}
}

const vector<Player *> Skills::getAffectedPartyMembers(Party *party, int8_t affected, int8_t members) {
	vector<Player *> ret;
	if (affected & GameLogicUtilities::getPartyMember1(members)) {
		ret.push_back(party->getMemberByIndex(1));
	}
	if (affected & GameLogicUtilities::getPartyMember2(members)) {
		ret.push_back(party->getMemberByIndex(2));
	}
	if (affected & GameLogicUtilities::getPartyMember3(members)) {
		ret.push_back(party->getMemberByIndex(3));
	}
	if (affected & GameLogicUtilities::getPartyMember4(members)) {
		ret.push_back(party->getMemberByIndex(4));
	}
	if (affected & GameLogicUtilities::getPartyMember5(members)) {
		ret.push_back(party->getMemberByIndex(5));
	}
	if (affected & GameLogicUtilities::getPartyMember6(members)) {
		ret.push_back(party->getMemberByIndex(6));
	}
	return ret;
}

void Skills::useSkill(Player *player, PacketReader &packet) {
	packet.skipBytes(4); // Ticks
	int32_t skillId = packet.get<int32_t>();
	int16_t addedInfo = 0;
	uint8_t level = packet.get<uint8_t>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player->getSkills()->getSkillLevel(skillId) != level) {
		// Hacking
		return;
	}
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	switch (skillId) {
		case Jobs::Brawler::MpRecovery: {
			int16_t modHp = player->getStats()->getMaxHp() * skill->x / 100;
			int16_t healMp = modHp * skill->y / 100;
			player->getStats()->modifyHp(-modHp);
			player->getStats()->modifyMp(healMp);
			break;
		}
		case Jobs::Shadower::Smokescreen: {
			int16_t x = packet.get<int16_t>();
			int16_t y = packet.get<int16_t>();
			const Pos &origin = Pos(x, y);
			Mist *m = new Mist(player->getMap(), player, origin, skill, skillId, level);
			break;
		}
		case Jobs::Corsair::Battleship:
			if (player->getActiveBuffs()->getBattleshipHp() == 0) {
				player->getActiveBuffs()->resetBattleshipHp();
			}
			break;
		case Jobs::Crusader::ArmorCrash:
		case Jobs::WhiteKnight::MagicCrash:
		case Jobs::DragonKnight::PowerCrash: {
			packet.skipBytes(4); // Might be CRC too O.o?
			uint8_t mobs = packet.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				int32_t mapMobId = packet.get<int32_t>();
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(mapMobId)) {
					if (Randomizer::Instance()->randShort(99) < skill->prop) {
						mob->doCrashSkill(skillId);
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
				int32_t mapMobId = packet.get<int32_t>();
				uint8_t success = packet.get<int8_t>();
				SkillsPacket::showMagnetSuccess(player, mapMobId, success);
			}
			direction = packet.get<uint8_t>();
			break;
		}
		case Jobs::FpWizard::Slow:
		case Jobs::IlWizard::Slow:
		case Jobs::BlazeWizard::Slow:
		case Jobs::Page::Threaten:
			packet.skipBytes(4); // Might be CRC too O.o?
		case Jobs::FpMage::Seal:
		case Jobs::IlMage::Seal:
		case Jobs::BlazeWizard::Seal:
		case Jobs::Priest::Doom:
		case Jobs::Hermit::ShadowWeb:
		case Jobs::NightWalker::ShadowWeb:
		case Jobs::Shadower::NinjaAmbush:
		case Jobs::NightLord::NinjaAmbush: {
			uint8_t mobs = packet.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(packet.get<int32_t>())) {
					MobHandler::handleMobStatus(player->getId(), mob, skillId, level, 0, 0);
				}
			}
			break;
		}
		case Jobs::Bishop::HerosWill:
		case Jobs::IlArchMage::HerosWill:
		case Jobs::FpArchMage::HerosWill:
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
			if (Party *party = player->getParty()) {
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != nullptr && cmem != player && cmem->getMap() == player->getMap()) {
						if (Randomizer::Instance()->randShort(99) < skill->prop) {
							SkillsPacket::showSkill(cmem, skillId, level, direction, true, true);
							SkillsPacket::showSkill(cmem, skillId, level, direction, true);
							cmem->getActiveBuffs()->useDispel();
						}
					}
				}
			}
			packet.skipBytes(2);
			affected = packet.get<int8_t>();
			for (int8_t k = 0; k < affected; k++) {
				int32_t mapMobId = packet.get<int32_t>();
				if (Mob *mob = Maps::getMap(player->getMap())->getMob(mapMobId)) {
					if (Randomizer::Instance()->randShort(99) < skill->prop) {
						mob->dispelBuffs();
					}
				}
			}
			break;
		}
		case Jobs::Cleric::Heal: {
			uint16_t healRate = skill->hpProp;
			if (healRate > 100) {
				healRate = 100;
			}
			Party *party = player->getParty();
			int8_t partyPlayers = (party != nullptr ? party->getMembersCount() : 1);
			int32_t expIncrease = 0;

			int16_t heal = (healRate * player->getStats()->getMaxHp() / 100) / partyPlayers;

			if (party != nullptr) {
				vector<Player *> members = party->getPartyMembers(player->getMap());
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					int16_t chp = cmem->getStats()->getHp();
					if (chp > 0 && chp < cmem->getStats()->getMaxHp()) {
						cmem->getStats()->modifyHp(heal);
						if (player != cmem) {
							expIncrease += 20 * (cmem->getStats()->getHp() - chp) / (8 * cmem->getStats()->getLevel() + 190);
						}
					}
				}
				if (expIncrease > 0) {
					player->getStats()->giveExp(expIncrease);
				}
			}
			else {
				player->getStats()->modifyHp(heal);
			}
			break;
		}
		case Jobs::Fighter::Rage:
		case Jobs::DawnWarrior::Rage:
		case Jobs::Spearman::IronWill:
		case Jobs::Spearman::HyperBody:
		case Jobs::FpWizard::Meditation:
		case Jobs::IlWizard::Meditation:
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
		case Jobs::FpArchMage::MapleWarrior:
		case Jobs::IlArchMage::MapleWarrior:
		case Jobs::Bishop::MapleWarrior:
		case Jobs::Bowmaster::MapleWarrior:
		case Jobs::Marksman::MapleWarrior:
		case Jobs::NightLord::MapleWarrior:
		case Jobs::Shadower::MapleWarrior:
		case Jobs::Buccaneer::MapleWarrior:
		case Jobs::Corsair::MapleWarrior: {
			if (skillId == Jobs::Buccaneer::TimeLeap) {
				player->getSkills()->removeAllCooldowns();
			}
			if (Party *party = player->getParty()) {
				int8_t affected = packet.get<int8_t>();
				int8_t pmembers = party->getMembersCount();
				vector<Player *> members = getAffectedPartyMembers(party, affected, pmembers);
				for (size_t i = 0; i < members.size(); i++) {
					Player *cmem = members[i];
					if (cmem != nullptr && cmem != player && cmem->getMap() == player->getMap()) {
						SkillsPacket::showSkill(cmem, skillId, level, direction, true, true);
						SkillsPacket::showSkill(cmem, skillId, level, direction, true);
						Buffs::addBuff(cmem, skillId, level, addedInfo);
						if (skillId == Jobs::Buccaneer::TimeLeap) {
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
				int32_t playerId = packet.get<int32_t>();
				Player *target = PlayerDataProvider::Instance()->getPlayer(playerId);
				if (target != nullptr && target != player) {
					SkillsPacket::showSkill(target, skillId, level, direction, true, true);
					SkillsPacket::showSkill(target, skillId, level, direction, true);
					Buffs::addBuff(target, skillId, level, addedInfo);
				}
			}
			break;
		}
		case Jobs::SuperGm::HealPlusDispel: {
			uint8_t players = packet.get<int8_t>();
			for (uint8_t i = 0; i < players; i++) {
				int32_t playerId = packet.get<int32_t>();
				Player *target = PlayerDataProvider::Instance()->getPlayer(playerId);
				if (target != nullptr && target != player && target->getStats()->getHp() > 0) {
					SkillsPacket::showSkill(target, skillId, level, direction, true, true);
					SkillsPacket::showSkill(target, skillId, level, direction, true);
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
				int32_t playerId = packet.get<int32_t>();
				Player *target = PlayerDataProvider::Instance()->getPlayer(playerId);
				if (target != nullptr && target != player && target->getStats()->getHp() <= 0) {
					SkillsPacket::showSkill(target, skillId, level, direction, true, true);
					SkillsPacket::showSkill(target, skillId, level, direction, true);
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
					addedInfo = packet.get<int16_t>();
					break;
			}
			break;
	}
	applySkillCosts(player, skillId, level);
	SkillsPacket::showSkill(player, skillId, level, direction);

	if (Buffs::addBuff(player, skillId, level, addedInfo)) {
		return;
	}
	if (GameLogicUtilities::isSummon(skillId)) {
		SummonHandler::useSummon(player, skillId, level);
	}
}

void Skills::applySkillCosts(Player *player, int32_t skillId, uint8_t level, bool elementalAmp) {
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	int16_t coolTime = skill->coolTime;
	int16_t mpUse = skill->mp;
	int16_t hpUse = skill->hp;
	int16_t moneyConsume = skill->moneyConsume;
	int32_t item = skill->item;
	if (mpUse > 0) {
		if (SkillLevelInfo *conc = player->getActiveBuffs()->getActiveSkillInfo(Jobs::Bowmaster::Concentrate)) {
			int16_t mpRate = conc->x;
			int16_t mpLoss = (mpUse * mpRate) / 100;
			player->getStats()->modifyMp(-mpLoss, true);
		}
		else if (elementalAmp && player->getSkills()->hasElementalAmp()) {
			player->getStats()->modifyMp(-1 * (mpUse * player->getSkills()->getSkillInfo(player->getSkills()->getElementalAmp())->x / 100), true);
		}
		else {
			player->getStats()->modifyMp(-mpUse, true);
		}
	}
	else {
		player->getStats()->setMp(player->getStats()->getMp(), true);
	}
	if (hpUse > 0) {
		player->getStats()->modifyHp(-hpUse);
	}
	if (item > 0) {
		Inventory::takeItem(player, item, skill->itemCount);
	}
	if (coolTime > 0 && skillId != Jobs::Corsair::Battleship) {
		startCooldown(player, skillId, coolTime);
	}
	if (moneyConsume > 0) {
		int16_t minMesos = moneyConsume - (80 + level * 5);
		int16_t maxMesos = moneyConsume + (80 + level * 5);
		int16_t amount = Randomizer::Instance()->randShort(maxMesos, minMesos);
		int32_t mesos = player->getInventory()->getMesos();
		if (mesos - amount > -1) {
			player->getInventory()->modifyMesos(-amount);
		}
		else {
			// Hacking
			return;
		}
	}
}

void Skills::useAttackSkill(Player *player, int32_t skillId) {
	if (skillId != Jobs::All::RegularAttack) {
		uint8_t level = player->getSkills()->getSkillLevel(skillId);
		if (!SkillDataProvider::Instance()->isSkill(skillId) || level == 0) {
			return;
		}
		applySkillCosts(player, skillId, level, true);
	}
}

void Skills::useAttackSkillRanged(Player *player, int32_t skillId, int16_t pos) {
	uint8_t level = 0;
	if (skillId != Jobs::All::RegularAttack) {
		level = player->getSkills()->getSkillLevel(skillId);
		if (!SkillDataProvider::Instance()->isSkill(skillId) || level == 0) {
			return;
		}
		applySkillCosts(player, skillId, level);
	}
	uint16_t hits = 1;
	if (skillId != Jobs::All::RegularAttack) {
		uint16_t bullets = SkillDataProvider::Instance()->getSkill(skillId, level)->bulletConsume;
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

void Skills::heal(Player *player, int16_t value, int32_t skillId) {
	if (player->getStats()->getHp() < player->getStats()->getMaxHp() && player->getStats()->getHp() > 0) {
		player->getStats()->modifyHp(value);
		SkillsPacket::healHP(player, value);
	}
}

void Skills::hurt(Player *player, int16_t value, int32_t skillId) {
	if (player->getStats()->getHp() - value > 1) {
		player->getStats()->modifyHp(-value);
		SkillsPacket::showSkillEffect(player, skillId);
	}
	else {
		Buffs::endBuff(player, skillId);
	}
}

void Skills::startCooldown(Player *player, int32_t skillId, int16_t coolTime, bool initialLoad) {
	if (isCooling(player, skillId)) {
		// Hacking
		return;
	}
	if (!initialLoad) {
		SkillsPacket::sendCooldown(player, skillId, coolTime);
		player->getSkills()->addCooldown(skillId, coolTime);
	}
	new Timer::Timer(bind(&Skills::stopCooldown, player, skillId),
		Timer::Id(Timer::Types::CoolTimer, skillId, 0),
		player->getTimers(), TimeUtilities::fromNow(coolTime * 1000));
}

void Skills::stopCooldown(Player *player, int32_t skillId) {
	player->getSkills()->removeCooldown(skillId);
	SkillsPacket::sendCooldown(player, skillId, 0);
	if (skillId == Jobs::Corsair::Battleship) {
		player->getActiveBuffs()->resetBattleshipHp();
	}

	Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
	if (player->getTimers()->checkTimer(id) > 0) {
		player->getTimers()->removeTimer(id);
	}
}

bool Skills::isCooling(Player *player, int32_t skillId) {
	Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
	return player->getTimers()->checkTimer(id) > 0;
}

int16_t Skills::getCooldownTimeLeft(Player *player, int32_t skillId) {
	int16_t coolTime = 0;
	if (isCooling(player, skillId)) {
		Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
		coolTime = static_cast<int16_t>(player->getTimers()->checkTimer(id));
	}
	return coolTime;
}