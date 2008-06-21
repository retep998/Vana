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
class ReadPacket;
class Map;
struct Equip;
struct Item;

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
	short tamingmob;
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

struct Skillbook{
	int skillid;
	int reqlevel;
	int maxlevel;
};

struct ConsumeInfo {
	short hp;
	short mp;
	short hpr;
	short mpr;
	int moveTo;
	int time;
	short watk;
	short matk;
	short avo;
	short acc;
	short wdef;
	short mdef;
	short speed;
	short jump;
	short morph;
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
	vector <Skillbook> skills;
};

class Drop;

namespace Drops {
	extern hash_map <int, MobDropsInfo> dropsinfo;
	extern hash_map <int, Mesos> mesos;
	extern hash_map <int, EquipInfo> equips;
	extern hash_map <int, ItemInfo> items;
	extern hash_map <int, ConsumeInfo> consumes;
	void addDrop(int id, MobDropInfo drop);
	void addEquip(int id, EquipInfo equip);
	void addItem(int id, ItemInfo item);
	void addConsume(int id, ConsumeInfo cons);
	void addMesos(int id, Mesos meso);
	void dropMesos(Player *player, ReadPacket *packet);
	void lootItem(Player *player, ReadPacket *packet);
	void dropMob(Player *player, Mob *mob);
};

class Drop {
private:
	int mapid;
	int id;
	bool ismesos;
	int mesos;
	bool isequip;
	Equip equip;
	Item item;
	int owner;
	int time;
	int questid;
	int dropped;
	int playerid;
	Pos pos;
public:
	Drop (int mapid, int mesos, Pos pos, int owner);
	Drop (int mapid, Equip equip, Pos pos, int owner);
	Drop (int mapid, Item item, Pos pos, int owner);
	void setID(int id) {
		this->id = id;
	}
	int getID() {
		return id;
	}
	int getObjectID();
	int getAmount();
	bool isQuest() {
		return questid > 0;
	}
	void setQuest(int questid) {
		this->questid = questid;
	}
	int getQuest() {
		return questid;
	}
	void setTime(int time) {
		this->time = time;
	}
	int getTime() {
		return time;
	}
	int getOwner() {
		return owner;
	}
	int getMap() {
		return mapid;
	}
	bool isMesos() {
		return ismesos;
	}
	bool isEquip() {
		return isequip;
	}
	Equip getEquip() {
		return equip;
	}
	Item getItem() {
		return item;
	}
	void setItemAmount(short amount) {
		this->item.amount = amount;
	}
	void setPos(Pos pos) {
		this->pos = pos;
	}
	Pos getPos() {
		return pos;
	}
	void setDropped(int time) {
		dropped = time;
	}
	int getDropped() {
		return dropped;
	}
	void setPlayer(int playerid) {
		playerid = playerid;
	}
	void doDrop(Pos origin);
	void showDrop(Player *player);
	void takeDrop(Player *player);
	void removeDrop();
};

#endif
