/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Skills.hpp"
#include "Buffs.hpp"
#include "GameLogicUtilities.hpp"
#include "GmPacket.hpp"
#include "Inventory.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "Mist.hpp"
#include "MobHandler.hpp"
#include "PacketReader.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Randomizer.hpp"
#include "SkillConstants.hpp"
#include "SkillDataProvider.hpp"
#include "SkillsPacket.hpp"
#include "SummonHandler.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>

auto Skills::addSkill(Player *player, PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
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

auto Skills::cancelSkill(Player *player, PacketReader &packet) -> void {
	stopSkill(player, packet.get<int32_t>());
}

auto Skills::stopSkill(Player *player, int32_t skillId, bool fromTimer) -> void {
	switch (skillId) {
		case Skills::Bowmaster::Hurricane:
		case Skills::WindArcher::Hurricane:
		case Skills::Marksman::PiercingArrow:
		case Skills::FpArchMage::BigBang:
		case Skills::IlArchMage::BigBang:
		case Skills::Bishop::BigBang:
		case Skills::Corsair::RapidFire:
			SkillsPacket::endChargeOrStationarySkill(player, player->getChargeOrStationarySkillInfo());
			player->setChargeOrStationarySkill(ChargeOrStationarySkillInfo());
			break;
		default:
			if (player->getActiveBuffs()->getActiveSkillLevel(skillId) == 0) {
				// Hacking
				return;
			}
			player->getActiveBuffs()->removeBuff(skillId, fromTimer);
			if (GameLogicUtilities::isMobSkill(skillId)) {
				Buffs::endDebuff(player, static_cast<uint8_t>(skillId));
			}
			else {
				Buffs::endBuff(player, skillId);
			}

			if (skillId == Skills::SuperGm::Hide) {
				GmPacket::endHide(player);
				player->getMap()->gmHideChange(player);
			}

			break;
	}
}

auto Skills::getAffectedPartyMembers(Party *party, int8_t affected, int8_t members) -> const vector_t<Player *> {
	vector_t<Player *> ret;
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

auto Skills::useSkill(Player *player, PacketReader &packet) -> void {
	uint32_t ticks = packet.get<uint32_t>();
	int32_t skillId = packet.get<int32_t>();
	int16_t addedInfo = 0;
	uint8_t level = packet.get<uint8_t>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player->getSkills()->getSkillLevel(skillId) != level) {
		// Hacking
		return;
	}

	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	switch (skillId) {
		case Skills::Brawler::MpRecovery: {
			int16_t modHp = player->getStats()->getMaxHp() * skill->x / 100;
			int16_t healMp = modHp * skill->y / 100;
			player->getStats()->modifyHp(-modHp);
			player->getStats()->modifyMp(healMp);
			break;
		}
		case Skills::Shadower::Smokescreen: {
			Pos origin = packet.getClass<Pos>();
			Mist *m = new Mist(player->getMapId(), player, skill->time, skill->dimensions.move(player->getPos()), skillId, level);
			break;
		}
		case Skills::Corsair::Battleship:
			if (player->getActiveBuffs()->getBattleshipHp() == 0) {
				player->getActiveBuffs()->resetBattleshipHp();
			}
			break;
		case Skills::Crusader::ArmorCrash:
		case Skills::WhiteKnight::MagicCrash:
		case Skills::DragonKnight::PowerCrash: {
			// Might be CRC
			packet.skipBytes(4);
			uint8_t mobs = packet.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				int32_t mapMobId = packet.get<int32_t>();
				if (auto mob = player->getMap()->getMob(mapMobId)) {
					if (Randomizer::rand<uint16_t>(99) < skill->prop) {
						mob->doCrashSkill(skillId);
					}
				}
			}
			break;
		}
		case Skills::Hero::MonsterMagnet:
		case Skills::Paladin::MonsterMagnet:
		case Skills::DarkKnight::MonsterMagnet: {
			int32_t mobs = packet.get<int32_t>();
			for (int8_t k = 0; k < mobs; k++) {
				int32_t mapMobId = packet.get<int32_t>();
				uint8_t success = packet.get<int8_t>();
				SkillsPacket::showMagnetSuccess(player, mapMobId, success);
			}
			direction = packet.get<uint8_t>();
			break;
		}
		case Skills::FpWizard::Slow:
		case Skills::IlWizard::Slow:
		case Skills::BlazeWizard::Slow:
		case Skills::Page::Threaten:
			// Might be CRC
			packet.skipBytes(4);
			// Intentional fallthrough
		case Skills::FpMage::Seal:
		case Skills::IlMage::Seal:
		case Skills::BlazeWizard::Seal:
		case Skills::Priest::Doom:
		case Skills::Hermit::ShadowWeb:
		case Skills::NightWalker::ShadowWeb:
		case Skills::Shadower::NinjaAmbush:
		case Skills::NightLord::NinjaAmbush: {
			uint8_t mobs = packet.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (auto mob = player->getMap()->getMob(packet.get<int32_t>())) {
					MobHandler::handleMobStatus(player->getId(), mob, skillId, level, 0, 0);
				}
			}
			break;
		}
		case Skills::Bishop::HerosWill:
		case Skills::IlArchMage::HerosWill:
		case Skills::FpArchMage::HerosWill:
		case Skills::DarkKnight::HerosWill:
		case Skills::Hero::HerosWill:
		case Skills::Paladin::HerosWill:
		case Skills::NightLord::HerosWill:
		case Skills::Shadower::HerosWill:
		case Skills::Bowmaster::HerosWill:
		case Skills::Marksman::HerosWill:
		case Skills::Buccaneer::HerosWill:
		case Skills::Corsair::HerosWill:
			player->getActiveBuffs()->removeDebuff(MobSkills::Seduce);
			break;
		case Skills::Priest::Dispel: {
			int8_t affected = packet.get<int8_t>();
			player->getActiveBuffs()->useDispel();
			if (Party *party = player->getParty()) {
				const auto members = getAffectedPartyMembers(party, affected, party->getMembersCount());
				for (const auto &partyMember : members) {
					if (partyMember != nullptr && partyMember != player && partyMember->getMap() == player->getMap()) {
						if (Randomizer::rand<uint16_t>(99) < skill->prop) {
							SkillsPacket::showSkill(partyMember, skillId, level, direction, true, true);
							SkillsPacket::showSkill(partyMember, skillId, level, direction, true);
							partyMember->getActiveBuffs()->useDispel();
						}
					}
				}
			}
			packet.skipBytes(2);
			affected = packet.get<int8_t>();
			for (int8_t k = 0; k < affected; k++) {
				int32_t mapMobId = packet.get<int32_t>();
				if (auto mob = player->getMap()->getMob(mapMobId)) {
					if (Randomizer::rand<uint16_t>(99) < skill->prop) {
						mob->dispelBuffs();
					}
				}
			}
			break;
		}
		case Skills::Cleric::Heal: {
			uint16_t healRate = skill->hpProp;
			if (healRate > 100) {
				healRate = 100;
			}
			Party *party = player->getParty();
			int8_t partyPlayers = (party != nullptr ? party->getMembersCount() : 1);
			int32_t expIncrease = 0;

			int16_t heal = (healRate * player->getStats()->getMaxHp() / 100) / partyPlayers;

			if (party != nullptr) {
				const auto members = party->getPartyMembers(player->getMapId());
				for (const auto &partyMember : members) {
					int16_t chp = partyMember->getStats()->getHp();
					if (chp > 0 && chp < partyMember->getStats()->getMaxHp()) {
						partyMember->getStats()->modifyHp(heal);
						if (player != partyMember) {
							expIncrease += 20 * (partyMember->getStats()->getHp() - chp) / (8 * partyMember->getStats()->getLevel() + 190);
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
		case Skills::Fighter::Rage:
		case Skills::DawnWarrior::Rage:
		case Skills::Spearman::IronWill:
		case Skills::Spearman::HyperBody:
		case Skills::FpWizard::Meditation:
		case Skills::IlWizard::Meditation:
		case Skills::BlazeWizard::Meditation:
		case Skills::Cleric::Bless:
		case Skills::Priest::HolySymbol:
		case Skills::Bishop::Resurrection:
		case Skills::Bishop::HolyShield:
		case Skills::Bowmaster::SharpEyes:
		case Skills::Marksman::SharpEyes:
		case Skills::Assassin::Haste:
		case Skills::NightWalker::Haste:
		case Skills::Hermit::MesoUp:
		case Skills::Bandit::Haste:
		case Skills::Buccaneer::SpeedInfusion:
		case Skills::ThunderBreaker::SpeedInfusion:
		case Skills::Buccaneer::TimeLeap:
		case Skills::Hero::MapleWarrior:
		case Skills::Paladin::MapleWarrior:
		case Skills::DarkKnight::MapleWarrior:
		case Skills::FpArchMage::MapleWarrior:
		case Skills::IlArchMage::MapleWarrior:
		case Skills::Bishop::MapleWarrior:
		case Skills::Bowmaster::MapleWarrior:
		case Skills::Marksman::MapleWarrior:
		case Skills::NightLord::MapleWarrior:
		case Skills::Shadower::MapleWarrior:
		case Skills::Buccaneer::MapleWarrior:
		case Skills::Corsair::MapleWarrior: {
			if (skillId == Skills::Buccaneer::TimeLeap) {
				player->getSkills()->removeAllCooldowns();
			}
			if (Party *party = player->getParty()) {
				int8_t affected = packet.get<int8_t>();
				const auto members = getAffectedPartyMembers(party, affected, party->getMembersCount());
				for (const auto &partyMember : members) {
					if (partyMember != nullptr && partyMember != player && partyMember->getMap() == player->getMap()) {
						SkillsPacket::showSkill(partyMember, skillId, level, direction, true, true);
						SkillsPacket::showSkill(partyMember, skillId, level, direction, true);
						Buffs::addBuff(partyMember, skillId, level, 0);
						if (skillId == Skills::Buccaneer::TimeLeap) {
							partyMember->getSkills()->removeAllCooldowns();
						}
					}
				}
			}
			break;
		}
		case Skills::Beginner::EchoOfHero:
		case Skills::Noblesse::EchoOfHero:
		case Skills::SuperGm::Haste:
		case Skills::SuperGm::HolySymbol:
		case Skills::SuperGm::Bless:
		case Skills::SuperGm::HyperBody:
		case Skills::SuperGm::HealPlusDispel:
		case Skills::SuperGm::Resurrection: {
			uint8_t players = packet.get<int8_t>();
			function_t<bool(Player *)> doAction;
			function_t<void(Player *)> action;
			switch (skillId) {
				case Skills::SuperGm::HealPlusDispel:
					doAction = [](Player *target) { return !target->getStats()->isDead(); };
					action = [](Player *target) {
						target->getStats()->setHp(target->getStats()->getMaxHp());
						target->getStats()->setMp(target->getStats()->getMaxMp());
						target->getActiveBuffs()->useDispel();
					};
					break;
				case Skills::SuperGm::Resurrection:
					doAction = [](Player *target) { return target->getStats()->isDead(); };
					action = [](Player *target) { target->getStats()->setHp(target->getStats()->getMaxHp()); };
					break;
				default:
					doAction = [](Player *target) { return true; };
					action = [skillId, level](Player *target) { Buffs::addBuff(target, skillId, level, 0); };
			}
			for (uint8_t i = 0; i < players; i++) {
				int32_t playerId = packet.get<int32_t>();
				Player *target = PlayerDataProvider::getInstance().getPlayer(playerId);
				if (target != nullptr && target != player && doAction(target)) {
					SkillsPacket::showSkill(target, skillId, level, direction, true, true);
					SkillsPacket::showSkill(target, skillId, level, direction, true);
					action(target);
				}
			}
			break;
		}
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
		if (skillId == Skills::SuperGm::Hide) {
			GmPacket::beginHide(player);
			player->getMap()->gmHideChange(player);
		}

		return;
	}

	if (GameLogicUtilities::isSummon(skillId)) {
		SummonHandler::useSummon(player, skillId, level);
	}
}

auto Skills::applySkillCosts(Player *player, int32_t skillId, uint8_t level, bool elementalAmp) -> void {
	if (player->hasGmBenefits()) {
		// Ensure we don't lock, but don't actually use anything
		player->getStats()->setHp(player->getStats()->getHp(), true);
		player->getStats()->setMp(player->getStats()->getMp(), true);
		return;
	}

	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	int16_t coolTime = skill->coolTime;
	int16_t mpUse = skill->mp;
	int16_t hpUse = skill->hp;
	int16_t moneyConsume = skill->moneyConsume;
	int32_t item = skill->item;
	if (mpUse > 0) {
		if (auto concentrate = player->getActiveBuffs()->getActiveSkillInfo(Skills::Bowmaster::Concentrate)) {
			int16_t mpRate = concentrate->x;
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
	if (coolTime > 0 && skillId != Skills::Corsair::Battleship) {
		startCooldown(player, skillId, coolTime);
	}
	if (moneyConsume > 0) {
		int16_t minMesos = moneyConsume - (80 + level * 5);
		int16_t maxMesos = moneyConsume + (80 + level * 5);
		int16_t amount = Randomizer::rand<int16_t>(maxMesos, minMesos);
		int32_t mesos = player->getInventory()->getMesos();
		if (mesos - amount < 0) {
			// Hacking
			return;
		}
		player->getInventory()->modifyMesos(-amount);
	}
}

auto Skills::useAttackSkill(Player *player, int32_t skillId) -> void {
	if (skillId != Skills::All::RegularAttack) {
		uint8_t level = player->getSkills()->getSkillLevel(skillId);
		if (!SkillDataProvider::getInstance().isValidSkill(skillId) || level == 0) {
			return;
		}
		applySkillCosts(player, skillId, level, true);
	}
}

auto Skills::useAttackSkillRanged(Player *player, int32_t skillId, int16_t pos) -> void {
	uint8_t level = 0;
	if (skillId != Skills::All::RegularAttack) {
		level = player->getSkills()->getSkillLevel(skillId);
		if (!SkillDataProvider::getInstance().isValidSkill(skillId) || level == 0) {
			return;
		}
		applySkillCosts(player, skillId, level);
	}
	uint16_t hits = 1;
	if (skillId != Skills::All::RegularAttack) {
		uint16_t bullets = SkillDataProvider::getInstance().getSkill(skillId, level)->bulletConsume;
		if (bullets > 0) {
			hits = bullets;
		}
	}
	if (player->getActiveBuffs()->hasShadowPartner()) {
		hits *= 2;
	}
	if (pos > 0 && !(player->getActiveBuffs()->hasShadowStars() || player->getActiveBuffs()->hasSoulArrow()) && !player->hasGmBenefits()) {
		// If they don't have Shadow Stars or Soul Arrow, take the items
		Inventory::takeItemSlot(player, Inventories::UseInventory, pos, hits);
	}
}

auto Skills::heal(Player *player, int16_t value, int32_t skillId) -> void {
	if (player->getStats()->getHp() < player->getStats()->getMaxHp() && player->getStats()->getHp() > 0) {
		player->getStats()->modifyHp(value);
		SkillsPacket::healHp(player, value);
	}
}

auto Skills::hurt(Player *player, int16_t value, int32_t skillId) -> void {
	if (player->getStats()->getHp() - value > 1) {
		player->getStats()->modifyHp(-value);
		SkillsPacket::showSkillEffect(player, skillId);
	}
	else {
		Buffs::endBuff(player, skillId);
	}
}

auto Skills::startCooldown(Player *player, int32_t skillId, int16_t coolTime, bool initialLoad) -> void {
	if (isCooling(player, skillId)) {
		// Hacking
		return;
	}
	if (!initialLoad) {
		SkillsPacket::sendCooldown(player, skillId, coolTime);
		player->getSkills()->addCooldown(skillId, coolTime);
	}
	Timer::Timer::create([player, skillId](const time_point_t &now) { Skills::stopCooldown(player, skillId); },
		Timer::Id(Timer::Types::CoolTimer, skillId, 0),
		player->getTimerContainer(), seconds_t(coolTime));
}

auto Skills::stopCooldown(Player *player, int32_t skillId) -> void {
	player->getSkills()->removeCooldown(skillId);
	SkillsPacket::sendCooldown(player, skillId, 0);
	if (skillId == Skills::Corsair::Battleship) {
		player->getActiveBuffs()->resetBattleshipHp();
	}

	Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
	if (player->getTimerContainer()->isTimerRunning(id)) {
		player->getTimerContainer()->removeTimer(id);
	}
}

auto Skills::isCooling(Player *player, int32_t skillId) -> bool {
	Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
	return player->getTimerContainer()->isTimerRunning(id);
}

auto Skills::getCooldownTimeLeft(Player *player, int32_t skillId) -> int16_t {
	int16_t coolTime = 0;
	if (isCooling(player, skillId)) {
		Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
		coolTime = static_cast<int16_t>(player->getTimerContainer()->getRemainingTime<seconds_t>(id).count());
	}
	return coolTime;
}