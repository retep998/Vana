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
#include "MobHandler.h"
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "InstanceMessageConstants.h"
#include "Maps.h"
#include "Mist.h"
#include "Mob.h"
#include "MobConstants.h"
#include "MobsPacket.h"
#include "MovementHandler.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "Pos.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include <functional>

using std::bind;

void MobHandler::handleBomb(Player *player, PacketReader &packet) {
	int32_t mobId = packet.get<int32_t>();
	Mob *mob = player->getMap()->getMob(mobId);
	if (player->getStats()->isDead() || mob == nullptr) {
		return;
	}
	if (mob->getSelfDestructHp() == 0) {
		// Hacking, I think
		return;
	}
	mob->explode();
}

void MobHandler::friendlyDamaged(Player *player, PacketReader &packet) {
	int32_t mobFrom = packet.get<int32_t>();
	int32_t playerId = packet.get<int32_t>();
	int32_t mobTo = packet.get<int32_t>();

	Map *map = player->getMap();
	Mob *dealer = map->getMob(mobFrom);
	Mob *taker = map->getMob(mobTo);
	if (dealer != nullptr && taker != nullptr && taker->isFriendly()) {
		int32_t damage = dealer->getInfo()->level * Randomizer::Instance()->randInt(100) / 10;
		// Temp for now until I figure out something more effective
		// TODO: Fix formula
		int32_t mobId = taker->getMobId();
		int32_t mapMobId =  taker->getId();
		int32_t mobHp = (damage > taker->getHp() ? 0 : taker->getHp() - damage);
		int32_t maxHp = taker->getMaxHp();

		taker->applyDamage(playerId, damage);
		if (Instance *i = map->getInstance()) {
			i->sendMessage(FriendlyMobHit, mobId, mapMobId, map->getId(), mobHp, maxHp);
		}
	}
}

void MobHandler::handleTurncoats(Player *player, PacketReader &packet) {
	int32_t mobFrom = packet.get<int32_t>();
	int32_t playerId = packet.get<int32_t>();
	int32_t mobTo = packet.get<int32_t>();
	packet.skipBytes(1); // Same as player damage, -1 = bump, integer = skill ID
	int32_t damage = packet.get<int32_t>();
	packet.skipBytes(1); // Facing direction
	packet.skipBytes(4); // Some type of pos, damage display, I think

	Map *map = player->getMap();
	Mob *damager = map->getMob(mobFrom);
	Mob *taker = map->getMob(mobTo);
	if (damager != nullptr && taker != nullptr) {
		taker->applyDamage(playerId, damage);
	}
}

void MobHandler::monsterControl(Player *player, PacketReader &packet) {
	int32_t mobId = packet.get<int32_t>();

	Mob *mob = player->getMap()->getMob(mobId);

	if (mob == nullptr || mob->getControlStatus() == Mobs::ControlStatus::ControlNone) {
		return;
	}

	int16_t moveId = packet.get<int16_t>();
	bool useSkill = packet.getBool();
	int8_t skill = packet.get<int8_t>();
	uint8_t realSkill = 0;
	uint8_t level = 0;
	const Pos &projectileTarget = packet.getClass<Pos>();
	packet.skipBytes(5); // 1 byte of always 0?, 4 bytes of always 1 or always 0?
	const Pos &spot = packet.getClass<Pos>();

	MovementHandler::parseMovement(mob, packet);

	if (useSkill && (skill == -1 || skill == 0)) {
		if (mob->canCastSkills()) {
			uint8_t size = mob->getSkillCount();
			bool used = false;
			if (size > 0) {
				bool stop = false;
				uint8_t rand = Randomizer::Instance()->randChar(size - 1);
				MobSkillInfo *info = MobDataProvider::Instance()->getMobSkill(mob->getMobId(), rand);
				realSkill = info->skillId;
				level = info->level;
				MobSkillLevelInfo *mobSkill = SkillDataProvider::Instance()->getMobSkill(realSkill, level);
				switch (realSkill) {
					case MobSkills::WeaponAttackUp:
					case MobSkills::WeaponAttackUpAoe:
						stop = mob->hasStatus(StatusEffects::Mob::Watk);
						break;
					case MobSkills::MagicAttackUp:
					case MobSkills::MagicAttackUpAoe:
						stop = mob->hasStatus(StatusEffects::Mob::Matk);
						break;
					case MobSkills::WeaponDefenseUp:
					case MobSkills::WeaponDefenseUpAoe:
						stop = mob->hasStatus(StatusEffects::Mob::Wdef);
						break;
					case MobSkills::MagicDefenseUp:
					case MobSkills::MagicDefenseUpAoe:
						stop = mob->hasStatus(StatusEffects::Mob::Mdef);
						break;
					case MobSkills::WeaponImmunity:
					case MobSkills::MagicImmunity:
					case MobSkills::WeaponDamageReflect:
					case MobSkills::MagicDamageReflect:
						stop = mob->hasImmunity();
						break;
					case MobSkills::McSpeedUp:
						stop = mob->hasStatus(StatusEffects::Mob::Speed);
						break;
					case MobSkills::Summon: {
						int16_t limit = mobSkill->limit;
						if (limit == 5000) {
							// Custom limit based on number of players on map
							limit = 30 + Maps::getMap(mob->getMapId())->getNumPlayers() * 2;
						}
						if (mob->getSpawnCount() >= limit) {
							stop = true;
						}
						break;
					}
				}
				if (!stop) {
					time_t now = time(nullptr);
					time_t ls = mob->getLastSkillUse(realSkill);
					if (ls == 0 || ((int32_t)(now - ls) > mobSkill->interval)) {
						mob->setLastSkillUse(realSkill, now);
						int64_t reqhp = mob->getHp() * 100;
						reqhp /= mob->getMaxHp();
						if ((uint8_t)(reqhp) <= mobSkill->hp) {
							if (info->effectAfter == 0) {
								handleMobSkill(mob, realSkill, level, mobSkill);
							}
							else {
								new Timer::Timer(bind(&MobHandler::handleMobSkill, mob, realSkill, level, mobSkill),
									Timer::Id(Timer::Types::MobSkillTimer, mob->getMobId(), mob->getCounter()),
									mob->getTimers(), TimeUtilities::fromNow(info->effectAfter));
							}
							used = true;
						}
					}
				}
			}
			if (!used) {
				realSkill = 0;
				level = 0;
			}
		}
	}
	MobsPacket::moveMobResponse(player, mobId, moveId, useSkill, mob->getMp(), realSkill, level);
	packet.reset(19);
	MobsPacket::moveMob(player, mobId, useSkill, skill, projectileTarget, packet.getBuffer(), packet.getBufferLength());
}

