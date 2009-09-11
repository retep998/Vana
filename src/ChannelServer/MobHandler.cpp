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
#include "MobHandler.h"
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "InstanceMessageConstants.h"
#include "Maps.h"
#include "Mist.h"
#include "Mob.h"
#include "MobsPacket.h"
#include "MovementHandler.h"
#include "PacketReader.h"
#include "Player.h"
#include "Players.h"
#include "Pos.h"
#include "Randomizer.h"
#include "SkillDataProvider.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

void MobHandler::handleBomb(Player *player, PacketReader &packet) {
	int32_t mobid = packet.get<int32_t>();
	Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);
	if (player->getHp() == 0 || mob == 0) {
		return;
	}
	if (mob->getSelfDestructHp() == 0) {
		// Hacking, I think
		return;
	}
	mob->explode();
}

void MobHandler::friendlyDamaged(Player *player, PacketReader &packet) {
	int32_t mobfrom = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	int32_t mobto = packet.get<int32_t>();

	Mob *dealer = Maps::getMap(player->getMap())->getMob(mobfrom);
	Mob *taker = Maps::getMap(player->getMap())->getMob(mobto);
	if (dealer != 0 && taker != 0 && taker->isFriendly()) {
		int32_t damage = dealer->getInfo()->level * Randomizer::Instance()->randInt(100) / 10; // Temp for now until I figure out something more effective
		taker->applyDamage(playerid, damage);
	}
}

void MobHandler::handleTurncoats(Player *player, PacketReader &packet) {
	int32_t mobfrom = packet.get<int32_t>();
	int32_t playerid = packet.get<int32_t>();
	int32_t mobto = packet.get<int32_t>();
	packet.skipBytes(1); // Same as player damage, -1 = bump, integer = skill ID
	int32_t damage = packet.get<int32_t>();
	packet.skipBytes(1); // Facing direction
	packet.skipBytes(4); // Some type of pos, damage display, I think

	Mob *damager = Maps::getMap(player->getMap())->getMob(mobfrom);
	Mob *taker = Maps::getMap(player->getMap())->getMob(mobto);
	if (damager != 0 && taker != 0) {
		taker->applyDamage(playerid, damage);
		Map *m = Maps::getMap(player->getMap());
		if (m->getInstance() != 0) {
			m->getInstance()->sendMessage(FriendlyMobHit, taker->getMobId(), taker->getId(), taker->getMapId(), taker->getHp(), taker->getMHp());
		}
	}
}

