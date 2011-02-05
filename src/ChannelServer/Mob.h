/*
Copyright (C) 2008-2011 Vana Development Team

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
#pragma once

#include "MobDataProvider.h"
#include "MovableLife.h"
#include "Pos.h"
#include "Timer/Container.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <vector>

using std::map;
using std::tr1::unordered_map;
using std::vector;

class Party;
class Player;
class Map;
class PacketCreator;
struct MpEaterInfo;

struct StatusInfo {
	StatusInfo() : status(0), val(0), skillid(0), mobskill(0), level(0), time(0), reflection(-1) { }
	StatusInfo(int32_t status, int32_t val, int32_t skillid, clock_t time);
	StatusInfo(int32_t status, int32_t val, int16_t mobskill, int16_t level, clock_t time);
	StatusInfo(int32_t status, int32_t val, int16_t mobskill, int16_t level, int32_t reflect, clock_t time);
	int32_t status;
	int32_t skillid;
	int32_t reflection;
	int32_t val;
	int16_t mobskill;
	int16_t level;
	clock_t time;
};

struct PartyExp {
	PartyExp() : totalexp(0), party(0), highestdamager(0), highestdamage(0), minhitlevel(200) { }
	uint8_t minhitlevel;
	uint32_t totalexp;
	uint64_t highestdamage;
	Player *highestdamager;
	Party *party;
};

class Mob : public MovableLife {
public:
	Mob(int32_t id, int32_t mapid, int32_t mobid, const Pos &pos, int16_t fh = 0, int8_t controlstatus = 1);
	Mob(int32_t id, int32_t mapid, int32_t mobid, const Pos &pos, int32_t spawnid, int8_t direction, int16_t fh);

	void applyDamage(int32_t playerid, int32_t damage, bool poison = false);
	void applyWebDamage();
	void setHp(int32_t hp) { this->hp = hp; }
	void setMp(int32_t mp) { this->mp = mp; }
	void addStatus(int32_t playerid, vector<StatusInfo> &statusinfo);
	void removeStatus(int32_t status, bool fromTimer = false);
	void setControl(Player *control, bool spawn = false, Player *display = 0);
	void endControl();
	void setOwner(Mob *owner) { this->owner = owner; }
	void setSponge(Mob *sponge) { this->sponge = sponge; }
	void setLastSkillUse(uint8_t skill, time_t usetime) { skilluse[skill] = usetime; }
	void statusPacket(PacketCreator &packet);
	void addSpawn(int32_t mapmobid, Mob *mob) { spawns[mapmobid] = mob; }
	void removeSpawn(int32_t mapmobid) { spawns.erase(mapmobid); }
	void skillHeal(int32_t basehealhp, int32_t healrange);
	void dispelBuffs();
	void doCrashSkill(int32_t skillid);
	void explode();
	void setVenomCount(int8_t count) { venomcount = count; }
	void mpEat(Player *player, MpEaterInfo *mp);
	void naturalHealHp(int32_t amount);
	void naturalHealMp(int32_t amount);
	void setControlStatus(int8_t newstat);

	int8_t getControlStatus() const { return controlstatus; }
	int8_t getVenomCount() const { return venomcount; }
	int8_t getFacingDirection() const { return facingdirection; }
	int8_t getHpBarColor() const { return info->hpColor; }
	int8_t getHpBarBgColor() const { return info->hpBackgroundColor; }
	int16_t getOriginFh() const { return originfh; }
	uint16_t getLevel() const { return info->level; }
	int32_t getWeaponReflection();
	int32_t getMagicReflection();
	int32_t getStatusValue(int32_t status);
	int32_t getId() const { return id; }
	int32_t getMapId() const { return mapid; }
	int32_t getMobId() const { return mobid; }
	int32_t getSpawnId() const { return spawnid; }
	int32_t getHp() const { return hp; }
	int32_t getMp() const { return mp; }
	int32_t getMaxHp() const { return info->hp; }
	int32_t getMaxMp() const { return info->mp; }
	int32_t getLink() const { return info->link; }
	int32_t getDeathBuff() const { return info->buff; }
	int32_t getExp() const { return info->exp; }
	int32_t getCounter() { return ++counter; }
	int32_t getSelfDestructHp() const { return info->selfDestruction; }
	int32_t getTauntEffect() const { return taunteffect; }
	time_t getLastSkillUse(uint8_t skill) { return (skilluse.find(skill) != skilluse.end() ? skilluse[skill] : 0); }
	bool isBoss() const { return info->boss; }
	bool canFreeze() const { return info->canFreeze; }
	bool canPoison() const { return info->canPoison; }
	bool canFly() const { return info->flying; }
	bool isFriendly() const { return info->friendly; }
	bool isUndead() const { return info->undead; }
	bool hasLink() const { return info->link != 0; }
	bool hasExplosiveDrop() const { return info->explosiveReward; }
	bool hasFfaDrop() const { return info->publicReward; }
	bool hasImmunity() const;
	bool hasReflect() const;
	bool hasWeaponReflect() const;
	bool hasMagicReflect() const;
	bool hasStatus(int32_t status) const;
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }
	Mob * getOwner() const { return owner; }
	Mob * getSponge() const { return sponge; }
	const MobInfo getInfo() const { return info; }
	uint8_t getSkillCount() const { return info->skillCount; }
	int16_t getSpawnCount() const { return static_cast<int16_t>(spawns.size()); }
	unordered_map<int32_t, Mob *> getSpawns() const { return spawns; }

	Timer::Container * getTimers() const { return timers.get(); }
	Player * getControl() const { return control; }

	void die(bool showpacket = false); // Removes mob, no EXP, no summoning
private:
	int8_t venomcount;
	int8_t mpeatercount;
	int8_t facingdirection;
	int8_t controlstatus;
	uint8_t weblevel;
	int16_t originfh;
	int32_t taunteffect;
	int32_t id;
	int32_t mapid;
	int32_t spawnid;
	int32_t mobid;
	int32_t hp;
	int32_t mp;
	int32_t status;
	int32_t counter;
	int32_t webplayerid;
	uint64_t totalhealth;
	Mob *owner;
	Mob *sponge;
	const MobInfo info;
	map<int32_t, StatusInfo> statuses;
	unordered_map<int32_t, uint64_t> damages;
	unordered_map<uint8_t, time_t> skilluse;
	unordered_map<int32_t, Mob *> spawns;
	Player *control;
	boost::scoped_ptr<Timer::Container> timers;

	void initMob();
	void die(Player *player, bool fromexplosion = false);
	int32_t giveExp(Player *killer);
	void spawnDeathMobs(Map *map);
	void updateSpawnLinks();
};
