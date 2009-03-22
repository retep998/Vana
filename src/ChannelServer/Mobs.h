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
#include <boost/tr1/unordered_map.hpp>
#include <vector>
#include <boost/scoped_ptr.hpp>

using std::vector;
using std::tr1::unordered_map;

class Player;
class Mob;
class PacketReader;
class PacketCreator;
struct MPEaterInfo;

enum MobStatus {
	WATK = 0x1,
	WDEF = 0x2,
	MATK = 0x4,
	MDEF = 0x8,
	ACC = 0x10,
	AVOID = 0x20,
	SPEED = 0x40,
	STUN = 0x80,
	FREEZE = 0x100,
	POISON = 0x200,
	SEAL = 0x400,
	WEAPON_ATTACK_UP = 0x1000,
	WEAPON_DEFENSE_UP = 0x2000,
	MAGIC_ATTACK_UP = 0x4000,
	MAGIC_DEFENSE_UP = 0x8000,
	DOOM = 0x10000,
	SHADOW_WEB = 0x20000,
	WEAPON_IMMUNITY = 0x40000,
	MAGIC_IMMUNITY = 0x80000
};

struct StatusInfo {
	StatusInfo() : status(0), val(0), skillid(0), mobskill(0), level(0), time(0) { }
	StatusInfo(int32_t status, int16_t val, int32_t skillid, clock_t time) : status(status), val(val), skillid(skillid), mobskill(0), level(0), time(time) { }
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
	uint32_t damageMobInternal(Player *player, PacketReader &packet, int8_t targets, int8_t hits, int32_t skillid, int32_t &extra, MPEaterInfo *eater = 0, bool ismelee = false);
	void handleMobStatus(Player *player, Mob *mob, int32_t skillid, uint8_t weapon_type);
	void monsterControl(Player *player, PacketReader &packet);
	void checkSpawn(int32_t mapid);
};

class Mob : public MovableLife {
public:
	Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid = -1, int16_t fh = 0);
	void applyDamage(int32_t playerid, int32_t damage, bool poison = false);
	void setMP(int32_t mp) { this->mp = mp; }
	void addStatus(int32_t playerid, vector<StatusInfo> statusinfo);
	void removeStatus(int32_t status);
	void setControl(Player *control);
	void endControl();
	void cleanHorntail(int32_t mapid, Player *player);

	int16_t getOriginFH() const { return originfh; }
	int32_t getID() const { return id; }
	int32_t getMapID() const { return mapid; }
	int32_t getMobID() const { return mobid; }
	int32_t getSpawnID() const { return spawnid; }
	int32_t getHP() const { return hp; }
	int32_t getMP() const { return mp; }
	int32_t getMHP() const { return info.hp; }
	int32_t getMMP() const { return info.mp; }
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }
	MobAttackInfo getAttackInfo(uint8_t id) const { return info.skills.at(id); }
	bool isBoss() const { return info.boss; }
	bool canFreeze() const { return info.canfreeze; }
	bool canPoison() const { return info.canpoison; }
	void statusPacket(PacketCreator &packet);
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
	const MobInfo info;
	unordered_map<int32_t, StatusInfo> statuses;
	boost::scoped_ptr<Timer::Container> timers;
	unordered_map<int32_t, uint32_t> damages;
	Player *control;

	void die(Player *player);
};

#endif
