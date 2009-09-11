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
#include "Mob.h"
#include "ChannelServer.h"
#include "DropHandler.h"
#include "GameConstants.h"
#include "Instance.h"
#include "Levels.h"
#include "Maps.h"
#include "MobsPacket.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "Players.h"
#include "Randomizer.h"
#include "Skills.h"
#include "SkillsPacket.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

StatusInfo::StatusInfo(int32_t status, int32_t val, int32_t skillid, clock_t time) :
status(status),
val(val),
skillid(skillid),
mobskill(0),
level(0),
reflection(0),
time(time)
{
	if (val == StatusEffects::Mob::Freeze && skillid != Jobs::FPArchMage::Paralyze) {
		this->time += Randomizer::Instance()->randInt(time);
	}
}

StatusInfo::StatusInfo(int32_t status, int32_t val, int16_t mobskill, int16_t level, clock_t time) :
status(status),
val(val),
skillid(-1),
mobskill(mobskill),
level(level),
time(time),
reflection(-1)
{
}

StatusInfo::StatusInfo(int32_t status, int32_t val, int16_t mobskill, int16_t level, int32_t reflect, clock_t time) :
status(status),
val(val),
skillid(-1),
mobskill(mobskill),
level(level),
time(time),
reflection(reflect)
{
}

Mob::Mob(int32_t id, int32_t mapid, int32_t mobid, const Pos &pos, int16_t fh, int8_t controlstatus) :
MovableLife(fh, pos, 2),
originfh(fh),
id(id),
mapid(mapid),
spawnid(-1),
mobid(mobid),
timers(new Timer::Container),
info(MobDataProvider::Instance()->getMobInfo(mobid)),
facingdirection(1),
controlstatus(controlstatus)
{
	initMob();
}

Mob::Mob(int32_t id, int32_t mapid, int32_t mobid, const Pos &pos, int32_t spawnid, int8_t direction, int16_t fh) :
MovableLife(fh, pos, 2),
originfh(fh),
id(id),
mapid(mapid),
spawnid(spawnid),
mobid(mobid),
timers(new Timer::Container),
info(MobDataProvider::Instance()->getMobInfo(mobid)),
facingdirection(direction),
controlstatus(Mobs::ControlStatus::ControlNormal)
{
	initMob();
}

void Mob::initMob() {
	this->hp = info.hp;
	this->mp = info.mp;
	if (info.flying) {
		setFh(0);
		originfh = 0;
	}

	totalhealth = hp;

	owner = 0; // Pointers
	sponge = 0;
	control = 0;

	webplayerid = 0; // Skill stuff
	weblevel = 0;
	counter = 0;
	venomcount = 0;
	mpeatercount = 0;
	taunteffect = 100;

	Map *map = Maps::getMap(mapid);
	Instance *instance = map->getInstance();
	if (instance != 0) {
		instance->sendMessage(MobSpawn, mobid, id, mapid);
	}

	status = StatusEffects::Mob::Empty;
	StatusInfo empty = StatusInfo(StatusEffects::Mob::Empty, 0, 0, 0);
	statuses[empty.status] = empty;

	if (info.hprecovery > 0) {
		new Timer::Timer(bind(&Mob::naturalHealHp, this, info.hprecovery),
			Timer::Id(Timer::Types::MobHealTimer, 0, 0),
			getTimers(), 0, 10 * 1000);
	}
	if (info.mprecovery > 0) {
		new Timer::Timer(bind(&Mob::naturalHealMp, this, info.mprecovery),
			Timer::Id(Timer::Types::MobHealTimer, 1, 1),
			getTimers(), 0, 10 * 1000);
	}
	if (info.removeafter > 0) {
		new Timer::Timer(bind(&Mob::applyDamage, this, 0, info.hp, false),
			Timer::Id(Timer::Types::MobRemoveTimer, mobid, id),
			map->getTimers(), Timer::Time::fromNow(info.removeafter * 1000));
	}
}

void Mob::naturalHealHp(int32_t amount) {
	if (getHp() < getMHp()) {
		int32_t hp = getHp() + amount;
		int32_t sponge = amount;
		if (hp < 0 || hp > getMHp()) {
			sponge = getMHp() - getHp(); // This is the amount for the sponge
			hp = getMHp();
		}
		setHp(hp);
		totalhealth += sponge;
		if (getSponge() != 0) {
			getSponge()->setHp(getSponge()->getHp() + sponge);
		}
	}
}

