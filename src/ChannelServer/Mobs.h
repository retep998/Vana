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

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

#include "Player.h"

class Player;
class Mob;

struct SpawnInfo {
	int id;
	short x;
	short cy;
	short fh;
	int rate;
	int last;
};
typedef vector<SpawnInfo> SpawnsInfo;

struct MobInfo {
	int hp;
	int mp;
	int exp;
	bool boss;
	vector<int> summon;
};

namespace Mobs {
	extern hash_map <int, MobInfo> mobinfo;
	extern hash_map <int, SpawnsInfo> info;
	extern hash_map <int, vector<Mob*>> mobs;
	extern int mobscount;
	void addMob(int id, MobInfo mob);
	void addSpawn(int id, SpawnsInfo spawns);
	void damageMob(Player* player, unsigned char* packet);
	void damageMobS(Player* player, unsigned char* packet, int size);
	void damageMobSkill(Player* player, unsigned char* packet);
	void monsterControl(Player* player, unsigned char* packet, int size);
	void monsterControlSkill(Player* player, unsigned char* packet);
	void checkSpawn(int mapid);
	void showMobs(Player* player);
	void updateSpawn(int mapid);
	void spawnMob(Player* player, int mobid);
	void dieMob(Player* player, Mob* mob);
	void spawnMobPos(Player* player, int mobid, int xx, int yy);
	Mob* getMobByID(int mobid, int map);
};

class Mob {
public:
	Mob (){
		control=NULL;
	}
	void setPos(Pos pos){
		this->pos = pos;
	}
	Pos getPos(){
		return pos;
	}
	void setID(int id){
		this->id=id;
	}
	int getID(){
		return id;
	}
	void setMobID(int mobid){
		this->mobid=mobid;
	}
	int getMobID(){
		return mobid;
	}
	void setMapID(int mapid){
		this->mapid=mapid;
	}
	int getMapID(){
		return mapid;
	}
	void setFH(short fh){
		this->fh=fh;
	}
	short getFH(){
		return fh;
	}
	void setHP(int hp){
		this->hp=hp;
		if(this->hp<0)
			this->hp=0;
	}
	int getHP(){
		return hp;
	}
	void setMP(int mp){
		this->mp=mp;
	}
	int getMP(){
		return mp;
	}
	void setType(char type){
		this->type=type;
	}
	char getType(){
		return type;
	}
	void setControl(Player* control);
	Player* getControl(){
		return control;
	}
private:
	Pos pos;
	int id;
	int mapid;
	int mobid;
	short fh;
	int hp;
	int mp;
	char type;
	Player* control;
};

#endif
