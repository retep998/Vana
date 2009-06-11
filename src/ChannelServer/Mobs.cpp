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
#include "Mobs.h"
#include "Drops.h"
#include "DropsPacket.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "Inventory.h"
#include "Levels.h"
#include "Maps.h"
#include "Mist.h"
#include "MobsPacket.h"
#include "MovementHandler.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Randomizer.h"
#include "Skills.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

// Mob status stuff
const int32_t Mobs::mobstatuses[StatusEffects::Mob::Count] = { // Order by value ascending
	StatusEffects::Mob::Watk,
	StatusEffects::Mob::Wdef,
	StatusEffects::Mob::Matk,
	StatusEffects::Mob::Mdef,
	StatusEffects::Mob::Acc,

	StatusEffects::Mob::Avoid,
	StatusEffects::Mob::Speed,
	StatusEffects::Mob::Stun,
	StatusEffects::Mob::Freeze,
	StatusEffects::Mob::Poison,

	StatusEffects::Mob::Seal,
	StatusEffects::Mob::WeaponAttackUp,
	StatusEffects::Mob::WeaponDefenseUp,
	StatusEffects::Mob::MagicAttackUp,
	StatusEffects::Mob::MagicDefenseUp,

	StatusEffects::Mob::Doom,
	StatusEffects::Mob::ShadowWeb,
	StatusEffects::Mob::WeaponImmunity,
	StatusEffects::Mob::MagicImmunity,
	StatusEffects::Mob::Empty
};

StatusInfo::StatusInfo(int32_t status, int16_t val, int32_t skillid, clock_t time) :
status(status),
val(val),
skillid(skillid),
mobskill(0),
level(0),
time(time)
{
	if (val == StatusEffects::Mob::Freeze && skillid != Jobs::FPArchMage::Paralyze) {
		this->time += Randomizer::Instance()->randInt(time);
	}
}

/* Mob class */
Mob::Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid, int16_t fh) :
MovableLife(fh, pos, 2),
originfh(fh),
id(id),
mapid(mapid),
spawnid(spawnid),
mobid(mobid),
status(StatusEffects::Mob::Empty),
webplayerid(0),
weblevel(0),
owner(0),
horntailsponge(0),
counter(0),
taunteffect(100),
hasimmunity(false),
info(MobDataProvider::Instance()->getMobInfo(mobid)),
timers(new Timer::Container),
control(0)
{
	this->hp = info.hp;
	this->mp = info.mp;
	Instance *instance = Maps::getMap(mapid)->getInstance();
	if (instance != 0) {
		instance->sendMessage(MobSpawn, mobid, id);
	}
	StatusInfo empty = StatusInfo(StatusEffects::Mob::Empty, 0, 0, 0);
	statuses[empty.status] = empty;
}

void Mob::applyDamage(int32_t playerid, int32_t damage, bool poison) {
	if (damage < 0)
		damage = 0;
	if (damage > hp)
		damage = hp - poison; // Keep HP from hitting 0 for poison and from going below 0

	damages[playerid] += damage;
	hp -= damage;

	if (!poison) { // HP bar packet does nothing for showing damage when poison is damaging for whatever reason
		Player *player = Players::Instance()->getPlayer(playerid);

		if (info.hpcolor > 0) // Boss HP bars - Horntail's damage sponge isn't a boss in the data
			MobsPacket::showBossHp(player, mobid, hp, info);
		else { // Normal/Miniboss HP bars
			uint8_t percent = static_cast<uint8_t>(hp * 100 / info.hp);
			MobsPacket::showHp(player, id, percent, info.boss);
		}
		Mob *sponge = getSponge(); // Need to preserve the pointer through mob deletion in die()
		if (hp == 0) { // Time to die
			if (getMobId() == Mobs::HorntailSponge) { // Horntail damage sponge
				for (unordered_map<int32_t, Mob *>::iterator spawniter = spawns.begin(); spawniter != spawns.end(); spawniter++) {
					new Timer::Timer(bind(&Mob::die, spawniter->second, true),
						Timer::Id(Timer::Types::HorntailTimer, id, spawniter->first),
						0, Timer::Time::fromNow(400));
				}
			}
			die(Players::Instance()->getPlayer(playerid));
		}
		if (sponge != 0) {
			sponge->applyDamage(playerid, damage, false); // Apply damage after you can be sure that all the units are linked and ready
		}
	}
}

