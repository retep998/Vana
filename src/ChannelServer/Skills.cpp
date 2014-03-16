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
#include "PacketWrapper.hpp"
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

auto Skills::addSkill(Player *player, PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<tick_count_t>();
	skill_id_t skillId = reader.get<skill_id_t>();
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

auto Skills::cancelSkill(Player *player, PacketReader &reader) -> void {
	stopSkill(player, reader.get<skill_id_t>());
}

auto Skills::stopSkill(Player *player, skill_id_t skillId, bool fromTimer) -> void {
	switch (skillId) {
		case Skills::Bowmaster::Hurricane:
		case Skills::WindArcher::Hurricane:
		case Skills::Marksman::PiercingArrow:
		case Skills::FpArchMage::BigBang:
		case Skills::IlArchMage::BigBang:
		case Skills::Bishop::BigBang:
		case Skills::Corsair::RapidFire:
			player->sendMap(SkillsPacket::endChargeOrStationarySkill(player->getId(), player->getChargeOrStationarySkillInfo()));
			player->setChargeOrStationarySkill(ChargeOrStationarySkillInfo());
			break;
		default:
			if (player->getActiveBuffs()->getActiveSkillLevel(skillId) == 0) {
				// Hacking
				return;
			}
			player->getActiveBuffs()->removeBuff(skillId, fromTimer);
			if (GameLogicUtilities::isMobSkill(skillId)) {
				Buffs::endDebuff(player, static_cast<mob_skill_id_t>(skillId));
			}
			else {
				Buffs::endBuff(player, skillId);
			}

			if (skillId == Skills::SuperGm::Hide) {
				player->send(GmPacket::endHide());
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

auto Skills::useSkill(Player *player, PacketReader &reader) -> void {
	tick_count_t ticks = reader.get<tick_count_t>();
	skill_id_t skillId = reader.get<skill_id_t>();
	int16_t addedInfo = 0;
	skill_level_t level = reader.get<skill_level_t>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player->getSkills()->getSkillLevel(skillId) != level) {
		// Hacking
		return;
	}

	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	if (applySkillCosts(player, skillId, level) == Result::Failure) {
		// Most likely hacking, could feasibly be lag
		return;
	}

	switch (skillId) {
		case Skills::Brawler::MpRecovery: {
			int16_t modHp = player->getStats()->getMaxHp() * skill->x / 100;
			int16_t healMp = modHp * skill->y / 100;
			player->getStats()->modifyHp(-modHp);
			player->getStats()->modifyMp(healMp);
			break;
		}
		case Skills::Shadower::Smokescreen: {
			Pos origin = reader.get<Pos>();
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
			reader.skipBytes(4);
			uint8_t mobs = reader.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				map_object_t mapMobId = reader.get<map_object_t>();
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
			int32_t mobs = reader.get<int32_t>();
			for (int8_t k = 0; k < mobs; k++) {
				map_object_t mapMobId = reader.get<map_object_t>();
				uint8_t success = reader.get<int8_t>();
				player->sendMap(SkillsPacket::showMagnetSuccess(mapMobId, success));
			}
			direction = reader.get<uint8_t>();
			break;
		}
		case Skills::FpWizard::Slow:
		case Skills::IlWizard::Slow:
		case Skills::BlazeWizard::Slow:
		case Skills::Page::Threaten:
			// Might be CRC
			reader.skipBytes(4);
			// Intentional fallthrough
		case Skills::FpMage::Seal:
		case Skills::IlMage::Seal:
		case Skills::BlazeWizard::Seal:
		case Skills::Priest::Doom:
		case Skills::Hermit::ShadowWeb:
		case Skills::NightWalker::ShadowWeb:
		case Skills::Shadower::NinjaAmbush:
		case Skills::NightLord::NinjaAmbush: {
			uint8_t mobs = reader.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (auto mob = player->getMap()->getMob(reader.get<int32_t>())) {
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
			int8_t affected = reader.get<int8_t>();
			player->getActiveBuffs()->useDispel();
			if (Party *party = player->getParty()) {
				const auto members = getAffectedPartyMembers(party, affected, party->getMembersCount());
				for (const auto &partyMember : members) {
					if (partyMember != nullptr && partyMember != player && partyMember->getMap() == player->getMap()) {
						if (Randomizer::rand<uint16_t>(99) < skill->prop) {
							partyMember->send(SkillsPacket::showSkill(partyMember->getId(), skillId, level, direction, true, true));
							partyMember->sendMap(SkillsPacket::showSkill(partyMember->getId(), skillId, level, direction, true));
							partyMember->getActiveBuffs()->useDispel();
						}
					}
				}
			}
			reader.skipBytes(2);
			affected = reader.get<int8_t>();
			for (int8_t k = 0; k < affected; k++) {
				map_object_t mapMobId = reader.get<map_object_t>();
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
				int8_t affected = reader.get<int8_t>();
				const auto members = getAffectedPartyMembers(party, affected, party->getMembersCount());
				for (const auto &partyMember : members) {
					if (partyMember != nullptr && partyMember != player && partyMember->getMap() == player->getMap()) {
						partyMember->send(SkillsPacket::showSkill(partyMember->getId(), skillId, level, direction, true, true));
						partyMember->sendMap(SkillsPacket::showSkill(partyMember->getId(), skillId, level, direction, true));
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
			uint8_t players = reader.get<int8_t>();
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
				player_id_t playerId = reader.get<player_id_t>();
				Player *target = PlayerDataProvider::getInstance().getPlayer(playerId);
				if (target != nullptr && target != player && doAction(target)) {
					player->send(SkillsPacket::showSkill(player->getId(), skillId, level, direction, true, true));

					player->sendMap(SkillsPacket::showSkill(player->getId(), skillId, level, direction, true));

					action(target);
				}
			}
			break;
		}
		default:
			type = reader.get<int8_t>();
			switch (type) {
				case 0x80:
					addedInfo = reader.get<int16_t>();
					break;
			}
			break;
	}

	player->sendMap(SkillsPacket::showSkill(player->getId(), skillId, level, direction));

	if (Buffs::addBuff(player, skillId, level, addedInfo) == Result::Successful) {
		if (skillId == Skills::SuperGm::Hide) {
			player->send(GmPacket::beginHide());
			player->getMap()->gmHideChange(player);
		}

		return;
	}

	if (GameLogicUtilities::isSummon(skillId)) {
		Pos pos = reader.get<Pos>(); // Useful?
		SummonHandler::useSummon(player, skillId, level);
	}
}

auto Skills::applySkillCosts(Player *player, skill_id_t skillId, skill_level_t level, bool elementalAmp) -> Result {
	if (player->hasGmBenefits()) {
		// Ensure we don't lock, but don't actually use anything
		player->getStats()->setHp(player->getStats()->getHp(), true);
		player->getStats()->setMp(player->getStats()->getMp(), true);
		return Result::Successful;
	}

	auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	int16_t coolTime = skill->coolTime;
	int16_t mpUse = skill->mp;
	int16_t hpUse = skill->hp;
	int16_t moneyConsume = skill->moneyConsume;
	item_id_t item = skill->item;
	if (mpUse > 0) {
		if (auto concentrate = player->getActiveBuffs()->getActiveSkillInfo(Skills::Bowmaster::Concentrate)) {
			mpUse = (mpUse * concentrate->x) / 100;
		}
		else if (elementalAmp && player->getSkills()->hasElementalAmp()) {
			mpUse = (mpUse * player->getSkills()->getSkillInfo(player->getSkills()->getElementalAmp())->x) / 100;
		}

		if (player->getStats()->getMp() < mpUse) {
			return Result::Failure;
		}
		player->getStats()->modifyMp(-mpUse, true);
	}
	else {
		player->getStats()->setMp(player->getStats()->getMp(), true);
	}
	if (hpUse > 0) {
		if (player->getStats()->getHp() < hpUse) {
			return Result::Failure;
		}
		player->getStats()->modifyHp(-hpUse);
	}
	if (item > 0) {
		if (player->getInventory()->getItemAmount(item) < skill->itemCount) {
			return Result::Failure;
		}
		Inventory::takeItem(player, item, skill->itemCount);
	}
	if (coolTime > 0 && skillId != Skills::Corsair::Battleship) {
		if (isCooling(player, skillId)) {
			return Result::Failure;
		}
		startCooldown(player, skillId, coolTime);
	}

	if (moneyConsume > 0) {
		int16_t minMesos = moneyConsume - (80 + level * 5);
		int16_t maxMesos = moneyConsume + (80 + level * 5);
		int16_t amount = Randomizer::rand<int16_t>(maxMesos, minMesos);
		mesos_t mesos = player->getInventory()->getMesos();
		if (mesos - amount < 0) {
			// Hacking
			return Result::Failure;
		}
		player->getInventory()->modifyMesos(-amount);
	}

	return Result::Successful;
}

auto Skills::useAttackSkill(Player *player, skill_id_t skillId) -> Result {
	if (skillId != Skills::All::RegularAttack) {
		skill_level_t level = player->getSkills()->getSkillLevel(skillId);
		if (!SkillDataProvider::getInstance().isValidSkill(skillId) || level == 0) {
			return Result::Failure;
		}
		return applySkillCosts(player, skillId, level, true);
	}
	return Result::Successful;
}

auto Skills::useAttackSkillRanged(Player *player, skill_id_t skillId, inventory_slot_t projectilePos, inventory_slot_t cashProjectilePos, item_id_t projectileId) -> Result {
	skill_level_t level = 0;
	if (skillId != Skills::All::RegularAttack) {
		level = player->getSkills()->getSkillLevel(skillId);
		if (!SkillDataProvider::getInstance().isValidSkill(skillId) || level == 0) {
			return Result::Failure;
		}
		if (applySkillCosts(player, skillId, level) == Result::Failure) {
			return Result::Failure;
		}
	}

	if (player->hasGmBenefits()) {
		return Result::Successful;
	}

	switch (GameLogicUtilities::getJobTrack(player->getStats()->getJob())) {
		case Jobs::JobTracks::Bowman:
		case Jobs::JobTracks::WindArcher:
			if (player->getActiveBuffs()->hasSoulArrow()) {
				return Result::Successful;
			}
			if (!GameLogicUtilities::isArrow(projectileId)) {
				return Result::Failure;
			}
			break;
		case Jobs::JobTracks::Thief:
		case Jobs::JobTracks::NightWalker:
			if (player->getActiveBuffs()->hasShadowStars()) {
				return Result::Successful;
			}
			if (cashProjectilePos > 0) {
				Item *cashItem = player->getInventory()->getItem(Inventories::CashInventory, cashProjectilePos);
				if (cashItem == nullptr || cashItem->getId() != projectileId) {
					return Result::Failure;
				}

				Item *projectile = player->getInventory()->getItem(Inventories::UseInventory, projectilePos);
				if (projectile == nullptr) {
					return Result::Failure;
				}

				projectileId = projectile->getId();
			}
			if (!GameLogicUtilities::isStar(projectileId)) {
				return Result::Failure;
			}
			break;
		case Jobs::JobTracks::Pirate:
			if (!GameLogicUtilities::isBullet(projectileId)) {
				return Result::Failure;
			}
			break;
	}

	if (projectilePos <= 0) {
		return Result::Failure;
	}

	Item *projectile = player->getInventory()->getItem(Inventories::UseInventory, projectilePos);
	if (projectile == nullptr || projectile->getId() != projectileId) {
		return Result::Failure;
	}

	slot_qty_t hits = 1;
	if (skillId != Skills::All::RegularAttack) {
		auto skill = SkillDataProvider::getInstance().getSkill(skillId, level);
		item_id_t optionalItem = skill->optionalItem;

		if (optionalItem != 0 && optionalItem == projectileId) {
			if (projectile->getAmount() < skill->itemCount) {
				return Result::Failure;
			}
			Inventory::takeItemSlot(player, Inventories::UseInventory, projectilePos, skill->itemCount);
			return Result::Successful;
		}

		slot_qty_t bullets = skill->bulletConsume;
		if (bullets > 0) {
			hits = bullets;
		}
	}

	if (player->getActiveBuffs()->hasShadowPartner()) {
		hits *= 2;
	}

	if (projectile->getAmount() < hits) {
		return Result::Failure;
	}

	Inventory::takeItemSlot(player, Inventories::UseInventory, projectilePos, hits);
	return Result::Successful;
}

auto Skills::heal(Player *player, int16_t value, skill_id_t skillId) -> void {
	if (player->getStats()->getHp() < player->getStats()->getMaxHp() && player->getStats()->getHp() > 0) {
		player->getStats()->modifyHp(value);
		player->send(SkillsPacket::healHp(value));
	}
}

auto Skills::hurt(Player *player, int16_t value, skill_id_t skillId) -> void {
	if (player->getStats()->getHp() - value > 1) {
		player->getStats()->modifyHp(-value);
		player->sendMap(SkillsPacket::showSkillEffect(player->getId(), skillId));
	}
	else {
		Buffs::endBuff(player, skillId);
	}
}

auto Skills::startCooldown(Player *player, skill_id_t skillId, int16_t coolTime, bool initialLoad) -> void {
	if (isCooling(player, skillId)) {
		// Hacking
		return;
	}
	if (!initialLoad) {
		player->send(SkillsPacket::sendCooldown(skillId, coolTime));
		player->getSkills()->addCooldown(skillId, coolTime);
	}
	Timer::Timer::create([player, skillId](const time_point_t &now) { Skills::stopCooldown(player, skillId); },
		Timer::Id(Timer::Types::CoolTimer, skillId, 0),
		player->getTimerContainer(), seconds_t(coolTime));
}

auto Skills::stopCooldown(Player *player, skill_id_t skillId) -> void {
	player->getSkills()->removeCooldown(skillId);
	player->send(SkillsPacket::sendCooldown(skillId, 0));
	if (skillId == Skills::Corsair::Battleship) {
		player->getActiveBuffs()->resetBattleshipHp();
	}

	Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
	if (player->getTimerContainer()->isTimerRunning(id)) {
		player->getTimerContainer()->removeTimer(id);
	}
}

auto Skills::isCooling(Player *player, skill_id_t skillId) -> bool {
	Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
	return player->getTimerContainer()->isTimerRunning(id);
}

auto Skills::getCooldownTimeLeft(Player *player, skill_id_t skillId) -> int16_t {
	int16_t coolTime = 0;
	if (isCooling(player, skillId)) {
		Timer::Id id(Timer::Types::CoolTimer, skillId, 0);
		coolTime = static_cast<int16_t>(player->getTimerContainer()->getRemainingTime<seconds_t>(id).count());
	}
	return coolTime;
}