void Mob::naturalHealMp(int32_t amount) {
	if (getMp() < getMMp()) {
		int32_t mp = getMp() + amount;
		if (mp < 0 || mp > getMMp())
			mp = getMMp();
		setMp(mp);
	}
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
	
		uint8_t percent = static_cast<uint8_t>(hp * 100 / info.hp);

		if (info.hpcolor > 0) { // Boss HP bars - Horntail's damage sponge isn't a boss in the data
			MobsPacket::showBossHp(mapid, mobid, hp, info);
		}
		else if (info.boss) { // Minibosses
			MobsPacket::showHp(mapid, id, percent);
		}
		else if (info.friendly) {
			MobsPacket::damageFriendlyMob(this, damage);
		}
		else if (player != 0) {
			MobsPacket::showHp(player, id, percent);
		}

		Mob *sponge = getSponge(); // Need to preserve the pointer through mob deletion in die()
		if (hp == 0) { // Time to die
			switch (getMobId()) {
				case Mobs::HorntailSponge:
					for (unordered_map<int32_t, Mob *>::iterator spawniter = spawns.begin(); spawniter != spawns.end(); spawniter++) {
						new Timer::Timer(bind(&Mob::die, spawniter->second, true),
							Timer::Id(Timer::Types::SpongeCleanupTimer, id, spawniter->first),
							0, Timer::Time::fromNow(400));
					}
					break;
				case Mobs::ZakumArm1:
				case Mobs::ZakumArm2:
				case Mobs::ZakumArm3:
				case Mobs::ZakumArm4:
				case Mobs::ZakumArm5:
				case Mobs::ZakumArm6:
				case Mobs::ZakumArm7:
				case Mobs::ZakumArm8:
					if (getOwner() != 0 && getOwner()->getSpawnCount() == 1) {
						// Last linked arm died
						int8_t cstatus = Mobs::ControlStatus::ControlNormal;
						getOwner()->setControlStatus(cstatus);
					}
					break;
			}
			die(player);
		}
		if (sponge != 0) {
			sponge->applyDamage(playerid, damage, false); // Apply damage after you can be sure that all the units are linked and ready
		}
	}
	// TODO: Fix this, for some reason, it causes issues within the timer container
//	else if (hp == 1) {
//		removeStatus(StatusEffects::Mob::Poison);
//	}
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

void Mob::addStatus(int32_t playerid, vector<StatusInfo> &statusinfo) {
	int32_t addedstatus = 0;
	vector<int32_t> reflection;
	for (size_t i = 0; i < statusinfo.size(); i++) {
		int32_t cstatus = statusinfo[i].status;
		bool alreadyhas = (statuses.find(cstatus) != statuses.end());
		switch (cstatus) {
			case StatusEffects::Mob::Poison: // Status effects that do not renew
			case StatusEffects::Mob::Doom:
				if (alreadyhas)
					continue;
				break;
			case StatusEffects::Mob::ShadowWeb:
				webplayerid = playerid;
				weblevel = static_cast<uint8_t>(statusinfo[i].val);
				Maps::getMap(mapid)->setWebbedCount(Maps::getMap(mapid)->getWebbedCount() + 1);
				break;
			case StatusEffects::Mob::MagicAttackUp:
				if (statusinfo[i].skillid == Jobs::NightLord::Taunt || statusinfo[i].skillid == Jobs::Shadower::Taunt) {
					taunteffect = (100 - statusinfo[i].val) + 100; // Value passed as 100 - x, so 100 - value will = x
				}
				break;
			case StatusEffects::Mob::VenomousWeapon:
				setVenomCount(getVenomCount() + 1);
				if (alreadyhas) {
					statusinfo[i].val += statuses[cstatus].val; // Increase the damage
				}
				break;
			case StatusEffects::Mob::WeaponDamageReflect:
			case StatusEffects::Mob::MagicDamageReflect:
				reflection.push_back(statusinfo[i].reflection);
				break;
		}

		statuses[cstatus] = statusinfo[i];
		addedstatus += cstatus;

		switch (cstatus) {
			case StatusEffects::Mob::Poison:
			case StatusEffects::Mob::VenomousWeapon:
			case StatusEffects::Mob::NinjaAmbush: // Damage timer for poison(s)
				new Timer::Timer(bind(&Mob::applyDamage, this, playerid, statusinfo[i].val, true),
					Timer::Id(Timer::Types::MobStatusTimer, cstatus, 1),
					getTimers(), 0, 1000);
				break;
		}

		new Timer::Timer(bind(&Mob::removeStatus, this, cstatus, true),
			Timer::Id(Timer::Types::MobStatusTimer, cstatus, 0),
			getTimers(), Timer::Time::fromNow(statusinfo[i].time * 1000));
	}
	// Calculate new status mask
	status = 0;
	for (map<int32_t, StatusInfo>::iterator iter = statuses.begin(); iter != statuses.end(); iter++) {
		status += iter->first;
	}
	MobsPacket::applyStatus(this, addedstatus, statusinfo, 300, reflection);
}

