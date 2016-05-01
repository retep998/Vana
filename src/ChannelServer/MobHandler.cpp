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
#include "MobHandler.hpp"
#include "Common/Algorithm.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/MobConstants.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Point.hpp"
#include "Common/Randomizer.hpp"
#include "Common/SkillDataProvider.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Instance.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Mob.hpp"
#include "ChannelServer/MobsPacket.hpp"
#include "ChannelServer/MovementHandler.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/PlayerPacket.hpp"
#include "ChannelServer/StatusInfo.hpp"
#include <functional>

namespace Vana {
namespace ChannelServer {

auto MobHandler::handleBomb(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	map_object_t mobId = reader.get<map_object_t>();
	auto mob = player->getMap()->getMob(mobId);
	if (player->getStats()->isDead() || mob == nullptr) {
		return;
	}
	if (mob->getSelfDestructHp() == 0) {
		// Hacking, I think
		return;
	}
	mob->explode();
}

auto MobHandler::friendlyDamaged(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	map_object_t mobFrom = reader.get<map_object_t>();
	player_id_t playerId = reader.get<player_id_t>();
	map_object_t mobTo = reader.get<map_object_t>();

	Map *map = player->getMap();
	auto dealer = map->getMob(mobFrom);
	auto taker = map->getMob(mobTo);
	if (dealer != nullptr && taker != nullptr && taker->isFriendly()) {
		damage_t damage = dealer->getLevel() * Randomizer::rand<int32_t>(100) / 10;
		// Temp for now until I figure out something more effective
		// TODO FIXME: Formula
		mob_id_t mobId = taker->getMobId();
		map_object_t mapMobId = taker->getMapMobId();
		int32_t mobHp = std::max(0, taker->getHp() - damage);
		int32_t maxHp = taker->getMaxHp();

		taker->applyDamage(playerId, damage);
		if (Instance *instance = map->getInstance()) {
			instance->friendlyMobHit(mobId, mapMobId, map->getId(), mobHp, maxHp);
		}
	}
}

auto MobHandler::handleTurncoats(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	map_object_t mobFrom = reader.get<map_object_t>();
	player_id_t playerId = reader.get<player_id_t>();
	map_object_t mobTo = reader.get<map_object_t>();
	reader.skip<uint8_t>(); // Same as player damage, -1 = bump, integer = skill ID
	damage_t damage = reader.get<damage_t>();
	reader.skip<uint8_t>(); // Facing direction
	reader.unk<uint32_t>(); // Some type of pos, damage display, I think

	Map *map = player->getMap();
	auto damager = map->getMob(mobFrom);
	auto taker = map->getMob(mobTo);
	if (damager != nullptr && taker != nullptr) {
		taker->applyDamage(playerId, damage);
	}
}

auto MobHandler::monsterControl(ref_ptr_t<Player> player, PacketReader &reader) -> void {
	map_object_t mobId = reader.get<map_object_t>();

	Map *map = player->getMap();
	auto mob = map->getMob(mobId);
	if (mob == nullptr || mob->getControlStatus() == MobControlStatus::None) {
		return;
	}

	int16_t moveId = reader.get<int16_t>();
	if (mob->getController() != player && !mob->getSkillFeasibility()) {
		map->switchController(mob, player);
	}

	int8_t nibbles = reader.get<int8_t>();
	int8_t rawActivity = reader.get<int8_t>();
	mob_skill_id_t useSkillId = reader.get<mob_skill_id_t>();
	mob_skill_level_t useSkillLevel = reader.get<mob_skill_level_t>();
	int16_t option = reader.get<int16_t>();
	reader.unk<uint8_t>();
	reader.unk<uint32_t>(); // 4 bytes of always 1 or always 0?
	reader.unk<uint32_t>(); // Pos?

	// TODO FIXME mob.get() - perhaps movement parsing should be on the MovableLife class itself?
	MovementHandler::parseMovement(mob.get(), reader);

	int8_t parsedActivity = rawActivity;
	if (parsedActivity >= 0) {
		parsedActivity = static_cast<int8_t>(static_cast<uint8_t>(parsedActivity) >> 1);
	}
	bool isAttack = ext::in_range_inclusive<int8_t>(parsedActivity, 12, 20);
	bool isSkill = ext::in_range_inclusive<int8_t>(parsedActivity, 21, 25);
	mob_skill_id_t attackId = isAttack ? parsedActivity - 12 : -1;
	bool nextMovementCouldBeSkill = (nibbles & 0x0F) != 0;
	bool unk = (nibbles & 0xF0) != 0;

	mob_skill_id_t nextCastSkill = 0;
	mob_skill_level_t nextCastSkillLevel = 0;

	if (isAttack || isSkill) {
		if (isAttack) {
			auto attack = ChannelServer::getInstance().getMobDataProvider().getMobAttack(mob->getMobIdOrLink(), attackId);
			if (attack == nullptr) {
				// Hacking
				return;
			}
			mob->consumeMp(attack->mpConsume);
		}
		else {
			if (useSkillId != mob->getAnticipatedSkill() || useSkillLevel != mob->getAnticipatedSkillLevel()) {
				// Hacking?
				mob->resetAnticipatedSkill();
				return;
			}
			if (mob->useAnticipatedSkill() == Result::Failure) {
				return;
			}
		}
	}

	mob->setSkillFeasibility(nextMovementCouldBeSkill);
	if (nextMovementCouldBeSkill) {
		mob->chooseRandomSkill(player, nextCastSkill, nextCastSkillLevel);
	}

	player->send(Packets::Mobs::moveMobResponse(mobId, moveId, nextMovementCouldBeSkill, mob->getMp(), nextCastSkill, nextCastSkillLevel));
	reader.reset(19);
	player->sendMap(Packets::Mobs::moveMob(mobId, nextMovementCouldBeSkill, rawActivity, useSkillId, useSkillLevel, option, reader.getBuffer(), reader.getBufferLength()), true);
}

auto MobHandler::handleMobStatus(player_id_t playerId, ref_ptr_t<Mob> mob, skill_id_t skillId, skill_level_t level, item_id_t weapon, int8_t hits, damage_t damage) -> int32_t {
	auto player = ChannelServer::getInstance().getPlayerDataProvider().getPlayer(playerId);
	vector_t<StatusInfo> statuses;
	int16_t y = 0;
	auto skill = ChannelServer::getInstance().getSkillDataProvider().getSkill(skillId, level);
	bool success = (skillId == 0 ? false : (Randomizer::percentage<uint16_t>() < skill->prop));
	if (mob->canFreeze()) {
		// Freezing stuff
		switch (skillId) {
			case Vana::Skills::IlWizard::ColdBeam:
			case Vana::Skills::IlMage::IceStrike:
			case Vana::Skills::IlMage::ElementComposition:
			case Vana::Skills::Sniper::Blizzard:
			case Vana::Skills::IlArchMage::Blizzard:
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->buffTime);
				break;
			case Vana::Skills::Outlaw::IceSplitter:
				if (auto elementalBoost = player->getSkills()->getSkillInfo(Vana::Skills::Corsair::ElementalBoost)) {
					y = elementalBoost->y;
				}
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, seconds_t{skill->buffTime.count() + y});
				break;
			case Vana::Skills::FpArchMage::Elquines:
			case Vana::Skills::Marksman::Frostprey:
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, seconds_t{skill->x});
				break;
		}
		if ((GameLogicUtilities::isSword(weapon) || GameLogicUtilities::isMace(weapon)) && player->getActiveBuffs()->hasIceCharge()) {
			// Ice charges
			auto source = player->getActiveBuffs()->getChargeSource();
			auto &buffSource = source.get();
			if (buffSource.getType() != BuffSourceType::Skill) throw NotImplementedException{"Charge BuffSourceType"};
			auto skill = player->getActiveBuffs()->getBuffSkillInfo(buffSource);
			statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, buffSource.getSkillId(), seconds_t{skill->y});
		}
	}
	if (mob->canPoison() && mob->getHp() > 1) {
		// Poisoning stuff
		switch (skillId) {
			case Vana::Skills::All::RegularAttack: // Venomous Star/Stab
			case Vana::Skills::Rogue::LuckySeven:
			case Vana::Skills::Hermit::Avenger:
			case Vana::Skills::NightLord::TripleThrow:
			case Vana::Skills::Rogue::DoubleStab:
			case Vana::Skills::Rogue::Disorder:
			case Vana::Skills::Bandit::SavageBlow:
			case Vana::Skills::ChiefBandit::Assaulter:
			case Vana::Skills::Shadower::Assassinate:
			case Vana::Skills::Shadower::BoomerangStep:
			case Vana::Skills::NightWalker::Disorder:
				if (player->getSkills()->hasVenomousWeapon() && mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount) {
					// MAX = (18.5 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					// MIN = (8.0 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					skill_id_t vSkill = player->getSkills()->getVenomousWeapon();
					auto venom = player->getSkills()->getSkillInfo(vSkill);

					int32_t part1 = player->getStats()->getStr(true) + player->getStats()->getLuk(true);
					int32_t part2 = player->getStats()->getDex(true) * 2;
					int16_t vAtk = venom->mAtk;
					damage_t minDamage = ((80 * part1 / 10 + part2) / 100) * vAtk;
					damage_t maxDamage = ((185 * part1 / 10 + part2) / 100) * vAtk;

					damage = Randomizer::rand<damage_t>(maxDamage, minDamage);

					for (int8_t counter = 0; ((counter < hits) && (mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount)); ++counter) {
						success = (Randomizer::percentage<uint16_t>() < venom->prop);
						if (success) {
							statuses.emplace_back(StatusEffects::Mob::VenomousWeapon, damage, vSkill, venom->buffTime);
							mob->addStatus(player->getId(), statuses);
							statuses.clear();
						}
					}
				}
				break;
			case Vana::Skills::FpMage::PoisonMist:
				if (damage != 0) {
					// The attack itself doesn't poison them
					break;
				}
			case Vana::Skills::FpWizard::PoisonBreath:
			case Vana::Skills::FpMage::ElementComposition:
			case Vana::Skills::BlazeWizard::FlameGear:
			case Vana::Skills::NightWalker::PoisonBomb:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Poison, mob->getMaxHp() / (70 - level), skillId, skill->buffTime);
				}
				break;
		}
	}
	if (!mob->isBoss()) {
		// Seal, Stun, etc
		switch (skillId) {
			case Vana::Skills::Corsair::Hypnotize:
				statuses.emplace_back(StatusEffects::Mob::Hypnotize, 1, skillId, skill->buffTime);
				break;
			case Vana::Skills::Brawler::BackspinBlow:
			case Vana::Skills::Brawler::DoubleUppercut:
			case Vana::Skills::Buccaneer::Demolition:
			case Vana::Skills::Buccaneer::Snatch:
				statuses.emplace_back(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->buffTime);
				break;
			case Vana::Skills::Hunter::ArrowBomb:
			case Vana::Skills::Crusader::SwordComa:
			case Vana::Skills::DawnWarrior::Coma:
			case Vana::Skills::Crusader::AxeComa:
			case Vana::Skills::Crusader::Shout:
			case Vana::Skills::WhiteKnight::ChargeBlow:
			case Vana::Skills::ChiefBandit::Assaulter:
			case Vana::Skills::Shadower::BoomerangStep:
			case Vana::Skills::Gunslinger::BlankShot:
			case Vana::Skills::NightLord::NinjaStorm:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->buffTime);
				}
				break;
			case Vana::Skills::Ranger::SilverHawk:
			case Vana::Skills::Sniper::GoldenEagle:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, seconds_t{skill->x});
				}
				break;
			case Vana::Skills::FpMage::Seal:
			case Vana::Skills::IlMage::Seal:
			case Vana::Skills::BlazeWizard::Seal:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Seal, StatusEffects::Mob::Seal, skillId, skill->buffTime);
				}
				break;
			case Vana::Skills::Priest::Doom:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Doom, StatusEffects::Mob::Doom, skillId, skill->buffTime);
				}
				break;
			case Vana::Skills::Hermit::ShadowWeb:
			case Vana::Skills::NightWalker::ShadowWeb:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::ShadowWeb, level, skillId, skill->buffTime);
				}
				break;
			case Vana::Skills::FpArchMage::Paralyze:
				if (mob->canPoison()) {
					statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->buffTime);
				}
				break;
			case Vana::Skills::IlArchMage::IceDemon:
			case Vana::Skills::FpArchMage::FireDemon:
				statuses.emplace_back(StatusEffects::Mob::Poison, mob->getMaxHp() / (70 - level), skillId, skill->buffTime);
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, seconds_t{skill->x});
				break;
			case Vana::Skills::Shadower::Taunt:
			case Vana::Skills::NightLord::Taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.emplace_back(StatusEffects::Mob::MagicAttackUp, 100 - skill->x, skillId, skill->buffTime);
				statuses.emplace_back(StatusEffects::Mob::MagicDefenseUp, 100 - skill->x, skillId, skill->buffTime);
				break;
			case Vana::Skills::Outlaw::Flamethrower:
				if (auto elementalBoost = player->getSkills()->getSkillInfo(Vana::Skills::Corsair::ElementalBoost)) {
					y = elementalBoost->x;
				}
				statuses.emplace_back(StatusEffects::Mob::Poison, damage * (5 + y) / 100, skillId, skill->buffTime);
				break;
		}
	}
	switch (skillId) {
		case Vana::Skills::Shadower::NinjaAmbush:
		case Vana::Skills::NightLord::NinjaAmbush:
			damage = 2 * (player->getStats()->getStr(true) + player->getStats()->getLuk(true)) * skill->damage / 100;
			statuses.emplace_back(StatusEffects::Mob::NinjaAmbush, damage, skillId, skill->buffTime);
			break;
		case Vana::Skills::Rogue::Disorder:
		case Vana::Skills::NightWalker::Disorder:
		case Vana::Skills::Page::Threaten:
			statuses.emplace_back(StatusEffects::Mob::Watk, skill->x, skillId, skill->buffTime);
			statuses.emplace_back(StatusEffects::Mob::Wdef, skill->y, skillId, skill->buffTime);
			break;
		case Vana::Skills::FpWizard::Slow:
		case Vana::Skills::IlWizard::Slow:
		case Vana::Skills::BlazeWizard::Slow:
			statuses.emplace_back(StatusEffects::Mob::Speed, skill->x, skillId, skill->buffTime);
			break;
	}
	if (GameLogicUtilities::isBow(weapon)) {
		auto hamstring = player->getActiveBuffs()->getHamstringSource();
		if (hamstring.is_initialized()) {
			auto info = player->getActiveBuffs()->getBuffSkillInfo(hamstring.get());
			// Only triggers if player has the buff
			if (skillId != Vana::Skills::Bowmaster::Phoenix && skillId != Vana::Skills::Ranger::SilverHawk) {
				statuses.emplace_back(StatusEffects::Mob::Speed, info->x, Vana::Skills::Bowmaster::Hamstring, seconds_t{info->y});
			}
		}
	}
	else if (GameLogicUtilities::isCrossbow(weapon)) {
		auto blind = player->getActiveBuffs()->getBlindSource();
		if (blind.is_initialized()) {
			auto info = player->getActiveBuffs()->getBuffSkillInfo(blind.get());
			// Only triggers if player has the buff
			if (skillId != Vana::Skills::Marksman::Frostprey && skillId != Vana::Skills::Sniper::GoldenEagle) {
				statuses.emplace_back(StatusEffects::Mob::Acc, -(info->x), Vana::Skills::Marksman::Blind, seconds_t{info->y});
			}
		}
	}

	if (statuses.size() > 0) {
		mob->addStatus(playerId, statuses);
	}
	return statuses.size();
}

}
}