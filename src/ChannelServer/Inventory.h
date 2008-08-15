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
#ifndef INVENTORY_H
#define INVENTORY_H

#define GETINVENTORY(itemid) (itemid/1000000)
#define ISARROW(x) (x/10000 == 206)
#define ISSTAR(x) (x/10000 == 207)
#define ISBULLET(x) (x/10000 == 233) // Pirate's bullets and capsules
#define ISRECHARGEABLE(x) (ISSTAR(x) || ISBULLET(x))
#define ISEQUIP(x) (GETINVENTORY(x) == 1)
#define ISPET(x) (x >= 5000000 && x <= 5000045)

#include <unordered_map>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class ItemTimer;
class ReadPacket;
struct Item;

struct EquipInfo {
	char slots;
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
	unsigned char types[8];
	vector <short> vals;
	vector <SummonBag> mobs;
	vector <Skillbook> skills;
};

struct ItemInfo {
	short maxslot;
	int price;
	bool quest;
	bool consume;
	ConsumeInfo cons;
};

namespace Inventory {
	extern unordered_map<int, EquipInfo> equips;
	extern unordered_map<int, ItemInfo> items;
	void addEquipInfo(int id, EquipInfo equip);
	void addItemInfo(int id, ItemInfo item);
	void startTimer();
	extern bool isCash(int itemid);
	extern ItemTimer * timer;
	void useShop(Player *player, ReadPacket *packet);
	void useStorage(Player *player, ReadPacket *packet);
	void itemMove(Player *player, ReadPacket *packet);
	short addItem(Player *player, Item *item, bool is = false);
	void addNewItem(Player *player, int itemid, int amount);
	void takeItem(Player *player, int itemid, int howmany);
	void useItem(Player *player, ReadPacket *packet);
	void cancelItem(Player *player, ReadPacket *packet);
	void endItem(Player *player, int itemid);
	void useSkillbook(Player *player, ReadPacket *packet);
	void takeItemSlot(Player *player, char inv, short slot, short amount, bool takeStar = false);
	void useChair(Player *player, ReadPacket *packet);
	void useItemEffect(Player *player, ReadPacket *packet);
	void stopChair(Player *player, ReadPacket *packet);
	void useSummonBag(Player *player, ReadPacket *packet);
	void useReturnScroll(Player *player, ReadPacket *packet);
	void useScroll(Player *player, ReadPacket *packet);
	void useCashItem(Player *player, ReadPacket *packet);
};

#endif
