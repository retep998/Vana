/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Common/GameLogicUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/PacketWrapper.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillConstants.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "ChannelServer/Buffs.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/GmPacket.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/MapPacket.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Mist.hpp"
#include "ChannelServer/MobHandler.hpp"
#include "ChannelServer/MysticDoor.hpp"
#include "ChannelServer/Party.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/SkillsPacket.hpp"
#include "ChannelServer/SummonHandler.hpp"
#include <functional>

namespace Vana {
namespace ChannelServer {

auto Skills::addSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	reader.skip<tick_count_t>();
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

auto Skills::cancelSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	int32_t identifier = reader.get<int32_t>();

	// Both buffs and "standing" skills e.g. Hurricane go through this packet
	// Handle "standing" skills here, otherwise dispatch to buffs

	switch (identifier) {
		case Vana::Skills::Bowmaster::Hurricane:
		case Vana::Skills::WindArcher::Hurricane:
		case Vana::Skills::Marksman::PiercingArrow:
		case Vana::Skills::FpArchMage::BigBang:
		case Vana::Skills::IlArchMage::BigBang:
		case Vana::Skills::Bishop::BigBang:
		case Vana::Skills::Corsair::RapidFire:
			player->sendMap(Packets::Skills::endChargeOrStationarySkill(player->getId(), player->getChargeOrStationarySkill()));
			player->setChargeOrStationarySkill(ChargeOrStationarySkillData{});
			return;
	}

	stopSkill(player, player->getActiveBuffs()->translateToSource(identifier));
}

auto Skills::stopSkill(ref_ptr_t<Player> player, const BuffSource &source, bool fromTimer) -> void {
	switch (source.getType()) {
		case BuffSourceType::Item:
		case BuffSourceType::Skill:
			if (source.getSkillLevel() == 0) {
				// Hacking
				return;
			}
			break;
		case BuffSourceType::MobSkill:
			if (source.getMobSkillLevel() == 0) {
				// Hacking
				return;
			}
			break;
	}

	Buffs::endBuff(player, source, fromTimer);

	if (source.getSkillId() == Vana::Skills::SuperGm::Hide) {
		player->send(Packets::Gm::endHide());
		player->getMap()->gmHideChange(player);
	}
}

auto Skills::getAffectedPartyMembers(Party *party, int8_t affected, int8_t members) -> const vector_t<ref_ptr_t<Player>> {
	vector_t<ref_ptr_t<Player>> ret;
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

auto Skills::useSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	reader.skip<tick_count_t>();
	skill_id_t skillId = reader.get<skill_id_t>();
	int16_t addedInfo = 0;
	skill_level_t level = reader.get<skill_level_t>();
	uint8_t type = 0;
	uint8_t direction = 0;
	if (level == 0 || player->getSkills()->getSkillLevel(skillId) != level) {
		// Hacking
		return;
	}

	if (player->getStats()->isDead()) {
		// Possibly hacking, possibly lag
		return;
	}

	auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level);
	if (skillId == Vana::Skills::Priest::MysticDoor) {
		Point origin = reader.get<Point>();
		MysticDoorResult result = player->getSkills()->openMysticDoor(origin, skill->buffTime);
		if (result == MysticDoorResult::Hacking) {
			return;
		}
		if (result == MysticDoorResult::NoSpace || result == MysticDoorResult::NoDoorPoints) {
			// TODO FIXME packet?
			// There's probably some packet to indicate failure
			return;
		}
	}

	if (applySkillCosts(player, skillId, level) == Result::Failure) {
		// Most likely hacking, could feasibly be lag
		return;
	}

