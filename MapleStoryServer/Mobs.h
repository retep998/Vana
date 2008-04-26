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
	vector<int> summon;
};



class Mobs {
public:
	static hash_map <int, MobInfo> mobinfo;
	static hash_map <int, SpawnsInfo> info;
	static hash_map <int, vector<Mob*>> mobs;
	static int mobscount;
	static void addMob(int id, MobInfo mob){
		mobinfo[id] = mob;
	}
	static void addSpawn(int id, SpawnsInfo spawns){
		info[id] = spawns;
	}
	static void damageMob(Player* player, unsigned char* packet);
	static void damageMobS(Player* player, unsigned char* packet, int size);
	static void damageMobSkill(Player* player, unsigned char* packet);
	static void monsterControl(Player* player, unsigned char* packet, int size);
	static void monsterControlSkill(Player* player, unsigned char* packet){}
	static void checkSpawn(int mapid);
	static void showMobs(Player* player);
	static void updateSpawn(int mapid);
	static void spawnMob(Player* player, int mobid);
	static void dieMob(Player* player, Mob* mob);
	static void spawnMobPos(Player* player, int mobid, int xx, int yy);
	static Mob* getMobByID(int mobid, int map);
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