void MobHandler::handleMobSkill(Mob *mob, uint8_t skillId, uint8_t level, MobSkillLevelInfo *skillInfo) {
	const Pos &mobPos = mob->getPos();
	Map *map = Maps::getMap(mob->getMapId());
	vector<StatusInfo> statuses;
	bool aoe = false;
	switch (skillId) {
		case MobSkills::WeaponAttackUpAoe:
			aoe = true;
		case MobSkills::WeaponAttackUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Watk, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::MagicAttackUpAoe:
			aoe = true;
		case MobSkills::MagicAttackUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Matk, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::WeaponDefenseUpAoe:
			aoe = true;
		case MobSkills::WeaponDefenseUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Wdef, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::MagicDefenseUpAoe:
			aoe = true;
		case MobSkills::MagicDefenseUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Mdef, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::HealAoe:
			map->healMobs(skillInfo->x, skillInfo->y, mobPos, skillInfo->lt, skillInfo->rb);
			break;
		case MobSkills::Seal:
		case MobSkills::Darkness:
		case MobSkills::Weakness:
		case MobSkills::Stun:
		case MobSkills::Curse:
		case MobSkills::Poison:
		case MobSkills::Slow:
		case MobSkills::Seduce:
		case MobSkills::CrazySkull:
		case MobSkills::Zombify: {
			auto func = [&skillId, &level](Player *player) {
				player->getActiveBuffs()->addDebuff(skillId, level);
			};
			map->runFunctionPlayers(mobPos, skillInfo->lt, skillInfo->rb, skillInfo->prop, skillInfo->count, func);
			break;
		}
		case MobSkills::Dispel: {
			map->runFunctionPlayers(mobPos, skillInfo->lt, skillInfo->rb, skillInfo->prop, [](Player *player) {
				player->getActiveBuffs()->dispelBuffs();
			});
			break;
		}
		case MobSkills::SendToTown: {
			int32_t field = map->getReturnMap();
			PortalInfo *portal = nullptr;
			string message = "";
			if (BanishField *ban = SkillDataProvider::Instance()->getBanishData(mob->getMobId())) {
				field = ban->field;
				message = ban->message;
				if (ban->portal != "" && ban->portal != "sp") {
					portal = Maps::getMap(field)->getPortal(ban->portal);
				}
			}
			auto func = [&message, &field, &portal](Player *player) {
				if (message != "") {
					PlayerPacket::showMessage(player, message, PlayerPacket::NoticeTypes::Blue);
				}
				player->setMap(field, portal);
			};
			map->runFunctionPlayers(mobPos, skillInfo->lt, skillInfo->rb, skillInfo->prop, skillInfo->count, func);
			break;
		}
		case MobSkills::PoisonMist:
			new Mist(mob->getMapId(), mob, mobPos, skillInfo, skillId, level);
			break;
		case MobSkills::WeaponImmunity:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::MagicImmunity:
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::WeaponDamageReflect:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, skillInfo->x, skillId, level, skillInfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponDamageReflect, skillInfo->x, skillId, level, skillInfo->y, skillInfo->time));
			break;
		case MobSkills::MagicDamageReflect:
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, skillInfo->x, skillId, level, skillInfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDamageReflect, skillInfo->x, skillId, level, skillInfo->y, skillInfo->time));
			break;
		case MobSkills::AnyDamageReflect:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, skillInfo->x, skillId, level, skillInfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, skillInfo->x, skillId, level, skillInfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponDamageReflect, skillInfo->x, skillId, level, skillInfo->y, skillInfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDamageReflect, skillInfo->x, skillId, level, skillInfo->y, skillInfo->time));
			break;
		case MobSkills::McSpeedUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, skillInfo->x, skillId, level, skillInfo->time));
			break;
		case MobSkills::Summon: {
			int16_t xMin, xMax;
			int16_t yMin = mobPos.y + skillInfo->lt.y;
			int16_t yMax = mobPos.y + skillInfo->rb.y;
			int16_t d = 0;
			if (mob->isFacingRight()) {
				xMin = mobPos.x + skillInfo->rb.x * -1;
				xMax = mobPos.x + skillInfo->lt.x * -1;
			}
			else {
				xMin = mobPos.x + skillInfo->lt.x;
				xMax = mobPos.x + skillInfo->rb.x;
			}
			for (size_t summonSize = 0; summonSize < skillInfo->summons.size(); ++summonSize) {
				int32_t spawnId = skillInfo->summons[summonSize];
				int16_t xPos = Randomizer::Instance()->randShort(xMax, xMin);
				int16_t yPos = Randomizer::Instance()->randShort(yMax, yMin);
				Pos floor;
				if (mob->getMapId() == Maps::OriginOfClockTower) {
					// Papulatus' map
					if (spawnId == Mobs::HighDarkstar) {
						// Keep High Darkstars high
						while ((floor.y > -538 || floor.y == yPos) || !GameLogicUtilities::isInBox(mob->getPos(), skillInfo->lt, skillInfo->rb, floor)) {
							// Mobs spawn on the ground, we need them up top
							xPos = Randomizer::Instance()->randShort(xMax, xMin);
							yPos = -590;
							floor = map->findFloor(Pos(xPos, yPos));
						}
					}
					else if (spawnId == Mobs::LowDarkstar) {
						// Keep Low Darkstars low
						floor = map->findFloor(Pos(xPos, mobPos.y));
					}
				}
				else {
					xPos = mobPos.x + ((d % 2) ? (35 * (d + 1) / 2) : -(40 * (d / 2)));
					floor = map->findFloor(Pos(xPos, yPos));
					if (floor.y == yPos) {
						floor.y = mobPos.y;
					}
				}
				map->spawnMob(spawnId, floor, 0, mob, skillInfo->summonEffect);
				d++;
			}
			break;
		}
	}
	if (statuses.size() > 0) {
		if (aoe) {
			map->statusMobs(statuses, mob->getPos(), skillInfo->lt, skillInfo->rb);
		}
		else {
			mob->addStatus(0, statuses);
		}
	}
}