void MobHandler::monsterControl(Player *player, PacketReader &packet) {
	int32_t mobid = packet.get<int32_t>();

	Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);

	if (mob == 0 || mob->getControlStatus() == Mobs::ControlStatus::ControlNone) {
		return;
	}

	int16_t moveid = packet.get<int16_t>();
	bool useskill = (packet.get<int8_t>() != 0);
	int8_t skill = packet.get<int8_t>();
	uint8_t realskill = 0;
	uint8_t level = 0;
	Pos projectiletarget = packet.getPos();
	packet.skipBytes(5); // 1 byte of always 0?, 4 bytes of always 1 or always 0?
	Pos spot = packet.getPos();

	MovementHandler::parseMovement(mob, packet);

	if (useskill && (skill == -1 || skill == 0)) {
		if (!mob->hasStatus(StatusEffects::Mob::Freeze) && !mob->hasStatus(StatusEffects::Mob::Stun)) {
			int32_t size = mob->getSkillCount();
			bool used = false;
			if (size) {
				bool stop = false;
				uint8_t rand = (uint8_t)Randomizer::Instance()->randInt(size - 1);
				MobSkillInfo *info = MobDataProvider::Instance()->getMobSkill(mob->getMobId(), rand);
				realskill = info->skillid;
 				level = info->level;
				MobSkillLevelInfo *mobskill = SkillDataProvider::Instance()->getMobSkill(realskill, level);
				switch (realskill) {
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
						int16_t spawns = (int16_t)(mob->getSpawnCount());
						int16_t limit = mobskill->limit;
						if (limit == 5000) // Custom limit based on number of players on map
							limit = 30 + Maps::getMap(mob->getMapId())->getNumPlayers() * 2;
						if (spawns >= limit)
							stop = true;
						break;
					}
				}
				if (!stop) {
					time_t now = time(0);
					time_t ls = mob->getLastSkillUse(realskill);
					if (ls == 0 || ((int16_t)(now - ls) > mobskill->interval)) {
						mob->setLastSkillUse(realskill, now);
						int64_t reqhp = mob->getHp() * 100;
						reqhp /= mob->getMHp();
						if ((uint8_t)(reqhp) <= mobskill->hp) {
							if (info->effectAfter == 0) {
								handleMobSkill(mob, realskill, level, mobskill);
							}
							else {
								new Timer::Timer(bind(&MobHandler::handleMobSkill, mob, realskill, level, mobskill),
									Timer::Id(Timer::Types::MobSkillTimer, mob->getMobId(), mob->getCounter()),
									mob->getTimers(), Timer::Time::fromNow(info->effectAfter));
							}
							used = true;
						}
					}
				}
			}
			if (!used) { 
				realskill = 0;
				level = 0;
			}
		}
	}
	MobsPacket::moveMobResponse(player, mobid, moveid, useskill, mob->getMp(), realskill, level);
	packet.reset(19);
	MobsPacket::moveMob(player, mobid, useskill, skill, projectiletarget, packet.getBuffer(), packet.getBufferLength());
}

