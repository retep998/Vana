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
#include <hash_map>
#include <vector>
#include <queue>

using namespace std;
using namespace stdext;

class Player;
class Mob;
class LoopingId;
class ReadPacket;

struct SpawnInfo {
	int id;
	Pos pos;
	short fh;
	int rate;
	int last;
};
typedef vector<SpawnInfo> SpawnsInfo;

struct MobInfo {
	int hp;
	int mp;
	int exp;
	char hpcolor;
	char hpbgcolor;
	bool boss;
	vector<int> summon;
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

namespace Mobs {
	extern hash_map <int, MobInfo> mobinfo;
	extern hash_map <int, SpawnsInfo> info;
	extern hash_map <int, queue<int>> respawns;
	void addMob(int id, MobInfo mob);
	void addSpawn(int id, SpawnInfo spawn);
	void damageMob(Player *player, ReadPacket *packet);
	void damageMobRanged(Player *player, ReadPacket *packet);
	void damageMobSpell(Player *player, ReadPacket *packet);
	void damageMobPG(Player *player, int damage, Mob *mob);
	void displayHPBars(Player *player, vector <Player*> players, const MobHPInfo &mob);
	void monsterControl(Player *player, ReadPacket *packet);
	void checkSpawn(int mapid);
	void spawnMob(Player *player, int mobid, int amount = 1);
	void spawnMobPos(int mapid, int mobid, Pos pos);
	void dieMob(Player *player, Mob *mob);
};

class Mob {
public:
	Mob(int mapid, int id, int mobid, Pos pos, int spawnid, int fh);
	void setPos(Pos pos) {
		this->pos = pos;
	}
	Pos getPos() {
		return pos;
	}
	int getPosX() {
		return pos.x;
	}
	int getPosY() {
		return pos.y;
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
	void setType(char type) {
		this->type = type;
	}
	char getType() {
		return type;
	}
	void setControl(Player *control);
	Player * getControl() {
		return control;
	}
private:
	Pos pos;
	int id;
	int mapid;
	int spawnid;
	int mobid;
	short fh;
	int hp;
	int mp;
	char type;
	Player *control;
};

#endif