void Mob::statusPacket(PacketCreator &packet) {
	packet.add<int32_t>(status);
	for (map<int32_t, StatusInfo>::iterator iter = statuses.begin(); iter != statuses.end(); iter++) {
		// Val/skillid pairs must be ordered in the packet by status value ascending, this is done for us by std::map
		if (iter->first != StatusEffects::Mob::Empty) {
			packet.add<int16_t>(static_cast<int16_t>(iter->second.val));
			if (iter->second.skillid >= 0) {
				packet.add<int32_t>(iter->second.skillid);
			}
			else {
				packet.add<int16_t>(iter->second.mobskill);
				packet.add<int16_t>(iter->second.level);
			}
			packet.add<int16_t>(1);
		}
		else {
			packet.add<int32_t>(0);
		}
	}
}

void Mob::removeStatus(int32_t status, bool fromTimer) {
	if (hasStatus(status) && getHp() > 0) {
		StatusInfo stat = statuses[status];
		switch (status) {
			case StatusEffects::Mob::ShadowWeb:
				weblevel = 0;
				webplayerid = 0;
				Maps::getMap(mapid)->setWebbedCount(Maps::getMap(mapid)->getWebbedCount() - 1);
				break;
			case StatusEffects::Mob::MagicAttackUp:
				if (stat.skillid == Jobs::NightLord::Taunt || stat.skillid == Jobs::Shadower::Taunt) {
					taunteffect = 100;
				}
				break;
			case StatusEffects::Mob::VenomousWeapon:
				setVenomCount(0);
			case StatusEffects::Mob::Poison: // Stop poison damage timer
				getTimers()->removeTimer(Timer::Id(Timer::Types::MobStatusTimer, status, 1));
				break;
		}
		if (!fromTimer) {
			getTimers()->removeTimer(Timer::Id(Timer::Types::MobStatusTimer, status, 0));
		}
		this->status -= status;
		statuses.erase(status);
		MobsPacket::removeStatus(this, status);
	}
}

bool Mob::hasStatus(int32_t status) const {
	return ((this->status & status) != 0);
}

bool Mob::hasImmunity() const {
	int32_t mask = StatusEffects::Mob::WeaponImmunity | StatusEffects::Mob::MagicImmunity;
	return ((status & mask) != 0 || hasReflect());
}

bool Mob::hasReflect() const {
	int32_t mask = StatusEffects::Mob::WeaponDamageReflect | StatusEffects::Mob::MagicDamageReflect;
	return ((status & mask) != 0);
}

bool Mob::hasWeaponReflect() const {
	return hasStatus(StatusEffects::Mob::WeaponDamageReflect);
}

bool Mob::hasMagicReflect() const {
	return hasStatus(StatusEffects::Mob::MagicDamageReflect);
}

int32_t Mob::getStatusValue(int32_t status) {
	return (hasStatus(status) ? statuses[status].val : 0);
}

int32_t Mob::getMagicReflection() {
	return getStatusValue(StatusEffects::Mob::MagicDamageReflect);
}

int32_t Mob::getWeaponReflection() {
	return getStatusValue(StatusEffects::Mob::WeaponDamageReflect);
}