void MobHandler::handleMobSkill(Mob *mob, uint8_t skillid, uint8_t level, MobSkillLevelInfo *skillinfo) {
	Pos mobpos = mob->getPos();
	Map *map = Maps::getMap(mob->getMapId());
	vector<StatusInfo> statuses;
	bool aoe = false;
	switch (skillid) {
		case MobSkills::WeaponAttackUpAoe:
			aoe = true;
		case MobSkills::WeaponAttackUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Watk, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::MagicAttackUpAoe:
			aoe = true;
		case MobSkills::MagicAttackUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Matk, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::WeaponDefenseUpAoe:
			aoe = true;
		case MobSkills::WeaponDefenseUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Wdef, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::MagicDefenseUpAoe:
			aoe = true;
		case MobSkills::MagicDefenseUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Mdef, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::Heal:
			map->healMobs(skillinfo->x, skillinfo->y, mobpos, skillinfo->lt, skillinfo->rb);
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
		case MobSkills::Zombify:
			map->statusPlayers(skillid, level, skillinfo->count, skillinfo->prop, mobpos, skillinfo->lt, skillinfo->rb);
			break;
		case MobSkills::Dispel:
			map->dispelPlayers(skillinfo->prop, mobpos, skillinfo->lt, skillinfo->rb);
			break;
		case MobSkills::SendToTown:
			map->sendPlayersToTown(mob->getMobId(), skillinfo->prop, skillinfo->count, mobpos, skillinfo->lt, skillinfo->rb);
			break;
		case MobSkills::PoisonMist:
			new Mist(mob->getMapId(), mob, mobpos, skillinfo, skillid, level);
			break;
		case MobSkills::WeaponImmunity:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::MagicImmunity:
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::WeaponDamageReflect:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, skillinfo->x, skillid, level, skillinfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponDamageReflect, skillinfo->x, skillid, level, skillinfo->y, skillinfo->time));
			break;
		case MobSkills::MagicDamageReflect:
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, skillinfo->x, skillid, level, skillinfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDamageReflect, skillinfo->x, skillid, level, skillinfo->y, skillinfo->time));
			break;
		case MobSkills::AnyDamageReflect:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, skillinfo->x, skillid, level, skillinfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, skillinfo->x, skillid, level, skillinfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponDamageReflect, skillinfo->x, skillid, level, skillinfo->y, skillinfo->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDamageReflect, skillinfo->x, skillid, level, skillinfo->y, skillinfo->time));
			break;
		case MobSkills::McSpeedUp:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, skillinfo->x, skillid, level, skillinfo->time));
			break;
		case MobSkills::Summon: {
			int16_t minx, maxx;
			int16_t miny = mobpos.y + skillinfo->lt.y;
			int16_t maxy = mobpos.y + skillinfo->rb.y;
			int16_t d = 0;
			if (mob->isFacingRight()) {
				minx = mobpos.x + skillinfo->rb.x * -1;
				maxx = mobpos.x + skillinfo->lt.x * -1;
			}
			else {
				minx = mobpos.x + skillinfo->lt.x;
				maxx = mobpos.x + skillinfo->rb.x;
			}
			int16_t rangex = maxx - minx;
			int16_t rangey = maxy - miny;
			if (rangex < 0)
				rangex *= -1;
			if (rangey < 0)
				rangey *= -1;
			for (size_t summonsize = 0; summonsize < skillinfo->summons.size(); summonsize++) {
				int32_t spawnid = skillinfo->summons[summonsize];
				int16_t mobx = Randomizer::Instance()->randShort(rangex) + minx;
				int16_t moby = Randomizer::Instance()->randShort(rangey) + miny;
				Pos floor;
				if (mob->getMapId() == Maps::OriginOfClockTower) { // Papulatus' map
					if (spawnid == Mobs::HighDarkstar) { // Keep High Darkstars high
						while ((floor.y > -538 || floor.y == moby) || !GameLogicUtilities::isInBox(mob->getPos(), skillinfo->lt, skillinfo->rb, floor)) {
							// Mobs spawn on the ground, we need them up top
							mobx = Randomizer::Instance()->randShort(rangex) + minx;
							moby = -590;
							floor = map->findFloor(Pos(mobx, moby));
						}
					}
					else if (spawnid == Mobs::LowDarkstar) { // Keep Low Darkstars low
						floor = map->findFloor(Pos(mobx, mobpos.y));
					}
				}
				else {
					mobx = mobpos.x + ((d % 2) ? (35 * (d + 1) / 2) : -(40 * (d / 2)));
					floor = map->findFloor(Pos(mobx, moby));
					if (floor.y == moby) {
						floor.y = mobpos.y;
					}
				}
				map->spawnMob(spawnid, floor, 0, mob, skillinfo->summoneffect);
				d++;
			}
			break;
		}
	}
	if (statuses.size() > 0) {
		if (aoe) {
			map->statusMobs(statuses, mob->getPos(), skillinfo->lt, skillinfo->rb);
		}
		else {
			mob->addStatus(0, statuses);
		}
	}
}

