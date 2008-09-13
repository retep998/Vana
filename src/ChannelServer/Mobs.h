/*
Copyright (C) 2008 Vana Development Team

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

#include "MovableLife.h"
#include "Player.h"
#include "Pos.h"
#include <unordered_map>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class Mob;
class LoopingId;
class ReadPacket;

struct MobAttackInfo {
	MobAttackInfo() : mobid(-1), id(0), mpconsume(0), mpburn(0), level(0), deadlyattack(false) { }
	int32_t mobid;
	char id;
	unsigned char mpconsume;
	uint16_t mpburn;
	unsigned char disease;
	char level;
	bool deadlyattack;
};

struct MobInfo {
	uint16_t hp;
	uint16_t mp;
	uint16_t hprecovery;
	uint16_t mprecovery;
	int32_t exp;
	char hpcolor;
	char hpbgcolor;
	bool boss;
	vector<int32_t> summon;
	vector<MobAttackInfo> skills;
};

struct MobHPInfo {
	MobHPInfo() : hp(0), mhp(0), mapmobid(0), mobid(0), hpcolor(0), hpbgcolor(0), boss(0) { }
	int32_t hp;
	int32_t mhp;
	int32_t mapmobid;
	int32_t mobid;
	char hpcolor;
	char hpbgcolor;
	bool boss;
};

struct MPEaterInfo;

namespace Mobs {
	extern unordered_map<int32_t, MobInfo> mobinfo;
	void addMob(int32_t id, MobInfo mob);
	void damageMob(Player *player, ReadPacket *packet);
	void damageMobRanged(Player *player, ReadPacket *packet);
	void damageMobSpell(Player *player, ReadPacket *packet);
	uint32_t damageMobInternal(Player *player, ReadPacket *packet, char targets, char hits, int32_t skillid, int32_t &extra, MPEaterInfo *eater = 0);
	void displayHPBars(Player *player, Mob *mob);
	void monsterControl(Player *player, ReadPacket *packet);
	void checkSpawn(int32_t mapid);
	void spawnMob(Player *player, int32_t mobid, int32_t amount = 1);
	void spawnMobPos(int32_t mapid, int32_t mobid, Pos pos);
};

class Mob : public MovableLife {
public:
	Mob(int32_t id, int32_t mapid, int32_t mobid, Pos pos, int32_t spawnid = -1, int16_t fh = 0);
	Pos getPos() {
		return Pos(getPosX(), getPosY());
	}
	int16_t getPosX() {
		return m_pos.x;
	}
	int16_t getPosY() {
		return m_pos.y - 1;
	}
	void setID(int32_t id) {
		this->id = id;
	}
	int32_t getID() {
		return id;
	}
	int32_t getMapID() {
		return mapid;
	}
	int32_t getMobID() {
		return mobid;
	}
	int32_t getSpawnID() {
		return spawnid;
	}
	void setHP(int16_t hp) {
		this->hp = hp;
		if (this->hp < 0)
			this->hp = 0;
	}
	int16_t getHP() {
		return hp;
	}
	void setMP(int16_t mp) {
		this->mp = mp;
	}
	int16_t getMP() {
		return mp;
	}
	void setControl(Player *control);
	Player * getControl() {
		return control;
	}
	void die(Player *player);
	void die(); // Removes mob, no EXP, no summoning
private:
	int32_t id;
	int32_t mapid;
	int32_t spawnid;
	int32_t mobid;
	int16_t hp;
	int16_t mp;
	Player *control;
};

#endif
