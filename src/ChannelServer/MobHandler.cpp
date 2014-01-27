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
#include "MobHandler.hpp"
#include "GameLogicUtilities.hpp"
#include "Instance.hpp"
#include "InstanceMessageConstants.hpp"
#include "Maps.hpp"
#include "MiscUtilities.hpp"
#include "Mob.hpp"
#include "MobConstants.hpp"
#include "MobsPacket.hpp"
#include "MovementHandler.hpp"
#include "PacketReader.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "Pos.hpp"
#include "Randomizer.hpp"
#include "SkillDataProvider.hpp"
#include "StatusInfo.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <functional>

auto MobHandler::handleBomb(Player *player, PacketReader &packet) -> void {
	int32_t mobId = packet.get<int32_t>();
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

auto MobHandler::friendlyDamaged(Player *player, PacketReader &packet) -> void {
	int32_t mobFrom = packet.get<int32_t>();
	int32_t playerId = packet.get<int32_t>();
	int32_t mobTo = packet.get<int32_t>();

	Map *map = player->getMap();
	auto dealer = map->getMob(mobFrom);
	auto taker = map->getMob(mobTo);
	if (dealer != nullptr && taker != nullptr && taker->isFriendly()) {
		int32_t damage = dealer->getLevel() * Randomizer::rand<int32_t>(100) / 10;
		// Temp for now until I figure out something more effective
		// TODO FIXME: Formula
		int32_t mobId = taker->getMobId();
		int32_t mapMobId =  taker->getMapMobId();
		int32_t mobHp = std::max(0, taker->getHp() - damage);
		int32_t maxHp = taker->getMaxHp();

		taker->applyDamage(playerId, damage);
		if (Instance *instance = map->getInstance()) {
			instance->sendMessage(FriendlyMobHit, mobId, mapMobId, map->getId(), mobHp, maxHp);
		}
	}
}

auto MobHandler::handleTurncoats(Player *player, PacketReader &packet) -> void {
	int32_t mobFrom = packet.get<int32_t>();
	int32_t playerId = packet.get<int32_t>();
	int32_t mobTo = packet.get<int32_t>();
	packet.skipBytes(1); // Same as player damage, -1 = bump, integer = skill ID
	int32_t damage = packet.get<int32_t>();
	packet.skipBytes(1); // Facing direction
	packet.skipBytes(4); // Some type of pos, damage display, I think

	Map *map = player->getMap();
	auto damager = map->getMob(mobFrom);
	auto taker = map->getMob(mobTo);
	if (damager != nullptr && taker != nullptr) {
		taker->applyDamage(playerId, damage);
	}
}

auto MobHandler::monsterControl(Player *player, PacketReader &packet) -> void {
	int32_t mobId = packet.get<int32_t>();

	Map *map = player->getMap();
	auto mob = map->getMob(mobId);
	if (mob == nullptr || mob->getControlStatus() == MobControlStatus::None) {
		return;
	}

	int16_t moveId = packet.get<int16_t>();
	if (mob->getController() != player && !mob->getSkillFeasibility()) {
		map->switchController(mob, player);
	}

	int8_t nibbles1 = packet.get<int8_t>();
	int8_t rawActivity = packet.get<int8_t>();
	uint8_t useSkillId = packet.get<uint8_t>();
	uint8_t useSkillLevel = packet.get<uint8_t>();
	int16_t option = packet.get<int16_t>();
	packet.skipBytes(1); // unk
	packet.skipBytes(4); // 4 bytes of always 1 or always 0?
	packet.skipBytes(4); // Pos?

	// TODO FIXME mob.get() - perhaps movement parsing should be on the MovableLife class itself?
	MovementHandler::parseMovement(mob.get(), packet);

	int8_t parsedActivity = rawActivity;
	if (parsedActivity >= 0) {
		parsedActivity = static_cast<int8_t>(static_cast<uint8_t>(parsedActivity) >> 1);
	}
	bool isAttack = MiscUtilities::inRangeInclusive<int8_t>(parsedActivity, 12, 20);
	bool isSkill = MiscUtilities::inRangeInclusive<int8_t>(parsedActivity, 21, 25);
	int32_t attackId = isAttack ? parsedActivity - 12 : -1;
	bool nextMovementCouldBeSkill = (nibbles1 & 0x0F) != 0;
	bool unk = (nibbles1 & 0xF0) != 0;

	uint8_t nextCastSkill = 0;
	uint8_t nextCastSkillLevel = 0;

	if (isAttack || isSkill) {
		if (isAttack) {
			MobAttackInfo *attack = MobDataProvider::getInstance().getMobAttack(mob->getMobIdOrLink(), attackId);
			if (attack == nullptr) {
				// Hacks
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
			if (!mob->useAnticipatedSkill()) {
				return;
			}
		}
	}

	mob->setSkillFeasibility(nextMovementCouldBeSkill);
	if (nextMovementCouldBeSkill) {
		mob->chooseRandomSkill(nextCastSkill, nextCastSkillLevel);
	}

	MobsPacket::moveMobResponse(player, mobId, moveId, nextMovementCouldBeSkill, mob->getMp(), nextCastSkill, nextCastSkillLevel);
	packet.reset(19);
	MobsPacket::moveMob(player, mobId, nextMovementCouldBeSkill, rawActivity, useSkillId, useSkillLevel, option, packet.getBuffer(), packet.getBufferLength());
}

auto MobHandler::handleMobStatus(int32_t playerId, ref_ptr_t<Mob> mob, int32_t skillId, uint8_t level, int32_t weapon, int8_t hits, int32_t damage) -> int32_t {
	Player *player = PlayerDataProvider::getInstance().getPlayer(playerId);
	vector_t<StatusInfo> statuses;
	int16_t y = 0;
	SkillLevelInfo *skill = SkillDataProvider::getInstance().getSkill(skillId, level);
	bool success = (skillId == 0 ? false : (Randomizer::rand<uint16_t>(99) < skill->prop));
	if (mob->canFreeze()) {
		// Freezing stuff
		switch (skillId) {
			case Skills::IlWizard::ColdBeam:
			case Skills::IlMage::IceStrike:
			case Skills::IlMage::ElementComposition:
			case Skills::Sniper::Blizzard:
			case Skills::IlArchMage::Blizzard:
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->time);
				break;
			case Skills::Outlaw::IceSplitter:
				if (SkillLevelInfo *eBoost = player->getSkills()->getSkillInfo(Skills::Corsair::ElementalBoost)) {
					y = eBoost->y;
				}
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->time + y);
				break;
			case Skills::FpArchMage::Elquines:
			case Skills::Marksman::Frostprey:
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->x);
				break;
		}
		if ((GameLogicUtilities::isSword(weapon) || GameLogicUtilities::isMace(weapon)) && player->getActiveBuffs()->hasIceCharge()) {
			// Ice charges
			int32_t charge = player->getActiveBuffs()->getCharge();
			statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, charge, player->getActiveBuffs()->getActiveSkillInfo(charge)->y);
		}
	}
	if (mob->canPoison() && mob->getHp() > 1) {
		// Poisoning stuff
		switch (skillId) {
			case Skills::All::RegularAttack: // Venomous Star/Stab
			case Skills::Rogue::LuckySeven:
			case Skills::Hermit::Avenger:
			case Skills::NightLord::TripleThrow:
			case Skills::Rogue::DoubleStab:
			case Skills::Rogue::Disorder:
			case Skills::Bandit::SavageBlow:
			case Skills::ChiefBandit::Assaulter:
			case Skills::Shadower::Assassinate:
			case Skills::Shadower::BoomerangStep:
				if (player->getSkills()->hasVenomousWeapon() && mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount) {
					// MAX = (18.5 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					// MIN = (8.0 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					int32_t vSkill = player->getSkills()->getVenomousWeapon();
					SkillLevelInfo *venom = player->getSkills()->getSkillInfo(vSkill);

					int32_t part1 = player->getStats()->getStr(true) + player->getStats()->getLuk(true);
					int32_t part2 = player->getStats()->getDex(true) * 2;
					int16_t vAtk = venom->mAtk;
					int32_t minDamage = ((80 * part1 / 10 + part2) / 100) * vAtk;
					int32_t maxDamage = ((185 * part1 / 10 + part2) / 100) * vAtk;

					damage = Randomizer::rand<int32_t>(maxDamage, minDamage);

					for (int8_t counter = 0; ((counter < hits) && (mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount)); ++counter) {
						success = (Randomizer::rand<uint16_t>(99) < venom->prop);
						if (success) {
							statuses.emplace_back(StatusEffects::Mob::VenomousWeapon, damage, vSkill, venom->time);
							mob->addStatus(player->getId(), statuses);
							statuses.clear();
						}
					}
				}
				break;
			case Skills::FpMage::PoisonMist:
				if (damage != 0) {
					// The attack itself doesn't poison them
					break;
				}
			case Skills::FpWizard::PoisonBreath:
			case Skills::FpMage::ElementComposition:
			case Skills::BlazeWizard::FlameGear:
			case Skills::NightWalker::PoisonBomb:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Poison, mob->getMaxHp() / (70 - level), skillId, skill->time);
				}
				break;
		}
	}
	if (!mob->isBoss()) {
		// Seal, Stun, etc
		switch (skillId) {
			case Skills::Corsair::Hypnotize:
				statuses.emplace_back(StatusEffects::Mob::Hypnotize, 1, skillId, skill->time);
				break;
			case Skills::Brawler::BackspinBlow:
			case Skills::Brawler::DoubleUppercut:
			case Skills::Buccaneer::Demolition:
			case Skills::Buccaneer::Snatch:
				statuses.emplace_back(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->time);
				break;
			case Skills::Hunter::ArrowBomb:
			case Skills::Crusader::SwordComa:
			case Skills::DawnWarrior::Coma:
			case Skills::Crusader::AxeComa:
			case Skills::Crusader::Shout:
			case Skills::WhiteKnight::ChargeBlow:
			case Skills::ChiefBandit::Assaulter:
			case Skills::Shadower::BoomerangStep:
			case Skills::Gunslinger::BlankShot:
			case Skills::NightLord::NinjaStorm:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->time);
				}
				break;
			case Skills::Ranger::SilverHawk:
			case Skills::Sniper::GoldenEagle:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->x);
				}
				break;
			case Skills::FpMage::Seal:
			case Skills::IlMage::Seal:
			case Skills::BlazeWizard::Seal:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Seal, StatusEffects::Mob::Seal, skillId, skill->time);
				}
				break;
			case Skills::Priest::Doom:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::Doom, StatusEffects::Mob::Doom, skillId, skill->time);
				}
				break;
			case Skills::Hermit::ShadowWeb:
			case Skills::NightWalker::ShadowWeb:
				if (success) {
					statuses.emplace_back(StatusEffects::Mob::ShadowWeb, level, skillId, skill->time);
				}
				break;
			case Skills::FpArchMage::Paralyze:
				if (mob->canPoison()) {
					statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->time);
				}
				break;
			case Skills::IlArchMage::IceDemon:
			case Skills::FpArchMage::FireDemon:
				statuses.emplace_back(StatusEffects::Mob::Poison, mob->getMaxHp() / (70 - level), skillId, skill->time);
				statuses.emplace_back(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->x);
				break;
			case Skills::Shadower::Taunt:
			case Skills::NightLord::Taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.emplace_back(StatusEffects::Mob::MagicAttackUp, 100 - skill->x, skillId, skill->time);
				statuses.emplace_back(StatusEffects::Mob::MagicDefenseUp, 100 - skill->x, skillId, skill->time);
				break;
			case Skills::Outlaw::Flamethrower:
				if (SkillLevelInfo *eBoost = player->getSkills()->getSkillInfo(Skills::Corsair::ElementalBoost)) {
					y = eBoost->x;
				}
				statuses.emplace_back(StatusEffects::Mob::Poison, (damage * (5 + y) / 100), skillId, skill->time);
				break;
		}
	}
	switch (skillId) {
		case Skills::Shadower::NinjaAmbush:
		case Skills::NightLord::NinjaAmbush:
			damage = 2 * (player->getStats()->getStr(true) + player->getStats()->getLuk(true)) * skill->damage / 100;
			statuses.emplace_back(StatusEffects::Mob::NinjaAmbush, damage, skillId, skill->time);
			break;
		case Skills::Rogue::Disorder:
		case Skills::NightWalker::Disorder:
		case Skills::Page::Threaten:
			statuses.emplace_back(StatusEffects::Mob::Watk, skill->x, skillId, skill->time);
			statuses.emplace_back(StatusEffects::Mob::Wdef, skill->y, skillId, skill->time);
			break;
		case Skills::FpWizard::Slow:
		case Skills::IlWizard::Slow:
		case Skills::BlazeWizard::Slow:
			statuses.emplace_back(StatusEffects::Mob::Speed, skill->x, skillId, skill->time);
			break;
	}
	if (GameLogicUtilities::isBow(weapon)) {
		if (SkillLevelInfo *hamstring = player->getActiveBuffs()->getActiveSkillInfo(Skills::Bowmaster::Hamstring)) {
			// Only triggers if player has the buff
			if (skillId != Skills::Bowmaster::Phoenix && skillId != Skills::Ranger::SilverHawk) {
				statuses.emplace_back(StatusEffects::Mob::Speed, hamstring->x, Skills::Bowmaster::Hamstring, hamstring->y);
			}
		}
	}
	else if (GameLogicUtilities::isCrossbow(weapon)) {
		if (SkillLevelInfo *blind = player->getActiveBuffs()->getActiveSkillInfo(Skills::Marksman::Blind)) {
			// Only triggers if player has the buff
			if (skillId != Skills::Marksman::Frostprey && skillId != Skills::Sniper::GoldenEagle) {
				statuses.emplace_back(StatusEffects::Mob::Acc, -(blind->x), Skills::Marksman::Blind, blind->y);
			}
		}
	}

	if (statuses.size() > 0) {
		mob->addStatus(playerId, statuses);
	}
	return statuses.size();
}