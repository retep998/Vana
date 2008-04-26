#ifndef DROPS_H
#define DROPS_H

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

#include "Players.h"
#include "Player.h"

class Player;
class Mob;

struct Dropped {
	int id;
	Pos pos;
};

struct DropInfo {
	char slots;
	short scrolls;
	char type;
	short istr;
	short idex;
	short iint;
	short iluk;
	short ihp;
	short imp;
	short iwatk;
	short imatk;
	short iwdef;
	short imdef;
	short iacc;
	short iavo;
	short ihand;
	short ijump;
	short ispeed;
};

struct MobDropInfo {
	int id;
	int chance;
	int quest;
};
typedef vector<MobDropInfo> MobDropsInfo;

struct Mesos {
	int min;
	int max;
};

struct EquipInfo {
	char slots;
	char type;
	int price;
	short istr;
	short idex;
	short iint;
	short iluk;
	short ihp;
	short imp;
	short iwatk;
	short imatk;
	short iwdef;
	short imdef;
	short iacc;
	short iavo;
	short ihand;
	short ijump;
	short ispeed;
	bool cash; 
	bool quest;
};

struct ItemInfo {
	char type;
	short maxslot;
	int price;
	bool quest;
	bool consume;
};

struct SummonBag {
	int mobid;
	int chance;
};

struct ConsumeInfo {
	short hp;
	short mp;
	short hpr;
	short mpr;
	int moveTo;
	int success;
	int cursed;
	short istr;
	short idex;
	short iint;
	short iluk;
	short ihp;
	short imp;
	short iwatk;
	short imatk;
	short iwdef;
	short imdef;
	short iacc;
	short iavo;
	short ihand;
	short ijump;
	short ispeed;
	vector <SummonBag> mobs;
};
struct FootholdInfo {
	short x1;
	short y1;
	short x2;
	short y2;
};
typedef vector<FootholdInfo> FootholdsInfo;

class Drop;

class Drops {
public:
	static hash_map <int, MobDropsInfo> dropsinfo;
	static hash_map <int, Mesos> mesos;
	static hash_map <int, EquipInfo> equips;
	static hash_map <int, ItemInfo> items;
	static hash_map <int, ConsumeInfo> consumes;
	static hash_map <int, FootholdsInfo> foots;
	static hash_map <int, int> objids;
	static hash_map <int, vector<Drop*>> drops;
	static void addDrop(int id, MobDropsInfo drops){
		dropsinfo[id] = drops;
	}
	static void addEquip(int id, EquipInfo equip){
		equips[id] = equip;
	}
	static void addItem(int id, ItemInfo item){
		items[id] = item;
	}
	static void addConsume(int id, ConsumeInfo cons){
		consumes[id] = cons;
	}
	static void addMesos(int id, Mesos meso){
		mesos[id] = meso;
	}
	static void addFoothold(int id, FootholdsInfo foot){
		foots[id] = foot;
	}
	static void dropMesos(Player* player, unsigned char* packet);
	static void lootItem(Player* player, unsigned char* packet);
	static void dropMob(Player* player, Mob* mob);
	static void showDrops(Player* player);
	static Pos findFloor(Pos pos, int map);
	static void checkDrops(int mapid);
};

class Drop {
private:
	int id;
	int objid;
	int owner;
	int map;
	short amount;
	bool ismeso;
	bool isequip;
	int time;
	int questid;
	bool quest;
	int playerid;
	int dropped;
	Pos pos;
	DropInfo info;
public:
	Drop(int mapid){
		quest=0;
		playerid = 0;
		ismeso=0;
		isequip=0;
		map = mapid;
		questid=0;
		dropped=0;
		objid = Drops::objids[mapid]++;
		if(Drops::drops.find(mapid) == Drops::drops.end())
			Drops::drops[mapid];
		Drops::drops[mapid].push_back(this);
	}
	void setID(int id){
		this->id = id;
	}
	int getID(){
		return id;
	}
	void setObjID(int objid){
		this->objid = objid;
	}
	int getObjID(){
		return objid;
	}
	void setIsQuest(bool is){
		quest = is;
	}
	void setPlayer(int playerid){
		this->playerid = playerid;
	}
	int getPlayer(){
		return playerid;
	}
	bool isQuest(){
		return quest;
	}
	void setQuest(int questid){
		this->questid = questid;
	}
	int getQuest(){
		return questid;
	}
	void setTime(int time){
		this->time = time;
	}
	int getTime(){
		return time;
	}
	void setOwner(int owner){
		this->owner = owner;
	}
	int getOwner(){
		return owner;
	}
	void setMap(int map){
		this->map = map;
	}
	int getMap(){
		return map;
	}
	void setMesos(bool mesos){
		this->ismeso = mesos;
	}
	bool getMesos(){
		return ismeso;
	}
	void setAmount(short amount){
		this->amount = amount;
	}
	short getAmount(){
		return amount;
	}
	void setEquip(bool isequip){
		this->isequip = isequip;
	}
	bool getEquip(){
		return isequip;
	}
	void setPos(Pos pos){
		this->pos = pos;
	}
	Pos getPos(){
		return pos;
	}
	void setDropInfo(DropInfo info){
		this->info = info;
	}
	DropInfo getDropInfo(){
		return info;
	}
	void setDropped(int time){
		dropped=time;
	}
	int getDropped(){
		return dropped;
	}
	static Drop* getDrop(int objid, int mapid);
	void doDrop(Dropped dropped);
	void showDrop(Player* player);
	void takeDrop(Player* player);
	void removeDrop();
	void setStats(int id);
	void setRandomStats(int id);
};

#endif