	switch (skillId) {
		case Vana::Skills::Priest::MysticDoor:
			// Prevent the default case from executing, there's no packet data left for it
			break;
		case Vana::Skills::Brawler::MpRecovery: {
			health_t modHp = player->getStats()->getMaxHp() * skill->x / 100;
			health_t healMp = modHp * skill->y / 100;
			player->getStats()->modifyHp(-modHp);
			player->getStats()->modifyMp(healMp);
			break;
		}
		case Vana::Skills::Shadower::Smokescreen: {
			Point origin = reader.get<Point>();
			Mist *m = new Mist{
				player->getMapId(),
				player,
				skill->buffTime,
				skill->dimensions.move(player->getPos()),
				skillId,
				level};
			break;
		}
		case Vana::Skills::Corsair::Battleship:
			// TODO FIXME hacking? Remove?
			if (player->getActiveBuffs()->getBattleshipHp() == 0) {
				player->getActiveBuffs()->resetBattleshipHp();
			}
			break;
		case Vana::Skills::Crusader::ArmorCrash:
		case Vana::Skills::WhiteKnight::MagicCrash:
		case Vana::Skills::DragonKnight::PowerCrash: {
			// Might be CRC
			reader.unk<uint32_t>();
			uint8_t mobs = reader.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				map_object_t mapMobId = reader.get<map_object_t>();
				if (auto mob = player->getMap()->getMob(mapMobId)) {
					if (Randomizer::percentage<uint16_t>() < skill->prop) {
						mob->doCrashSkill(skillId);
					}
				}
			}
			break;
		}
		case Vana::Skills::Hero::MonsterMagnet:
		case Vana::Skills::Paladin::MonsterMagnet:
		case Vana::Skills::DarkKnight::MonsterMagnet: {
			int32_t mobs = reader.get<int32_t>();
			for (int32_t k = 0; k < mobs; k++) {
				map_object_t mapMobId = reader.get<map_object_t>();
				uint8_t success = reader.get<uint8_t>();
				player->sendMap(Packets::Skills::showMagnetSuccess(mapMobId, success));
			}
			direction = reader.get<uint8_t>();
			break;
		}
		case Vana::Skills::FpWizard::Slow:
		case Vana::Skills::IlWizard::Slow:
		case Vana::Skills::BlazeWizard::Slow:
		case Vana::Skills::Page::Threaten:
			// Might be CRC
			reader.unk<uint32_t>();
			// Intentional fallthrough
		case Vana::Skills::FpMage::Seal:
		case Vana::Skills::IlMage::Seal:
		case Vana::Skills::BlazeWizard::Seal:
		case Vana::Skills::Priest::Doom:
		case Vana::Skills::Hermit::ShadowWeb:
		case Vana::Skills::NightWalker::ShadowWeb:
		case Vana::Skills::Shadower::NinjaAmbush:
		case Vana::Skills::NightLord::NinjaAmbush: {
			uint8_t mobs = reader.get<uint8_t>();
			for (uint8_t k = 0; k < mobs; k++) {
				if (auto mob = player->getMap()->getMob(reader.get<int32_t>())) {
					MobHandler::handleMobStatus(player->getId(), mob, skillId, level, 0, 0);
				}
			}
			break;
		}
		case Vana::Skills::Bishop::HerosWill:
		case Vana::Skills::IlArchMage::HerosWill:
		case Vana::Skills::FpArchMage::HerosWill:
		case Vana::Skills::DarkKnight::HerosWill:
		case Vana::Skills::Hero::HerosWill:
		case Vana::Skills::Paladin::HerosWill:
		case Vana::Skills::NightLord::HerosWill:
		case Vana::Skills::Shadower::HerosWill:
		case Vana::Skills::Bowmaster::HerosWill:
		case Vana::Skills::Marksman::HerosWill:
		case Vana::Skills::Buccaneer::HerosWill:
		case Vana::Skills::Corsair::HerosWill:
			player->getActiveBuffs()->removeDebuff(MobSkills::Seduce);
			break;
		case Vana::Skills::Priest::Dispel: {
			int8_t affected = reader.get<int8_t>();
			player->getActiveBuffs()->usePlayerDispel();
			if (Party *party = player->getParty()) {
				const auto members = getAffectedPartyMembers(party, affected, party->getMembersCount());
				for (const auto &partyMember : members) {
					if (partyMember != nullptr && partyMember != player && partyMember->getMap() == player->getMap()) {
						if (Randomizer::percentage<uint16_t>() < skill->prop) {
							partyMember->send(Packets::Skills::showSkill(partyMember->getId(), skillId, level, direction, true, true));
							partyMember->sendMap(Packets::Skills::showSkill(partyMember->getId(), skillId, level, direction, true));
							partyMember->getActiveBuffs()->usePlayerDispel();
						}
					}
				}
			}

			reader.unk<int16_t>();

			affected = reader.get<int8_t>();
			for (int8_t k = 0; k < affected; k++) {
				map_object_t mapMobId = reader.get<map_object_t>();
				if (auto mob = player->getMap()->getMob(mapMobId)) {
					if (Randomizer::percentage<uint16_t>() < skill->prop) {
						mob->dispelBuffs();
					}
				}
			}
			break;
		}
		case Vana::Skills::Cleric::Heal: {
			uint16_t healRate = skill->hpProp;
			if (healRate > 100) {
				healRate = 100;
			}
			Party *party = player->getParty();
			int8_t partyPlayers = (party != nullptr ? party->getMembersCount() : 1);
			health_t heal = (healRate * player->getStats()->getMaxHp() / 100) / partyPlayers;

			if (party != nullptr) {
				experience_t expIncrease = 0;
				const auto members = party->getPartyMembers(player->getMapId());
				for (const auto &partyMember : members) {
					health_t chp = partyMember->getStats()->getHp();
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
		case Vana::Skills::Fighter::Rage:
		case Vana::Skills::DawnWarrior::Rage:
		case Vana::Skills::Spearman::IronWill:
		case Vana::Skills::Spearman::HyperBody:
		case Vana::Skills::FpWizard::Meditation:
		case Vana::Skills::IlWizard::Meditation:
		case Vana::Skills::BlazeWizard::Meditation:
		case Vana::Skills::Cleric::Bless:
		case Vana::Skills::Priest::HolySymbol:
		case Vana::Skills::Bishop::Resurrection:
		case Vana::Skills::Bishop::HolyShield:
		case Vana::Skills::Bowmaster::SharpEyes:
		case Vana::Skills::Marksman::SharpEyes:
		case Vana::Skills::Assassin::Haste:
		case Vana::Skills::NightWalker::Haste:
		case Vana::Skills::Hermit::MesoUp:
		case Vana::Skills::Bandit::Haste:
		case Vana::Skills::Buccaneer::SpeedInfusion:
		case Vana::Skills::ThunderBreaker::SpeedInfusion:
		case Vana::Skills::Buccaneer::TimeLeap:
		case Vana::Skills::Hero::MapleWarrior:
		case Vana::Skills::Paladin::MapleWarrior:
		case Vana::Skills::DarkKnight::MapleWarrior:
		case Vana::Skills::FpArchMage::MapleWarrior:
		case Vana::Skills::IlArchMage::MapleWarrior:
		case Vana::Skills::Bishop::MapleWarrior:
		case Vana::Skills::Bowmaster::MapleWarrior:
		case Vana::Skills::Marksman::MapleWarrior:
		case Vana::Skills::NightLord::MapleWarrior:
		case Vana::Skills::Shadower::MapleWarrior:
		case Vana::Skills::Buccaneer::MapleWarrior:
		case Vana::Skills::Corsair::MapleWarrior: {
			if (skillId == Vana::Skills::Buccaneer::TimeLeap) {
				player->getSkills()->removeAllCooldowns();
			}
			if (Party *party = player->getParty()) {
				int8_t affected = reader.get<int8_t>();
				const auto members = getAffectedPartyMembers(party, affected, party->getMembersCount());
				for (const auto &partyMember : members) {
					if (partyMember != nullptr && partyMember != player && partyMember->getMap() == player->getMap()) {
						partyMember->send(Packets::Skills::showSkill(partyMember->getId(), skillId, level, direction, true, true));
						partyMember->sendMap(Packets::Skills::showSkill(partyMember->getId(), skillId, level, direction, true));
						Buffs::addBuff(partyMember, skillId, level, 0);
						if (skillId == Vana::Skills::Buccaneer::TimeLeap) {
							partyMember->getSkills()->removeAllCooldowns();
						}
					}
				}
			}
			break;
		}

		case Vana::Skills::Beginner::EchoOfHero:
		case Vana::Skills::Noblesse::EchoOfHero:
		case Vana::Skills::SuperGm::Haste:
		case Vana::Skills::SuperGm::HolySymbol:
		case Vana::Skills::SuperGm::Bless:
		case Vana::Skills::SuperGm::HyperBody:
		case Vana::Skills::SuperGm::HealPlusDispel:
		case Vana::Skills::SuperGm::Resurrection: {
			uint8_t players = reader.get<uint8_t>();
			function_t<bool(ref_ptr_t<Player>)> doAction;
			function_t<void(ref_ptr_t<Player>)> action;
			switch (skillId) {
				case Vana::Skills::SuperGm::HealPlusDispel:
					doAction = [](ref_ptr_t<Player> target) { return !target->getStats()->isDead(); };
					action = [](ref_ptr_t<Player> target) {
						target->getStats()->setHp(target->getStats()->getMaxHp());
						target->getStats()->setMp(target->getStats()->getMaxMp());
						target->getActiveBuffs()->usePlayerDispel();
					};
					break;
				case Vana::Skills::SuperGm::Resurrection:
					doAction = [](ref_ptr_t<Player> target) { return target->getStats()->isDead(); };
					action = [](ref_ptr_t<Player> target) {
						target->getStats()->setHp(target->getStats()->getMaxHp());
					};
					break;
				default:
					doAction = [](ref_ptr_t<Player> target) { return true; };
					action = [skillId, level](ref_ptr_t<Player> target) {
						Buffs::addBuff(target, skillId, level, 0);
					};
			}
			for (uint8_t i = 0; i < players; i++) {
				player_id_t playerId = reader.get<player_id_t>();
				auto target = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerId);
				if (target != nullptr && target != player && doAction(target)) {
					player->send(Packets::Skills::showSkill(player->getId(), skillId, level, direction, true, true));
					player->sendMap(Packets::Skills::showSkill(player->getId(), skillId, level, direction, true));

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

	player->sendMap(Packets::Skills::showSkill(player->getId(), skillId, level, direction));

	if (Buffs::addBuff(player, skillId, level, addedInfo) == Result::Successful) {
		if (skillId == Vana::Skills::SuperGm::Hide) {
			player->send(Packets::Gm::beginHide());
			player->getMap()->gmHideChange(player);
		}

		return;
	}

	if (GameLogicUtilities::isSummon(skillId)) {
		Point pos = reader.get<Point>(); // Useful?
		SummonHandler::useSummon(player, skillId, level);
	}
}

auto Skills::applySkillCosts(ref_ptr_t<Player> player, skill_id_t skillId, skill_level_t level, bool elementalAmp) -> Result {
	if (player->hasGmBenefits()) {
		// Ensure we don't lock, but don't actually use anything
		player->getStats()->setHp(player->getStats()->getHp(), true);
		player->getStats()->setMp(player->getStats()->getMp(), true);
		return Result::Successful;
	}

	auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level);
	seconds_t coolTime = skill->coolTime;
	health_t mpUse = skill->mp;
	health_t hpUse = skill->hp;
	int16_t moneyConsume = skill->moneyConsume;
	item_id_t item = skill->item;
	if (mpUse > 0) {
		auto concentrate = player->getActiveBuffs()->getConcentrateSource();
		if (concentrate.is_initialized()) {
			auto skill = player->getActiveBuffs()->getBuffSkillInfo(concentrate.get());
			mpUse = (mpUse * skill->x) / 100;
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
	if (coolTime.count() > 0 && skillId != Vana::Skills::Corsair::Battleship) {
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

auto Skills::useAttackSkill(ref_ptr_t<Player> player, skill_id_t skillId) -> Result {
	if (skillId != Vana::Skills::All::RegularAttack) {
		skill_level_t level = player->getSkills()->getSkillLevel(skillId);
		if (!ChannelServer::getInstance().getSkillDataProvider().isValidSkill(skillId) || level == 0) {
			return Result::Failure;
		}
		return applySkillCosts(player, skillId, level, true);
	}
	return Result::Successful;
}

auto Skills::useAttackSkillRanged(ref_ptr_t<Player> player, skill_id_t skillId, inventory_slot_t projectilePos, inventory_slot_t cashProjectilePos, item_id_t projectileId) -> Result {
	skill_level_t level = 0;
	if (skillId != Vana::Skills::All::RegularAttack) {
		level = player->getSkills()->getSkillLevel(skillId);
		if (!ChannelServer::getInstance().getSkillDataProvider().isValidSkill(skillId) || level == 0) {
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
	if (skillId != Vana::Skills::All::RegularAttack) {
		auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level);
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

auto Skills::heal(ref_ptr_t<Player> player, int64_t value, const BuffSource &source) -> void {
	if (player->getStats()->getHp() < player->getStats()->getMaxHp() && player->getStats()->getHp() > 0) {
		health_t val = static_cast<health_t>(value);
		player->getStats()->modifyHp(val);
		player->send(Packets::Skills::healHp(val));
	}
}

auto Skills::hurt(ref_ptr_t<Player> player, int64_t value, const BuffSource &source) -> void {
	health_t val = static_cast<health_t>(value);
	if (source.getType() != BuffSourceType::Skill) throw NotImplementedException{"hurt BuffSourceType"};
	if (player->getStats()->getHp() - val > 1) {
		player->getStats()->modifyHp(-val);
		player->sendMap(Packets::Skills::showSkillEffect(player->getId(), source.getSkillId()));
	}
	else {
		Buffs::endBuff(player, source);
	}
}

auto Skills::startCooldown(ref_ptr_t<Player> player, skill_id_t skillId, seconds_t coolTime, bool initialLoad) -> void {
	if (isCooling(player, skillId)) {
		// Hacking
		return;
	}
	if (!initialLoad) {
		player->send(Packets::Skills::sendCooldown(skillId, coolTime));
		player->getSkills()->addCooldown(skillId, coolTime);
	}
	Vana::Timer::Timer::create(
		[player, skillId](const time_point_t &now) {
			Skills::stopCooldown(player, skillId);
		},
		Vana::Timer::Id{TimerType::CoolTimer, skillId},
		player->getTimerContainer(),
		seconds_t{coolTime});
}

auto Skills::stopCooldown(ref_ptr_t<Player> player, skill_id_t skillId) -> void {
	player->getSkills()->removeCooldown(skillId);
	player->send(Packets::Skills::sendCooldown(skillId, seconds_t{0}));
	if (skillId == Vana::Skills::Corsair::Battleship) {
		player->getActiveBuffs()->resetBattleshipHp();
	}

	Vana::Timer::Id id{TimerType::CoolTimer, skillId};
	auto container = player->getTimerContainer();
	if (container->isTimerRunning(id)) {
		container->removeTimer(id);
	}
}

auto Skills::isCooling(ref_ptr_t<Player> player, skill_id_t skillId) -> bool {
	Vana::Timer::Id id{TimerType::CoolTimer, skillId};
	return player->getTimerContainer()->isTimerRunning(id);
}

auto Skills::getCooldownTimeLeft(ref_ptr_t<Player> player, skill_id_t skillId) -> int16_t {
	int16_t coolTime = 0;
	if (isCooling(player, skillId)) {
		Vana::Timer::Id id{TimerType::CoolTimer, skillId};
		coolTime = static_cast<int16_t>(player->getTimerContainer()->getRemainingTime<seconds_t>(id).count());
	}
	return coolTime;
}

}
}