void Mob::applyWebDamage() {
	int32_t webdamage = getMHp() / (50 - weblevel);
	if (webdamage > hp)
		webdamage = hp - 1; // Keep HP from hitting 0

	if (webdamage != 0) {
		damages[webplayerid] += webdamage;
		hp -= webdamage;
		MobsPacket::hurtMob(this, webdamage);
	}
}

void Mob::addStatus(int32_t playerid, const vector<StatusInfo> &statusinfo) {
	int32_t addedstatus = 0;
	for (size_t i = 0; i < statusinfo.size(); i++) {
		int32_t cstatus = statusinfo[i].status;
		switch (cstatus) {
			case StatusEffects::Mob::Poison: // Status effects that do not renew
			case StatusEffects::Mob::Doom:
				if (statuses.find(cstatus) != statuses.end())
					continue;
				break;
			case StatusEffects::Mob::ShadowWeb:
				webplayerid = playerid;
				weblevel = static_cast<uint8_t>(statusinfo[i].val);
				break;
			case StatusEffects::Mob::MagicAttackUp:
				if (statusinfo[i].skillid == Jobs::NightLord::Taunt || statusinfo[i].skillid == Jobs::Shadower::Taunt) {
					taunteffect = (100 - statusinfo[i].val) + 100; // Value passed as 100 - x, so 100 - value will = x
				}
				break;
		}
		statuses[cstatus] = statusinfo[i];
		addedstatus += cstatus;

		if (cstatus == StatusEffects::Mob::Poison) { // Damage timer for poison
			new Timer::Timer(bind(&Mob::applyDamage, this, playerid, statusinfo[i].val, true),
				Timer::Id(Timer::Types::MobStatusTimer, cstatus, 1),
				getTimers(), 0, 1000);
		}

		new Timer::Timer(bind(&Mob::removeStatus, this, cstatus),
			Timer::Id(Timer::Types::MobStatusTimer, cstatus, 0),
			getTimers(), Timer::Time::fromNow(statusinfo[i].time * 1000));
	}
	// Calculate new status mask
	status = 0;
	for (unordered_map<int32_t, StatusInfo>::iterator iter = statuses.begin(); iter != statuses.end(); iter++) {
		status += iter->first;
	}
	MobsPacket::applyStatus(this, addedstatus, statusinfo, 300);
}

void Mob::statusPacket(PacketCreator &packet) {
	packet.add<int32_t>(status);
	for (uint8_t i = 0; i < StatusEffects::Mob::Count; i++) { // Val/skillid pairs must be ordered in the packet by status value ascending
		int32_t status = Mobs::mobstatuses[i];
		if (statuses.find(status) != statuses.end()) {
			if (status != StatusEffects::Mob::Empty) {
				packet.add<int16_t>(statuses[status].val);
				if (statuses[status].skillid >= 0) {
					packet.add<int32_t>(statuses[status].skillid);
				}
				else {
					packet.add<int16_t>(statuses[status].mobskill);
					packet.add<int16_t>(statuses[status].level);
				}
				packet.add<int16_t>(1);
			}
			else {
				packet.add<int32_t>(0);
			}
		}
	}
}

bool Mob::hasStatus(int32_t status) {
	return (statuses.find(status) != statuses.end());
}

void Mob::removeStatus(int32_t status) {
	if (hasStatus(status)) {
		StatusInfo stat = statuses[status];
		if (status == StatusEffects::Mob::WeaponImmunity || status == StatusEffects::Mob::MagicImmunity)
			setImmunity(false);
		if (status == StatusEffects::Mob::ShadowWeb) {
			weblevel = 0;
			webplayerid = 0;
		}
		if (stat.skillid == Jobs::NightLord::Taunt || stat.skillid == Jobs::Shadower::Taunt) {
			taunteffect = 100;
		}
		this->status -= status;
		statuses.erase(status);
		if (status == StatusEffects::Mob::Poison) // Stop poison damage timer
			getTimers()->removeTimer(Timer::Id(Timer::Types::MobStatusTimer, status, 1));
		MobsPacket::removeStatus(this, status);
	}
}

void Mob::setControl(Player *control) {
	/*if (this->control != 0)
		MobsPacket::endControlMob(this->control, this);*/
	this->control = control;
	if (control != 0)
		MobsPacket::requestControl(control, this, false);
}

void Mob::endControl() {
	if (control != 0 && control->getMap() == getMapId())
		MobsPacket::endControlMob(control, this);
}

