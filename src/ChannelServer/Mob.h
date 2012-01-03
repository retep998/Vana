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
#pragma once

#include "GameConstants.h"
#include "MobDataProvider.h"
#include "MovableLife.h"
#include "Pos.h"
#include "TimerContainer.h"
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
	StatusInfo() : status(0), val(0), skillId(0), mobSkill(0), level(0), time(0), reflection(-1) { }
	StatusInfo(int32_t status, int32_t val, int32_t skillId, clock_t time);
	StatusInfo(int32_t status, int32_t val, int16_t mobSkill, int16_t level, clock_t time);
	StatusInfo(int32_t status, int32_t val, int16_t mobSkill, int16_t level, int32_t reflect, clock_t time);
	int32_t status;
	int32_t skillId;
	int32_t reflection;
	int32_t val;
	int16_t mobSkill;
	int16_t level;
	clock_t time;
};

struct PartyExp {
	PartyExp() : totalExp(0), party(nullptr), highestDamager(nullptr), highestDamage(0), minHitLevel(Stats::PlayerLevels) { }
	uint8_t minHitLevel;
	uint32_t totalExp;
	uint64_t highestDamage;
	Player *highestDamager;
	Party *party;
};

class Mob : public MovableLife {
public:
	Mob(int32_t id, int32_t mapId, int32_t mobId, const Pos &pos, int16_t fh = 0, int8_t controlStatus = 1);
	Mob(int32_t id, int32_t mapId, int32_t mobId, const Pos &pos, int32_t spawnId, int8_t direction, int16_t fh);

	void applyDamage(int32_t playerId, int32_t damage, bool poison = false);
	void applyWebDamage();
	void setHp(int32_t hp) { m_hp = hp; }
	void setMp(int32_t mp) { m_mp = mp; }
	void addStatus(int32_t playerId, vector<StatusInfo> &statusInfo);
	void removeStatus(int32_t status, bool fromTimer = false);
	void setControl(Player *control, bool spawn = false, Player *display = nullptr);
	void endControl();
	void setOwner(Mob *owner) { m_owner = owner; }
	void setSponge(Mob *sponge) { m_sponge = sponge; }
	void setLastSkillUse(uint8_t skill, time_t useTime) { m_skillUse[skill] = useTime; }
	void statusPacket(PacketCreator &packet);
	void addSpawn(int32_t mapMobId, Mob *mob) { m_spawns[mapMobId] = mob; }
	void removeSpawn(int32_t mapMobId) { m_spawns.erase(mapMobId); }
	void skillHeal(int32_t healHp, int32_t healRange);
	void dispelBuffs();
	void doCrashSkill(int32_t skillId);
	void explode();
	void setVenomCount(int8_t count) { m_venomCount = count; }
	void mpEat(Player *player, MpEaterInfo *mp);
	void naturalHealHp(int32_t amount);
	void naturalHealMp(int32_t amount);
	void setControlStatus(int8_t newStat);

	int8_t getControlStatus() const { return m_controlStatus; }
	int8_t getVenomCount() const { return m_venomCount; }
	int8_t getFacingDirection() const { return m_facingDirection; }
	int8_t getHpBarColor() const { return m_info->hpColor; }
	int8_t getHpBarBgColor() const { return m_info->hpBackgroundColor; }
	int16_t getOriginFh() const { return m_originFh; }
	uint16_t getLevel() const { return m_info->level; }
	int32_t getWeaponReflection();
	int32_t getMagicReflection();
	int32_t getStatusValue(int32_t status);
	int32_t getId() const { return m_id; }
	int32_t getMapId() const { return m_mapId; }
	int32_t getMobId() const { return m_mobId; }
	int32_t getSpawnId() const { return m_spawnId; }
	int32_t getHp() const { return m_hp; }
	int32_t getMp() const { return m_mp; }
	int32_t getMaxHp() const { return m_info->hp; }
	int32_t getMaxMp() const { return m_info->mp; }
	int32_t getLink() const { return m_info->link; }
	int32_t getDeathBuff() const { return m_info->buff; }
	int32_t getExp() const { return m_info->exp; }
	int32_t getCounter() { return ++m_counter; }
	int32_t getSelfDestructHp() const { return m_info->selfDestruction; }
	int32_t getTauntEffect() const { return m_tauntEffect; }
	time_t getLastSkillUse(uint8_t skill) { return (m_skillUse.find(skill) != m_skillUse.end() ? m_skillUse[skill] : 0); }
	bool isBoss() const { return m_info->boss; }
	bool canFreeze() const { return m_info->canFreeze; }
	bool canPoison() const { return m_info->canPoison; }
	bool canFly() const { return m_info->flying; }
	bool isFriendly() const { return m_info->friendly; }
	bool isUndead() const { return m_info->undead; }
	bool hasLink() const { return m_info->link != 0; }
	bool hasExplosiveDrop() const { return m_info->explosiveReward; }
	bool hasFfaDrop() const { return m_info->publicReward; }
	bool hasImmunity() const;
	bool hasReflect() const;
	bool hasWeaponReflect() const;
	bool hasMagicReflect() const;
	bool hasStatus(int32_t status) const;
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }
	Mob * getOwner() const { return m_owner; }
	Mob * getSponge() const { return m_sponge; }
	const MobInfo getInfo() const { return m_info; }
	uint8_t getSkillCount() const { return m_info->skillCount; }
	int16_t getSpawnCount() const { return static_cast<int16_t>(m_spawns.size()); }
	unordered_map<int32_t, Mob *> getSpawns() const { return m_spawns; }

	Timer::Container * getTimers() const { return m_timers.get(); }
	Player * getControl() const { return m_controller; }

	void die(bool showPacket = false); // Removes mob, no EXP, no summoning
private:
	void initMob();
	void die(Player *player, bool fromExplosion = false);
	int32_t giveExp(Player *killer);
	void spawnDeathMobs(Map *map);
	void updateSpawnLinks();

	int8_t m_venomCount;
	int8_t m_mpEaterCount;
	int8_t m_facingDirection;
	int8_t m_controlStatus;
	uint8_t m_webLevel;
	int16_t m_originFh;
	int32_t m_tauntEffect;
	int32_t m_id;
	int32_t m_mapId;
	int32_t m_spawnId;
	int32_t m_mobId;
	int32_t m_hp;
	int32_t m_mp;
	int32_t m_status;
	int32_t m_counter;
	int32_t m_webPlayerId;
	uint64_t m_totalHealth;
	Mob *m_owner;
	Mob *m_sponge;
	const MobInfo m_info;
	map<int32_t, StatusInfo> m_statuses;
	unordered_map<int32_t, uint64_t> m_damages;
	unordered_map<uint8_t, time_t> m_skillUse;
	unordered_map<int32_t, Mob *> m_spawns;
	Player *m_controller;
	boost::scoped_ptr<Timer::Container> m_timers;
};