int32_t MobHandler::handleMobStatus(int32_t playerid, Mob *mob, int32_t skillid, uint8_t level, uint8_t weapon_type, int8_t hits, int32_t damage) {
	Player *player = Players::Instance()->getPlayer(playerid);
	vector<StatusInfo> statuses;
	int16_t y = 0;
	bool success = (skillid == 0 ? false : (Randomizer::Instance()->randInt(99) < SkillDataProvider::Instance()->getSkill(skillid, level)->prop));
	if (mob->canFreeze()) { // Freezing stuff
		switch (skillid) {
			case Jobs::ILWizard::ColdBeam:
			case Jobs::ILMage::IceStrike:
			case Jobs::ILMage::ElementComposition:
			case Jobs::Sniper::Blizzard:
			case Jobs::ILArchMage::Blizzard:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				break;
			case Jobs::Outlaw::IceSplitter:
				if (player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost) > 0)
					y = SkillDataProvider::Instance()->getSkill(Jobs::Corsair::ElementalBoost, player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost))->y;
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time + y));
				break;
			case Jobs::FPArchMage::Elquines:
			case Jobs::Marksman::Frostprey:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->x));
				break;
		}
		if ((weapon_type == Weapon1hSword || weapon_type == Weapon2hSword || weapon_type == Weapon1hMace || weapon_type == Weapon2hMace) && player->getActiveBuffs()->hasIceCharge()) { // Ice Charges
			int32_t charge = player->getActiveBuffs()->getCharge();
			statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, charge, SkillDataProvider::Instance()->getSkill(charge, player->getActiveBuffs()->getActiveSkillLevel(charge))->y));
		}
	}
	if (mob->canPoison() && mob->getHp() > 1) { // Poisoning stuff
		switch (skillid) {
			case Jobs::All::RegularAttack: // Venomous Star/Stab
			case Jobs::Rogue::LuckySeven:
			case Jobs::Hermit::Avenger:
			case Jobs::NightLord::TripleThrow:
			case Jobs::Rogue::DoubleStab:
			case Jobs::Rogue::Disorder:
			case Jobs::Bandit::SavageBlow:
			case Jobs::ChiefBandit::Assaulter:
			case Jobs::Shadower::Assassinate:
			case Jobs::Shadower::BoomerangStep:
				if (player->getSkills()->hasVenomousWeapon() && mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount) {
					// MAX = (18.5 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					// MIN = (8.0 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					int32_t vskill = player->getSkills()->getVenomousWeapon();
					uint8_t vlevel = player->getSkills()->getSkillLevel(vskill);
					SkillLevelInfo *venom = SkillDataProvider::Instance()->getSkill(vskill, vlevel);

					int32_t part1 = player->getStr() + player->getLuk();
					int32_t part2 = player->getDex() * 2;
					int16_t vatk = venom->matk;
					int32_t mindamage = ((80 * part1 / 10 + part2) / 100) * vatk;
					int32_t maxdamage = ((185 * part1 / 10 + part2) / 100) * vatk;
					
					damage = Randomizer::Instance()->randInt(maxdamage - mindamage) + mindamage;

					for (int8_t counter = 0; ((counter < hits) && (mob->getVenomCount() < StatusEffects::Mob::MaxVenomCount)); counter++) {
						success = (Randomizer::Instance()->randInt(99) < venom->prop);
						if (success) {
							statuses.push_back(StatusInfo(StatusEffects::Mob::VenomousWeapon, damage, vskill, venom->time));
							mob->addStatus(player->getId(), statuses);
							statuses.clear();
						}
					}
				}
				break;
			case Jobs::FPMage::PoisonMist:
				if (damage != 0) // The attack itself doesn't poison them
					break;
			case Jobs::FPWizard::PoisonBreath:
			case Jobs::FPMage::ElementComposition:
			case Jobs::BlazeWizard::FlameGear:
			case Jobs::NightWalker::PoisonBomb:	
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, mob->getMHp() / (70 - level), skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				}
				break;
		}
	}
	if (!mob->isBoss()) { // Seal, Stun, etc
		switch (skillid) {
			case Jobs::Corsair::Hypnotize:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Hypnotize, 1, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				break;
			case Jobs::Brawler::BackspinBlow:
			case Jobs::Brawler::DoubleUppercut:
			case Jobs::Buccaneer::Demolition:
			case Jobs::Buccaneer::Snatch:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				break;
			case Jobs::Hunter::ArrowBomb:
			case Jobs::Crusader::SwordComa:
			case Jobs::DawnWarrior::Coma:
			case Jobs::Crusader::AxeComa:
			case Jobs::Crusader::Shout:
			case Jobs::WhiteKnight::ChargeBlow:
			case Jobs::ChiefBandit::Assaulter:
			case Jobs::Shadower::BoomerangStep:
			case Jobs::Gunslinger::BlankShot:
			case Jobs::NightLord::NinjaStorm:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				}
				break;
			case Jobs::Ranger::SilverHawk:
			case Jobs::Sniper::GoldenEagle:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->x));
				}
				break;
			case Jobs::FPMage::Seal:
			case Jobs::ILMage::Seal:
			case Jobs::BlazeWizard::Seal:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Seal, StatusEffects::Mob::Seal, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				}
				break;
			case Jobs::Priest::Doom:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Doom, 0x100, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				}
				break;
			case Jobs::Hermit::ShadowWeb:
			case Jobs::NightWalker::ShadowWeb:
				if (success) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::ShadowWeb, level, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				}
				break;
			case Jobs::FPArchMage::Paralyze:
				if (mob->canPoison()) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				}
				break;
			case Jobs::ILArchMage::IceDemon:
			case Jobs::FPArchMage::FireDemon:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, mob->getMHp() / (70 - level), skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->x));
				break;
			case Jobs::Shadower::Taunt:
			case Jobs::NightLord::Taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.push_back(StatusInfo(StatusEffects::Mob::MagicAttackUp, 100 - SkillDataProvider::Instance()->getSkill(skillid, level)->x, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDefenseUp, 100 - SkillDataProvider::Instance()->getSkill(skillid, level)->x, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				break;
			case Jobs::Outlaw::Flamethrower:
				if (player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost) > 0)
					y = SkillDataProvider::Instance()->getSkill(Jobs::Corsair::ElementalBoost, player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost))->x;
				statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, (damage * (5 + y) / 100), skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
				break;
		}
	}
	switch (skillid) {
		case Jobs::Shadower::NinjaAmbush:
		case Jobs::NightLord::NinjaAmbush:
			damage = 2 * (player->getStr() + player->getLuk()) * SkillDataProvider::Instance()->getSkill(skillid, level)->damage / 100;
			statuses.push_back(StatusInfo(StatusEffects::Mob::NinjaAmbush, damage, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
			break;
		case Jobs::Rogue::Disorder:
		case Jobs::NightWalker::Disorder:
		case Jobs::Page::Threaten:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Watk, SkillDataProvider::Instance()->getSkill(skillid, level)->x, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::Wdef, SkillDataProvider::Instance()->getSkill(skillid, level)->y, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
			break;
		case Jobs::FPWizard::Slow:
		case Jobs::ILWizard::Slow:
		case Jobs::BlazeWizard::Slow:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, SkillDataProvider::Instance()->getSkill(skillid, level)->x, skillid, SkillDataProvider::Instance()->getSkill(skillid, level)->time));
			break;
	}
	if (weapon_type == WeaponBow && player->getActiveBuffs()->getActiveSkillLevel(Jobs::Bowmaster::Hamstring) > 0 && skillid != Jobs::Bowmaster::Phoenix && skillid != Jobs::Ranger::SilverHawk) {
		uint8_t hamlevel = player->getActiveBuffs()->getActiveSkillLevel(Jobs::Bowmaster::Hamstring);
		SkillLevelInfo *hamstring = SkillDataProvider::Instance()->getSkill(Jobs::Bowmaster::Hamstring, hamlevel);
		statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, hamstring->x, Jobs::Bowmaster::Hamstring, hamstring->y));
	}
	else if (weapon_type == WeaponCrossbow && player->getActiveBuffs()->getActiveSkillLevel(Jobs::Marksman::Blind) > 0 && skillid != Jobs::Marksman::Frostprey && skillid != Jobs::Sniper::GoldenEagle) {
		uint8_t blindlevel = player->getActiveBuffs()->getActiveSkillLevel(Jobs::Marksman::Blind);
		SkillLevelInfo *blind = SkillDataProvider::Instance()->getSkill(Jobs::Marksman::Blind, blindlevel);
		statuses.push_back(StatusInfo(StatusEffects::Mob::Acc, -blind->x, Jobs::Marksman::Blind, blind->y));
	}

	if (statuses.size() > 0) {
		mob->addStatus(playerid, statuses);
	}
	return statuses.size();
}