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
#ifndef MOBS_H
#define MOBS_H

#include "MobDataProvider.h"
#include "MovableLife.h"
#include "Player.h"
#include "Pos.h"
#include "Timer/Container.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class Mob;
class PacketReader;
class PacketCreator;
struct MpEaterInfo;

struct StatusInfo {
	StatusInfo() : status(0), val(0), skillid(0), mobskill(0), level(0), time(0) { }
	StatusInfo(int32_t status, int16_t val, int32_t skillid, clock_t time);
	StatusInfo(int32_t status, int16_t val, int16_t mobskill, int16_t level, clock_t time) : status(status), val(val), skillid(-1), mobskill(mobskill), level(level), time(time) { }
	int32_t status;
	int16_t val;
	int32_t skillid;
	int16_t mobskill;
	int16_t level;
	clock_t time;
};

namespace Mobs {
	extern const int32_t mobstatuses[19];
	void damageMob(Player *player, PacketReader &packet);
	void damageMobRanged(Player *player, PacketReader &packet);
	void damageMobSpell(Player *player, PacketReader &packet);
	void damageMobEnergyCharge(Player *player, PacketReader &packet);
	void damageMobSummon(Player *player, PacketReader &packet);
	uint32_t damageMobInternal(Player *player, PacketReader &packet, int8_t targets, int8_t hits, int32_t skillid, int32_t &extra, MpEaterInfo *eater = 0);
	void handleMobStatus(Player *player, Mob *mob, int32_t skillid, uint8_t weapon_type);
	void handleMobSkill(Mob *mob, uint8_t skillid, uint8_t level, const MobSkillLevelInfo &skillinfo);
	void handleBomb(Player *player, PacketReader &packet);
	void monsterControl(Player *player, PacketReader &packet);
	void checkSpawn(int32_t mapid);
};

class Mob : public MovableLife {
public:
	Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid = -1, int16_t fh = 0);
	void applyDamage(int32_t playerid, int32_t damage, bool poison = false);
	void setMp(int32_t mp) { this->mp = mp; }
	void addStatus(int32_t playerid, vector<StatusInfo> statusinfo);
	void removeStatus(int32_t status);
	void setControl(Player *control);
	void endControl();
	void cleanHorntail(int32_t mapid, Player *player);
	void setOwner(Mob *owner) { this->owner = owner; }
	void setLastSkillUse(uint8_t skill, time_t usetime) { skilluse[skill] = usetime; }
	void statusPacket(PacketCreator &packet);
	void addSpawn(int32_t mapmobid, Mob *mob) { spawns[mapmobid] = mob; }
	void removeSpawn(int32_t mapmobid) { spawns.erase(mapmobid); }
	void skillHeal(int32_t healhp, int32_t healmp);
	void dispelBuffs();
	void doCrashSkill(int32_t skillid);
	void setImmunity(bool isimmune) { hasimmunity = isimmune; }
	void explode();

	int16_t getOriginFh() const { return originfh; }
	int32_t getId() const { return id; }
	int32_t getMapId() const { return mapid; }
	int32_t getMobId() const { return mobid; }
	int32_t getSpawnId() const { return spawnid; }
	int32_t getHp() const { return hp; }
	int32_t getMp() const { return mp; }
	int32_t getMHp() const { return info.hp; }
	int32_t getMMp() const { return info.mp; }
	int32_t getCounter() { return ++counter; }
	int32_t getSelfDestructHp() const { return info.selfdestruction; }
	time_t getLastSkillUse(uint8_t skill) { return (skilluse.find(skill) != skilluse.end() ? skilluse[skill] : 0); }
	bool isBoss() const { return info.boss; }
	bool canFreeze() const { return info.canfreeze; }
	bool canPoison() const { return info.canpoison; }
	bool isUndead() const { return info.undead; }
	bool hasImmunity() const { return hasimmunity; }
	bool hasStatus(int32_t status);
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }
	Mob * getOwner() const { return owner; }
	MobAttackInfo getAttackInfo(uint8_t id) const { return info.attacks.at(id); }
	vector<MobSkillInfo> getSkills() const { return info.skills; }
	unordered_map<int32_t, Mob *> getSpawns() const { return spawns; }

	Timer::Container * getTimers() const { return timers.get(); }
	Player * getControl() const { return control; }

	void die(bool showpacket = false); // Removes mob, no EXP, no summoning
private:
	int16_t originfh;
	int32_t id;
	int32_t mapid;
	int32_t spawnid;
	int32_t mobid;
	int32_t hp;
	int32_t mp;
	int32_t status;
	int32_t counter;
	Mob *owner;
	const MobInfo info;
	bool hasimmunity;
	unordered_map<int32_t, StatusInfo> statuses;
	unordered_map<int32_t, uint32_t> damages;
	unordered_map<uint8_t, time_t> skilluse;
	unordered_map<int32_t, Mob *> spawns;
	Player *control;
	boost::scoped_ptr<Timer::Container> timers;

	void die(Player *player, bool fromexplosion = false);
};

#endif
