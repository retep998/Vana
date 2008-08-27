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
	int mobid;
	char id;
	unsigned char mpconsume;
	unsigned short mpburn;
	unsigned char disease;
	char level;
	bool deadlyattack;
};

struct MobInfo {
	int hp;
	int mp;
	int hprecovery;
	int mprecovery;
	int exp;
	char hpcolor;
	char hpbgcolor;
	bool boss;
	vector<int> summon;
	vector<MobAttackInfo> skills;
};

struct MobHPInfo {
	MobHPInfo() : hp(0), mhp(0), mapmobid(0), mobid(0), hpcolor(0), hpbgcolor(0), boss(0) { }
	int hp;
	int mhp;
	int mapmobid;
	int mobid;
	char hpcolor;
	char hpbgcolor;
	bool boss;
};

struct MPEaterInfo;

namespace Mobs {
	extern unordered_map<int, MobInfo> mobinfo;
	void addMob(int id, MobInfo mob);
	void damageMob(Player *player, ReadPacket *packet);
	void damageMobRanged(Player *player, ReadPacket *packet);
	void damageMobSpell(Player *player, ReadPacket *packet);
	unsigned int damageMobInternal(Player *player, ReadPacket *packet, char targets, char hits, int skillid, int &extra, MPEaterInfo *eater = 0);
	void displayHPBars(Player *player, Mob *mob);
	void monsterControl(Player *player, ReadPacket *packet);
	void checkSpawn(int mapid);
	void spawnMob(Player *player, int mobid, int amount = 1);
	void spawnMobPos(int mapid, int mobid, Pos pos);
};

class Mob {
public:
	Mob(int mapid, int mobid, Pos pos, int spawnid = -1, short fh = 0);
	void setPos(Pos pos) {
		this->pos = pos;
	}
	Pos getPos() {
		return Pos(pos.x, pos.y-1);
	}
	int getPosX() {
		return pos.x;
	}
	int getPosY() {
		return pos.y - 1;
	}
	void setID(int id) {
		this->id = id;
	}
	int getID() {
		return id;
	}
	int getMapID() {
		return mapid;
	}
	int getMobID() {
		return mobid;
	}
	int getSpawnID() {
		return spawnid;
	}
	void setFH(short fh) {
		this->fh = fh;
	}
	short getFH() {
		return fh;
	}
	void setHP(int hp) {
		this->hp = hp;
		if (this->hp < 0)
			this->hp = 0;
	}
	int getHP() {
		return hp;
	}
	void setMP(int mp) {
		this->mp = mp;
	}
	int getMP() {
		return mp;
	}
	void setStance(char stance) {
		this->stance = stance;
	}
	char getStance() {
		return stance;
	}
	void setControl(Player *control);
	Player * getControl() {
		return control;
	}
	void die(Player *player);
	void die(); // Removes mob, no EXP, no summoning
private:
	Pos pos;
	int id;
	int mapid;
	int spawnid;
	int mobid;
	short fh;
	int hp;
	int mp;
	char stance;
	Player *control;
};

#endif