int32_t MobHandler::handleMobStatus(int32_t playerId, Mob *mob, int32_t skillId, uint8_t level, int32_t weapon, int8_t hits, int32_t damage) {
	Player *player = PlayerDataProvider::Instance()->getPlayer(playerId);
	vector<StatusInfo> statuses;
	int16_t y = 0;
	SkillLevelInfo *skill = SkillDataProvider::Instance()->getSkill(skillId, level);
	bool success = (skillId == 0 ? false : (Randomizer::Instance()->randInt(99) < skill->prop));
	if (mob->canFreeze()) {
		// Freezing stuff
		switch (skillId) {
			case Skills::IlWizard::ColdBeam:
			case Skills::IlMage::IceStrike:
			case Skills::IlMage::ElementComposition:
			case Skills::Sniper::Blizzard:
			case Skills::IlArchMage::Blizzard:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->time));
				break;
			case Skills::Outlaw::IceSplitter:
				if (SkillLevelInfo *eBoost = player->getSkills()->getSkillInfo(Skills::Corsair::ElementalBoost)) {
					y = eBoost->y;
				}
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->time + y));
				break;
			case Skills::FpArchMage::Elquines:
			case Skills::Marksman::Frostprey:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->x));
				break;
		}
		if ((GameLogicUtilities::isSword(weapon) || GameLogicUtilities::isMace(weapon)) && player->getActiveBuffs()->hasIceCharge()) {
			// Ice charges
			int32_t charge = player->getActiveBuffs()->getCharge();
			statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, charge, player->getActiveBuffs()->getActiveSkillInfo(charge)->y));
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

					damage = Randomizer::Instance()->randInt(maxDamage, minDamage);

					for (int8_t counter = 0; ((counter < hits) && (mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount)); ++counter) {
						success = (Randomizer::Instance()->randInt(99) < venom->prop);
						if (success) {
							statuses.push_back(StatusInfo(StatusEffects::Mob::VenomousWeapon, damage, vSkill, venom->time));
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
					statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, mob->getMaxHp() / (70 - level), skillId, skill->time));
				}
				break;
		}
	}
	if (!mob->isBoss()) {
		// Seal, Stun, etc
		switch (skillId) {
			case Skills::Corsair::Hypnotize:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Hypnotize, 1, skillId, skill->time));
				break;
			case Skills::Brawler::BackspinBlow:
			case Skills::Brawler::DoubleUppercut:
			case Skills::Buccaneer::Demolition:
			case Skills::Buccaneer::Snatch:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->time));
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
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->time));
				}
				break;
			case Skills::Ranger::SilverHawk:
			case Skills::Sniper::GoldenEagle:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillId, skill->x));
				}
				break;
			case Skills::FpMage::Seal:
			case Skills::IlMage::Seal:
			case Skills::BlazeWizard::Seal:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Seal, StatusEffects::Mob::Seal, skillId, skill->time));
				}
				break;
			case Skills::Priest::Doom:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Doom, StatusEffects::Mob::Doom, skillId, skill->time));
				}
				break;
			case Skills::Hermit::ShadowWeb:
			case Skills::NightWalker::ShadowWeb:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::ShadowWeb, level, skillId, skill->time));
				}
				break;
			case Skills::FpArchMage::Paralyze:
				if (mob->canPoison()) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->time));
				}
				break;
			case Skills::IlArchMage::IceDemon:
			case Skills::FpArchMage::FireDemon:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, mob->getMaxHp() / (70 - level), skillId, skill->time));
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillId, skill->x));
				break;
			case Skills::Shadower::Taunt:
			case Skills::NightLord::Taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.push_back(StatusInfo(StatusEffects::Mob::MagicAttackUp, 100 - skill->x, skillId, skill->time));
				statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDefenseUp, 100 - skill->x, skillId, skill->time));
				break;
			case Skills::Outlaw::Flamethrower:
				if (SkillLevelInfo *eBoost = player->getSkills()->getSkillInfo(Skills::Corsair::ElementalBoost)) {
					y = eBoost->x;
				}
				statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, (damage * (5 + y) / 100), skillId, skill->time));
				break;
		}
	}
	switch (skillId) {
		case Skills::Shadower::NinjaAmbush:
		case Skills::NightLord::NinjaAmbush:
			damage = 2 * (player->getStats()->getStr(true) + player->getStats()->getLuk(true)) * skill->damage / 100;
			statuses.push_back(StatusInfo(StatusEffects::Mob::NinjaAmbush, damage, skillId, skill->time));
			break;
		case Skills::Rogue::Disorder:
		case Skills::NightWalker::Disorder:
		case Skills::Page::Threaten:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Watk, skill->x, skillId, skill->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::Wdef, skill->y, skillId, skill->time));
			break;
		case Skills::FpWizard::Slow:
		case Skills::IlWizard::Slow:
		case Skills::BlazeWizard::Slow:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, skill->x, skillId, skill->time));
			break;
	}
	if (GameLogicUtilities::isBow(weapon)) {
		if (SkillLevelInfo *hamstring = player->getActiveBuffs()->getActiveSkillInfo(Skills::Bowmaster::Hamstring)) {
			// Only triggers if player has the buff
			if (skillId != Skills::Bowmaster::Phoenix && skillId != Skills::Ranger::SilverHawk) {
				statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, hamstring->x, Skills::Bowmaster::Hamstring, hamstring->y));
			}
		}
	}
	else if (GameLogicUtilities::isCrossbow(weapon)) {
		if (SkillLevelInfo *blind = player->getActiveBuffs()->getActiveSkillInfo(Skills::Marksman::Blind)) {
			// Only triggers if player has the buff
			if (skillId != Skills::Marksman::Frostprey && skillId != Skills::Sniper::GoldenEagle) {
				statuses.push_back(StatusInfo(StatusEffects::Mob::Acc, -(blind->x), Skills::Marksman::Blind, blind->y));
			}
		}
	}

	if (statuses.size() > 0) {
		mob->addStatus(playerId, statuses);
	}
	return statuses.size();
}