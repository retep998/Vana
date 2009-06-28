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

#include "GameConstants.h"
#include "MobDataProvider.h"
#include "MovableLife.h"
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
class PacketCreator;
class PacketReader;
struct MobSkillLevelInfo;
struct MpEaterInfo;

struct StatusInfo {
	StatusInfo() : status(0), val(0), skillid(0), mobskill(0), level(0), time(0), reflection(-1) { }
	StatusInfo(int32_t status, int16_t val, int32_t skillid, clock_t time);
	StatusInfo(int32_t status, int16_t val, int16_t mobskill, int16_t level, clock_t time);
	StatusInfo(int32_t status, int16_t val, int16_t mobskill, int16_t level, int32_t reflect, clock_t time);
	int32_t status;
	int32_t skillid;
	int32_t reflection;
	int16_t val;
	int16_t mobskill;
	int16_t level;
	clock_t time;
};

namespace Mobs {
	extern const int32_t mobstatuses[StatusEffects::Mob::Count];
	int32_t handleMobStatus(Player *player, Mob *mob, int32_t skillid, uint8_t level, uint8_t weapon_type, int8_t hits, int32_t damage = 0);
	void handleMobSkill(Mob *mob, uint8_t skillid, uint8_t level, const MobSkillLevelInfo &skillinfo);
	void handleBomb(Player *player, PacketReader &packet);
	void monsterControl(Player *player, PacketReader &packet);
	void checkSpawn(int32_t mapid);
};

class Mob : public MovableLife {
public:
	Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid = -1, int16_t fh = 0);

	void applyDamage(int32_t playerid, int32_t damage, bool poison = false);
	void applyWebDamage();
	void setHp(int32_t hp) { this->hp = hp; }
	void setMp(int32_t mp) { this->mp = mp; }
	void addStatus(int32_t playerid, vector<StatusInfo> &statusinfo);
	void removeStatus(int32_t status, bool fromTimer = false);
	void setControl(Player *control);
	void endControl();
	void setOwner(Mob *owner) { this->owner = owner; }
	void setSponge(Mob *sponge) { horntailsponge = sponge; }
	void setLastSkillUse(uint8_t skill, time_t usetime) { skilluse[skill] = usetime; }
	void statusPacket(PacketCreator &packet);
	void addSpawn(int32_t mapmobid, Mob *mob) { spawns[mapmobid] = mob; }
	void removeSpawn(int32_t mapmobid) { spawns.erase(mapmobid); }
	void skillHeal(int32_t basehealhp, int32_t healrange);
	void dispelBuffs();
	void doCrashSkill(int32_t skillid);
	void explode();
	void setVenomCount(int8_t count) { venomcount = count; }

	int8_t getVenomCount() const { return venomcount; }
	int16_t getTauntEffect() const { return taunteffect; }
	int16_t getOriginFh() const { return originfh; }
	int16_t getStatusValue(int32_t status);
	int16_t getWeaponReflection();
	int16_t getMagicReflection();
	int32_t getId() const { return id; }
	int32_t getMapId() const { return mapid; }
	int32_t getMobId() const { return mobid; }
	int32_t getSpawnId() const { return spawnid; }
	int32_t getHp() const { return hp; }
	int32_t getMp() const { return mp; }
	int32_t getMHp() const { return info.hp; }
	int32_t getMMp() const { return info.mp; }
	int32_t getLink() const { return info.link; }
	int32_t getCounter() { return ++counter; }
	int32_t getSelfDestructHp() const { return info.selfdestruction; }
	time_t getLastSkillUse(uint8_t skill) { return (skilluse.find(skill) != skilluse.end() ? skilluse[skill] : 0); }
	bool isBoss() const { return info.boss; }
	bool canFreeze() const { return info.canfreeze; }
	bool canPoison() const { return info.canpoison; }
	bool isUndead() const { return info.undead; }
	bool hasLink() const { return info.link != 0; }
	bool hasExplosiveDrop() const { return info.explosivereward; }
	bool hasFfaDrop() const { return info.publicreward; }
	bool hasImmunity() const;
	bool hasReflect() const;
	bool hasWeaponReflect() const;
	bool hasMagicReflect() const;
	bool hasStatus(int32_t status) const;
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }
	Mob * getOwner() const { return owner; }
	Mob * getSponge() const { return horntailsponge; }
	vector<MobSkillInfo> getSkills() const { return info.skills; }
	unordered_map<int32_t, Mob *> getSpawns() const { return spawns; }

	Timer::Container * getTimers() const { return timers.get(); }
	Player * getControl() const { return control; }

	void die(bool showpacket = false); // Removes mob, no EXP, no summoning
private:
	int8_t venomcount;
	uint8_t weblevel;
	int16_t originfh;
	int16_t taunteffect;
	int32_t id;
	int32_t mapid;
	int32_t spawnid;
	int32_t mobid;
	int32_t hp;
	int32_t mp;
	int32_t status;
	int32_t counter;
	int32_t webplayerid;
	Mob *owner;
	Mob *horntailsponge;
	const MobInfo info;
	unordered_map<int32_t, StatusInfo> statuses;
	unordered_map<int32_t, uint32_t> damages;
	unordered_map<uint8_t, time_t> skilluse;
	unordered_map<int32_t, Mob *> spawns;
	Player *control;
	boost::scoped_ptr<Timer::Container> timers;

	void die(Player *player, bool fromexplosion = false);
};

#endif