void Mob::die(Player *player, bool fromexplosion) {
	Map *map = Maps::getMap(mapid);

	endControl();

	// Calculate EXP distribution
	int32_t highestdamager = 0;
	uint32_t highestdamage = 0;
	for (unordered_map<int32_t, uint32_t>::iterator iter = damages.begin(); iter != damages.end(); iter++) {
		if (iter->second > highestdamage) { // Find the highest damager to give drop ownership
			highestdamager = iter->first;
			highestdamage = iter->second;
		}
		Player *damager = Players::Instance()->getPlayer(iter->first);
		if (damager == 0 || damager->getMap() != this->mapid || damager->getHp() == 0) // Only give EXP if the damager is in the same channel, on the same map and is alive
			continue;

		uint8_t multiplier = damager == player ? 10 : 8; // Multiplier for player to give the finishing blow is 1 and .8 for others. We therefore set this to 10 or 8 and divide the result in the formula found later on by 10.
		// Account for Holy Symbol
		int16_t hsrate = 0;
		if (damager->getActiveBuffs()->hasHolySymbol()) {
			int32_t hsid = damager->getActiveBuffs()->getHolySymbol();
			hsrate = Skills::skills[hsid][damager->getActiveBuffs()->getActiveSkillLevel(hsid)].x;
		}
		uint32_t exp = (info.exp * (multiplier * iter->second / info.hp)) / 10;
		exp = exp * getTauntEffect() / 100;
		exp += ((exp * hsrate) / 100);
		exp *= ChannelServer::Instance()->getExprate();
		Levels::giveExp(damager, exp, false, (damager == player));
	}

	// Spawn mob(s) the mob is supposed to spawn when it dies
	if (getMobId() == Mobs::SummonHorntail) { // Special Horntail logic to keep Horntail units linked
		int32_t spongeid = 0;
		vector<int32_t> parts;
		for (size_t i = 0; i < info.summon.size(); i++) {
			int32_t spawnid = info.summon[i];
			if (spawnid == Mobs::HorntailSponge)
				spongeid = map->spawnMob(spawnid, m_pos, -1, getFh(), this);
			else {
				int32_t identifier = map->spawnMob(spawnid, m_pos, -1, getFh(), this);
				parts.push_back(identifier);
			}
		}
		Mob *htsponge = Maps::getMap(mapid)->getMob(spongeid);
		for (size_t m = 0; m < parts.size(); m++) {
			Mob *f = map->getMob(parts[m]);
			f->setSponge(htsponge);
			htsponge->addSpawn(parts[m], f);
		}
	}
	else if (getSponge() != 0) { // More special Horntail logic to keep units linked
		getSponge()->removeSpawn(getId());
		for (size_t i = 0; i < info.summon.size(); i++) {
			int32_t ident = map->spawnMob(info.summon[i], m_pos, -1, getFh(), this);
			Mob *mob = map->getMob(ident);
			getSponge()->addSpawn(ident, mob);
		}
	}
	else {
		for (size_t i = 0; i < info.summon.size(); i++) {
			map->spawnMob(info.summon[i], m_pos, -1, getFh(), this);
		}
	}

	// Spawn stuff
	if (spawns.size() > 0) {
		for (unordered_map<int32_t, Mob *>::iterator spawniter = spawns.begin(); spawniter != spawns.end(); spawniter++) {
			spawniter->second->setOwner(0);
		}
	}
	if (getOwner() != 0) {
		owner->removeSpawn(getId());
	}

	// Ending of death stuff
	MobsPacket::dieMob(this, fromexplosion ? 4 : 1);
	Drops::doDrops(highestdamager, mapid, mobid, getPos(), hasExplosiveDrop(), hasFfaDrop(), getTauntEffect());

	if (player != 0)
		player->getQuests()->updateQuestMob(mobid);

	if (info.buff != 0) {
		for (size_t k = 0; k < map->getNumPlayers(); k++) {
			if (Player *target = map->getPlayer(k)) {
				Inventory::useItem(target, info.buff);
			}
		}
	}

	Instance *instance = map->getInstance();
	if (instance != 0) {
		instance->sendMessage(MobDeath, mobid, id);
	}
	map->removeMob(id, spawnid);

	delete this;
}

void Mob::explode() {
	die(0, true);
}