void Mob::setControl(Player *control, bool spawn, Player *display) {
	/*if (this->control != 0)
		MobsPacket::endControlMob(this->control, this);*/
	this->control = control;
	if (control != 0)
		MobsPacket::requestControl(control, this, spawn);
	else if (getControlStatus() == Mobs::ControlStatus::ControlNone) {
		MobsPacket::requestControl(control, this, spawn, display);
	}
}

void Mob::endControl() {
	if (control != 0 && control->getMap() == getMapId())
		MobsPacket::endControlMob(control, this);
}

void Mob::die(Player *player, bool fromexplosion) {
	Map *map = Maps::getMap(mapid);

	endControl();

	Timer::Id tid(Timer::Types::MobRemoveTimer, mobid, id);
	if (map->getTimers()->checkTimer(tid) > 0) {
		map->getTimers()->removeTimer(tid);
	}

	int32_t highestdamager = giveExp(player);
	spawnDeathMobs(map);
	updateSpawnLinks();

	if (hasStatus(StatusEffects::Mob::ShadowWeb)) {
		map->setWebbedCount(map->getWebbedCount() - 1);
	}

	// Ending of death stuff
	MobsPacket::dieMob(this, fromexplosion ? 4 : 1);
	DropHandler::doDrops(highestdamager, mapid, info.level, mobid, getPos(), hasExplosiveDrop(), hasFfaDrop(), getTauntEffect());

	if (player != 0) {
		Party *party = player->getParty();
		if (party != 0) {
			vector<Player *> members = party->getPartyMembers(mapid);
			for (size_t memsize = 0; memsize < members.size(); memsize++) {
				members[memsize]->getQuests()->updateQuestMob(mobid);
			}
		}
		else {
			player->getQuests()->updateQuestMob(mobid);
		}
	}

	if (info.buff != 0) {
		map->buffPlayers(info.buff);
	}

	Instance *instance = map->getInstance();
	if (instance != 0) {
		instance->sendMessage(MobDeath, mobid, id, mapid);
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

int32_t Mob::giveExp(Player *killer) {
	int32_t highestdamager = 0;
	uint64_t highestdamage = 0;

	if (damages.size()) { // Don't really want to bother with construction of the iterators and stuff if we won't use them
		unordered_map<int32_t, PartyExp> parties;
		Party *damagerparty = 0;
		Player *damager = 0;
		for (unordered_map<int32_t, uint64_t>::iterator iter = damages.begin(); iter != damages.end(); iter++) {
			if (iter->second > highestdamage) { // Find the highest damager to give drop ownership
				highestdamager = iter->first;
				highestdamage = iter->second;
			}
			damager = Players::Instance()->getPlayer(iter->first);
			if (damager == 0 || damager->getMap() != this->mapid || damager->getHp() == 0) // Only give EXP if the damager is in the same channel, on the same map and is alive
				continue;

			damagerparty = damager->getParty();

			uint32_t exp = static_cast<uint32_t>(info.exp * ((8 * iter->second / totalhealth) + (damager == killer ? 2 : 0)) / 10);
			if (damagerparty != 0) {
				int32_t pid = damagerparty->getId();
				if (parties.find(pid) != parties.end()) {
					parties[pid].totalexp += exp;
				}
				else {
					parties[pid].totalexp = exp;
					parties[pid].party = damagerparty;
				}
				if (damager->getLevel() < parties[pid].minhitlevel) {
					parties[pid].minhitlevel = damager->getLevel();
				}
				if (iter->second > parties[pid].highestdamage) {
					parties[pid].highestdamager = damager;
					parties[pid].highestdamage = iter->second;
				}
			}
			else {
				// Account for EXP increasing junk
				int16_t hsrate = damager->getActiveBuffs()->getHolySymbolRate();
				exp = exp * getTauntEffect() / 100;
				exp *= ChannelServer::Instance()->getExprate();
				exp += ((exp * hsrate) / 100);
				Levels::giveExp(damager, exp, false, (damager == killer));
			}
		}
		if (parties.size()) {
			vector<Player *> partymembers;
			for (unordered_map<int32_t, PartyExp>::iterator partyiter = parties.begin(); partyiter != parties.end(); partyiter++) {
				damagerparty = partyiter->second.party;
				partymembers = damagerparty->getPartyMembers(getMapId());
				uint16_t totallevel = 0;
				uint16_t leechcount = 0;
				for (size_t i = 0; i < partymembers.size(); i++) {
					damager = partymembers[i];
					if (damager->getLevel() < (partyiter->second.minhitlevel - 5) && damager->getLevel() < (info.level - 5)) {
						continue;
					}
					totallevel += damager->getLevel();
					leechcount++;
				}
				for (size_t i = 0; i < partymembers.size(); i++) {
					damager = partymembers[i];
					if (damager->getLevel() < (partyiter->second.minhitlevel - 5) && damager->getLevel() < (info.level - 5)) {
						continue;
					}
					uint32_t exp = static_cast<uint32_t>(info.exp * ((8 * damager->getLevel() / totallevel) + (damager == partyiter->second.highestdamager ? 2 : 0)) / 10);
					int16_t hsrate = damager->getActiveBuffs()->getHolySymbolRate();
					exp = exp * getTauntEffect() / 100;
					exp *= ChannelServer::Instance()->getExprate();
					exp += ((exp * hsrate) / 100);
					Levels::giveExp(damager, exp, false, (damager == killer));
				}
			}
		}
	}
	return highestdamager;
}

void Mob::spawnDeathMobs(Map *map) { 
	if (getMobId() == Mobs::SummonHorntail) { // Special Horntail logic to keep Horntail units linked
		int32_t spongeid = 0;
		vector<int32_t> parts;
		for (size_t i = 0; i < info.summon.size(); i++) {
			int32_t spawnid = info.summon[i];
			if (spawnid == Mobs::HorntailSponge)
				spongeid = map->spawnMob(spawnid, m_pos, getFh(), this);
			else {
				int32_t identifier = map->spawnMob(spawnid, m_pos, getFh(), this);
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
			int32_t ident = map->spawnMob(info.summon[i], m_pos, getFh(), this);
			Mob *mob = map->getMob(ident);
			getSponge()->addSpawn(ident, mob);
		}
	}
	else {
		for (size_t i = 0; i < info.summon.size(); i++) {
			map->spawnMob(info.summon[i], m_pos, getFh(), this);
		}
	}
}

void Mob::updateSpawnLinks() {
	if (spawns.size() > 0) {
		for (unordered_map<int32_t, Mob *>::iterator spawniter = spawns.begin(); spawniter != spawns.end(); spawniter++) {
			spawniter->second->setOwner(0);
		}
	}
	if (getOwner() != 0) {
		owner->removeSpawn(getId());
	}
}

void Mob::skillHeal(int32_t basehealhp, int32_t healrange) {
	if (getMobId() == Mobs::HorntailSponge)
		return;

	int32_t amount = Randomizer::Instance()->randInt(healrange) + (basehealhp - (healrange / 2));
	int32_t original = amount;

	if (hp + amount > getMHp()) {
		amount = getMHp() - hp;
		hp = getMHp();
	}
	else {
		hp += amount;
	}
	totalhealth += amount;

	if (getSponge() != 0) {
		basehealhp = getSponge()->getHp() + amount;
		if (basehealhp < 0 || basehealhp > getSponge()->getMHp()) {
			basehealhp = getSponge()->getMHp();
		}
		getSponge()->setHp(basehealhp);
	}

	MobsPacket::healMob(this, original);
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

void Mob::mpEat(Player *player, MpEaterInfo *mp) {
	if ((mpeatercount < 3) && (getMp() > 0) && (Randomizer::Instance()->randInt(99) < mp->prop)) {
		mp->onlyonce = true;
		int32_t emp = getMMp() * mp->x / 100;

		if (emp > getMp())
			emp = getMp();
		setMp(getMp() - emp);

		if (emp > 30000)
			emp = 30000;
		player->modifyMp(static_cast<int16_t>(emp));

		SkillsPacket::showSkillEffect(player, mp->id);
		mpeatercount++;
	}
}

void Mob::setControlStatus(int8_t newstat) {
	MobsPacket::endControlMob(0, this);
	MobsPacket::spawnMob(0, this, 0, 0);
	controlstatus = newstat;
	Maps::getMap(getMapId())->updateMobControl(this);
}