void Mob::die(bool showpacket) {
	if (showpacket) {
		endControl();
		MobsPacket::dieMob(this);
		Instance *instance = Maps::getMap(mapid)->getInstance();
		if (instance != 0) {
			instance->sendMessage(MobDeath, mobid, id);
		}
	}
	Maps::getMap(mapid)->removeMob(id, spawnid);
	delete this;
}

void Mob::skillHeal(int32_t healhp, int32_t healmp) {
	int32_t minamount, maxamount, range, amount;
	minamount = healhp * 10 / 15;
	maxamount = healhp * 15 / 10;
	range = maxamount - minamount;
	amount = Randomizer::Instance()->randInt(range) + minamount;
	hp += amount;
	minamount = healmp * 10 / 15;
	maxamount = healmp * 15 / 10;
	range = maxamount - minamount;
	mp += Randomizer::Instance()->randInt(range) + minamount;
	if (getSponge() != 0)
		getSponge()->skillHeal(healhp, 0);
	if (hp > getMHp() || hp < 0)
		hp = getMHp();
	if (mp > getMMp() || mp < 0)
		mp = getMMp();
	if (getMobId() != Mobs::HorntailSponge)
		MobsPacket::healMob(this, amount);
}

void Mob::dispelBuffs() {
	removeStatus(StatusEffects::Mob::Watk);
	removeStatus(StatusEffects::Mob::Wdef);
	removeStatus(StatusEffects::Mob::Matk);
	removeStatus(StatusEffects::Mob::Mdef);
	removeStatus(StatusEffects::Mob::Acc);
	removeStatus(StatusEffects::Mob::Avoid);
	removeStatus(StatusEffects::Mob::Speed);
}

void Mob::doCrashSkill(int32_t skillid) {
	switch (skillid) {
		case Jobs::Crusader::ArmorCrash:
			removeStatus(StatusEffects::Mob::Wdef);
			break;
		case Jobs::WhiteKnight::MagicCrash:
			removeStatus(StatusEffects::Mob::Matk);
			break;
		case Jobs::DragonKnight::PowerCrash:
			removeStatus(StatusEffects::Mob::Watk);
			break;
	}
}

/* Mobs namespace */
void Mobs::handleBomb(Player *player, PacketReader &packet) {
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

void Mobs::monsterControl(Player *player, PacketReader &packet) {
	int32_t mobid = packet.get<int32_t>();

	Mob *mob = Maps::getMap(player->getMap())->getMob(mobid);

	if (mob == 0) {
		return;
	}

	int16_t moveid = packet.get<int16_t>();
	bool useskill = (packet.get<int8_t>() != 0);
	int8_t skill = packet.get<int8_t>();
	uint8_t realskill = 0;
	uint8_t level = 0;
	Pos target = packet.getPos();
	packet.skipBytes(9);

	Pos cpos = mob->getPos();
	MovementHandler::parseMovement(mob, packet);
	if (cpos - mob->getPos() > 300) {
		if (player->addWarning())
			return;
	}

	if (useskill && skill == -1 || useskill && skill == 0) {
		if (!mob->hasStatus(StatusEffects::Mob::Freeze) && !mob->hasStatus(StatusEffects::Mob::Stun)) {
			vector<MobSkillInfo> hurf = mob->getSkills();
			bool used = false;
			if (hurf.size()) {
				bool stop = false;
				uint8_t rand = (uint8_t)(Randomizer::Instance()->randInt() % hurf.size());
				realskill = hurf[rand].skillid;
				level = hurf[rand].level;
				MobSkillLevelInfo mobskill = Skills::mobskills[realskill][level];
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
						stop = mob->hasImmunity();
						break;
					case MobSkills::Summon: {
						int16_t spawns = (int16_t)(mob->getSpawns().size());
						int16_t limit = mobskill.limit;
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
					if (ls == 0 || ((int16_t)(now - ls) > mobskill.interval)) {
						mob->setLastSkillUse(realskill, now);
						int64_t reqhp = mob->getHp() * 100;
						reqhp /= mob->getMHp();
						if ((uint8_t)(reqhp) <= mobskill.hp) {
							if (hurf[rand].effectAfter == 0) {
								handleMobSkill(mob, realskill, level, mobskill);
							}
							else {
								new Timer::Timer(bind(&Mobs::handleMobSkill, mob, realskill, level, mobskill),
									Timer::Id(Timer::Types::MobSkillTimer, mob->getMobId(), mob->getCounter()),
									mob->getTimers(), Timer::Time::fromNow(hurf[rand].effectAfter));
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
	MobsPacket::moveMob(player, mobid, useskill, skill, target, packet.getBuffer(), packet.getBufferLength());
}

void Mobs::handleMobSkill(Mob *mob, uint8_t skillid, uint8_t level, const MobSkillLevelInfo &skillinfo) {
	Pos mobpos = mob->getPos();
	Map *map = Maps::getMap(mob->getMapId());
	vector<StatusInfo> statuses;
	bool pushed = false;
	switch (skillid) {
		case MobSkills::WeaponAttackUp:
		case MobSkills::WeaponAttackUpAoe:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Watk, (int16_t)(skillinfo.x), skillid, level, skillinfo.time));
			pushed = true;
			break;
		case MobSkills::MagicAttackUp:
		case MobSkills::MagicAttackUpAoe:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Matk, (int16_t)(skillinfo.x), skillid, level, skillinfo.time));
			pushed = true;
			break;
		case MobSkills::WeaponDefenseUp:
		case MobSkills::WeaponDefenseUpAoe:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Wdef, (int16_t)(skillinfo.x), skillid, level, skillinfo.time));
			pushed = true;
			break;
		case MobSkills::MagicDefenseUp:
		case MobSkills::MagicDefenseUpAoe:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Mdef, (int16_t)(skillinfo.x), skillid, level, skillinfo.time));
			pushed = true;
			break;
		case MobSkills::Heal:
			map->healMobs(skillinfo.x, skillinfo.y, mobpos, skillinfo.lt, skillinfo.rb);
			break;
		case MobSkills::Seal:
		case MobSkills::Darkness:
		case MobSkills::Weakness:
		case MobSkills::Stun:
		case MobSkills::Curse:
		case MobSkills::Poison:
		case MobSkills::Slow:
		case MobSkills::Seduce:
			map->statusPlayers(skillid, level, skillinfo.count, skillinfo.prop, mobpos, skillinfo.lt, skillinfo.rb);
			break;
		case MobSkills::Dispel:
			map->dispelPlayers(skillinfo.prop, mobpos, skillinfo.lt, skillinfo.rb);
			break;
		case MobSkills::SendToTown:
			map->sendPlayersToTown(skillinfo.prop, skillinfo.count, mobpos, skillinfo.lt, skillinfo.rb);
			break;
		case MobSkills::PoisonMist: {
			Mist *mist = new Mist(mob->getMapId(), mob, mobpos, skillinfo, skillid, level);
			break;
		}
		case MobSkills::NoClue2:
			// ???
			break;
		case MobSkills::WeaponImmunity:
			statuses.push_back(StatusInfo(StatusEffects::Mob::WeaponImmunity, (int16_t)(skillinfo.x), skillid, level, skillinfo.time));
			mob->addStatus(0, statuses);
			mob->setImmunity(true);
			break;
		case MobSkills::MagicImmunity:
			statuses.push_back(StatusInfo(StatusEffects::Mob::MagicImmunity, (int16_t)(skillinfo.x), skillid, level, skillinfo.time));
			mob->addStatus(0, statuses);
			mob->setImmunity(true);
			break;
		case MobSkills::Summon: {
			int16_t minx, maxx;
			int16_t miny = mobpos.y + skillinfo.lt.y;
			int16_t maxy = mobpos.y + skillinfo.rb.y;
			if (mob->isFacingRight()) {
				minx = mobpos.x + skillinfo.rb.x * -1;
				maxx = mobpos.x + skillinfo.lt.x * -1;
			}
			else {
				minx = mobpos.x + skillinfo.lt.x;
				maxx = mobpos.x + skillinfo.rb.x;
			}
			int16_t rangex = maxx - minx;
			int16_t rangey = maxy - miny;
			if (rangex < 0)
				rangex *= -1;
			if (rangey < 0)
				rangey *= -1;
			for (size_t summonsize = 0; summonsize < skillinfo.summons.size(); summonsize++) {
				int32_t spawnid = skillinfo.summons[summonsize];
				int16_t mobx = Randomizer::Instance()->randShort(rangex) + minx;
				int16_t moby = Randomizer::Instance()->randShort(rangey) + miny;
				Pos floor;
				if (mob->getMapId() == 220080001) { // Papulatus' map
					if (spawnid == Mobs::HighDarkstar) { // Keep High Darkstars high
						while ((floor.y > -538 || floor.y == moby) || !GameLogicUtilities::isInBox(mob->getPos(), skillinfo.lt, skillinfo.rb, floor)) {
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
					floor = map->findFloor(Pos(mobx, moby));
					if (floor.y == moby) {
						floor.y = mobpos.y;
					}
				}
				map->spawnMob(spawnid, floor, -1, 0, mob, skillinfo.summoneffect);
			}
			break;
		}
	}
	if (pushed) {
		if (GameLogicUtilities::isAoeMobSkill(skillid)) {
			map->statusMobs(statuses, mob->getPos(), skillinfo.lt, skillinfo.rb);
		}
		else {
			mob->addStatus(0, statuses);
		}
	}
}

void Mobs::handleMobStatus(Player *player, Mob *mob, int32_t skillid, uint8_t level, uint8_t weapon_type, int32_t damage) {
	vector<StatusInfo> statuses;
	if (mob->canFreeze()) { // Freezing stuff
		switch (skillid) {
			case Jobs::ILWizard::ColdBeam:
			case Jobs::ILMage::IceStrike:
			case Jobs::ILMage::ElementComposition:
			case Jobs::Sniper::Blizzard:
			case Jobs::ILArchMage::Blizzard:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, Skills::skills[skillid][level].time));
				break;
			case Jobs::Outlaw::IceSplitter: {
				int16_t y = 0;
				if (player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost) > 0)
					y = Skills::skills[Jobs::Corsair::ElementalBoost][player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost)].y;
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, Skills::skills[skillid][level].time + y));
				break;
			}
			case Jobs::FPArchMage::Elquines:
			case Jobs::Marksman::Frostprey:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, Skills::skills[skillid][level].x));
				break;
		}
		if ((weapon_type == Weapon1hSword || weapon_type == Weapon2hSword || weapon_type == Weapon1hMace || weapon_type == Weapon2hMace) && player->getActiveBuffs()->hasIceCharge()) { // Ice Charges
			int32_t charge = player->getActiveBuffs()->getCharge();
			statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, charge, Skills::skills[charge][player->getActiveBuffs()->getActiveSkillLevel(charge)].y));
		}
	}
	if (mob->canPoison() && mob->getHp() > 1) { // Poisoning stuff
		switch (skillid) {
			case Jobs::FPWizard::PoisonBreath:
			case Jobs::FPMage::ElementComposition:
			case Jobs::FPMage::PoisonMist:
			case Jobs::FlameWizard::FireCurtain:
			case Jobs::NightWalker::PoisonSling:
				if (skillid == Jobs::FPMage::PoisonMist && damage != 0) // The attack itself doesn't poison them
					break;
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					int16_t pdamage = (int16_t)(mob->getMHp() / (70 - level));
					statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, pdamage, skillid, Skills::skills[skillid][level].time));
				}
				break;
		}
	}
	if (!mob->isBoss()) { // Seal, Stun, etc
		switch (skillid) {
			case Jobs::Hunter::ArrowBomb:
			case Jobs::Crusader::SwordComa:
			case Jobs::SoulWarrior::SwordComa:
			case Jobs::Crusader::AxeComa:
			case Jobs::Crusader::Shout:
			case Jobs::WhiteKnight::ChargeBlow:
			case Jobs::ChiefBandit::Assaulter:
			case Jobs::Shadower::BoomerangStep:
			case Jobs::Gunslinger::BlankShot:
			case Jobs::NightLord::NinjaStorm:
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Jobs::Infighter::BackspinBlow:
			case Jobs::Infighter::DoubleUppercut:
			case Jobs::Buccaneer::Demolition:
			case Jobs::Buccaneer::Snatch:
				statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, Skills::skills[skillid][level].time));
				break;
			case Jobs::Ranger::SilverHawk:
			case Jobs::Sniper::GoldenEagle:
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, Skills::skills[skillid][level].x));
				}
				break;
			case Jobs::FPMage::Seal:
			case Jobs::ILMage::Seal:
			case Jobs::FlameWizard::Seal:
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Stun, StatusEffects::Mob::Stun, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Jobs::Priest::Doom:
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::Doom, 0x100, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Jobs::Hermit::ShadowWeb:
			case Jobs::NightWalker::ShadowWeb:
				if (Randomizer::Instance()->randInt(99) < Skills::skills[skillid][level].prop) {
					statuses.push_back(StatusInfo(StatusEffects::Mob::ShadowWeb, level, skillid, Skills::skills[skillid][level].time));
				}
				break;
			case Jobs::FPArchMage::Paralyze:
				if (mob->canPoison())
					statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, Skills::skills[skillid][level].time));
				break;
			case Jobs::ILArchMage::IceDemon:
			case Jobs::FPArchMage::FireDemon: {
				int16_t pdamage = (int16_t)(mob->getMHp() / (70 - level));
				statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, pdamage, skillid, Skills::skills[skillid][level].time));
				statuses.push_back(StatusInfo(StatusEffects::Mob::Freeze, StatusEffects::Mob::Freeze, skillid, Skills::skills[skillid][level].x));
				break;
			}
			case Jobs::Outlaw::Flamethrower: {
				int16_t y = 0;
				if (player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost) > 0)
					y = Skills::skills[Jobs::Corsair::ElementalBoost][player->getSkills()->getSkillLevel(Jobs::Corsair::ElementalBoost)].x;
				int32_t test = (damage * (5 + y) / 100);
				int16_t pdamage = (test > 30000 ? 30000 : static_cast<int16_t>(test));
				statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, pdamage, skillid, Skills::skills[skillid][level].time));
				break;
			}
			case Jobs::Shadower::Taunt:
			case Jobs::NightLord::Taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.push_back(StatusInfo(StatusEffects::Mob::MagicAttackUp, 100 - Skills::skills[skillid][level].x, skillid, Skills::skills[skillid][level].time));
				statuses.push_back(StatusInfo(StatusEffects::Mob::MagicDefenseUp, 100 - Skills::skills[skillid][level].x, skillid, Skills::skills[skillid][level].time));
				break;
		}
	}
	switch (skillid) {
		case Jobs::Shadower::NinjaAmbush:
		case Jobs::NightLord::NinjaAmbush: {
			int32_t test = 2 * (player->getStr() + player->getLuk()) * Skills::skills[skillid][level].damage / 100;
			int16_t pdamage = (test > 30000 ? 30000 : static_cast<int16_t>(test));
			statuses.push_back(StatusInfo(StatusEffects::Mob::Poison, pdamage, skillid, Skills::skills[skillid][level].time));
			break;
		}
		case Jobs::Rogue::Disorder:
		case Jobs::NightWalker::Disorder:
		case Jobs::Page::Threaten:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Watk, Skills::skills[skillid][level].x, skillid, Skills::skills[skillid][level].time));
			statuses.push_back(StatusInfo(StatusEffects::Mob::Wdef, Skills::skills[skillid][level].y, skillid, Skills::skills[skillid][level].time));
			break;
		case Jobs::FPWizard::Slow:
		case Jobs::ILWizard::Slow:
		case Jobs::FlameWizard::Slow:
			statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, Skills::skills[skillid][level].x, skillid, Skills::skills[skillid][level].time));
			break;
	}
	if (weapon_type == WeaponBow && player->getActiveBuffs()->getActiveSkillLevel(Jobs::Bowmaster::Hamstring) > 0 && skillid != Jobs::Bowmaster::Phoenix && skillid != Jobs::Ranger::SilverHawk) {
		uint8_t hamlevel = player->getActiveBuffs()->getActiveSkillLevel(Jobs::Bowmaster::Hamstring);
		statuses.push_back(StatusInfo(StatusEffects::Mob::Speed, Skills::skills[Jobs::Bowmaster::Hamstring][hamlevel].x, Jobs::Bowmaster::Hamstring, Skills::skills[Jobs::Bowmaster::Hamstring][hamlevel].y));
	}
	else if (weapon_type == WeaponCrossbow && player->getActiveBuffs()->getActiveSkillLevel(Jobs::Marksman::Blind) > 0 && skillid != Jobs::Marksman::Frostprey && skillid != Jobs::Sniper::GoldenEagle) {
		uint8_t blindlevel = player->getActiveBuffs()->getActiveSkillLevel(Jobs::Marksman::Blind);
		statuses.push_back(StatusInfo(StatusEffects::Mob::Acc, -Skills::skills[Jobs::Marksman::Blind][blindlevel].x, Jobs::Marksman::Blind, Skills::skills[Jobs::Marksman::Blind][blindlevel].y));
	}

	if (statuses.size() > 0)
		mob->addStatus(player->getId(), statuses);
}