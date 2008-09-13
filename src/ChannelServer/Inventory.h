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

#define GETINVENTORY(itemid) static_cast<uint8_t>(itemid/1000000)
#define ISARROW(x) (x/10000 == 206)
#define ISSTAR(x) (x/10000 == 207)
#define ISBULLET(x) (x/10000 == 233) // Pirate's bullets and capsules
#define ISRECHARGEABLE(x) (ISSTAR(x) || ISBULLET(x))
#define ISEQUIP(x) (GETINVENTORY(x) == 1)
#define ISPET(x) (x >= 5000000 && x <= 5000053)
#define GETWEAPONTYPE(x) (x/10000)

#include "Types.h"
#include <unordered_map>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class ItemTimer;
class ReadPacket;
struct Item;

enum {
	WEAPON_1H_SWORD = 130,
	WEAPON_1H_AXE = 131,
	WEAPON_1H_MACE = 132,
	WEAPON_DAGGER = 133,
	WEAPON_WAND = 137,
	WEAPON_STAFF = 138,
	WEAPON_2H_SWORD = 140,
	WEAPON_2H_AXE = 141,
	WEAPON_2H_MACE = 142,
	WEAPON_SPEAR = 143,
	WEAPON_POLEARM = 144,
	WEAPON_BOW = 145,
	WEAPON_CROSSBOW = 146,
	WEAPON_CLAW = 147,
	WEAPON_KNUCKLE = 148,
	WEAPON_GUN = 149
};

struct EquipInfo {
	int8_t slots;
	int32_t price;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	int16_t tamingmob;
	bool cash; 
	bool quest;
};

struct SummonBag {
	int32_t mobid;
	uint32_t chance;
};

struct Skillbook {
	int32_t skillid;
	uint8_t reqlevel;
	uint8_t maxlevel;
};

struct ConsumeInfo {
	int16_t hp;
	int16_t mp;
	int16_t hpr;
	int16_t mpr;
	int32_t moveTo;
	int32_t time;
	int16_t watk;
	int16_t matk;
	int16_t avo;
	int16_t acc;
	int16_t wdef;
	int16_t mdef;
	int16_t speed;
	int16_t jump;
	int16_t morph;
	int16_t success;
	int16_t cursed;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	uint8_t types[8];
	vector<int16_t> vals;
	vector<SummonBag> mobs;
	vector<Skillbook> skills;
};

struct ItemInfo {
	int16_t maxslot;
	int32_t price;
	bool quest;
	bool consume;
	ConsumeInfo cons;
};

namespace Inventory {
	extern unordered_map<int32_t, EquipInfo> equips;
	extern unordered_map<int32_t, ItemInfo> items;
	void addEquipInfo(int32_t id, EquipInfo equip);
	void addItemInfo(int32_t id, ItemInfo item);
	void startTimer();
	extern bool isCash(int32_t itemid);
	extern ItemTimer * timer;
	void useShop(Player *player, ReadPacket *packet);
	void useStorage(Player *player, ReadPacket *packet);
	void itemMove(Player *player, ReadPacket *packet);
	int16_t addItem(Player *player, Item *item, bool is = false);
	void addNewItem(Player *player, int32_t itemid, int16_t amount);
	void takeItem(Player *player, int32_t itemid, uint16_t howmany);
	void useItem(Player *player, ReadPacket *packet);
	void cancelItem(Player *player, ReadPacket *packet);
	void endItem(Player *player, int32_t itemid);
	void useSkillbook(Player *player, ReadPacket *packet);
	void takeItemSlot(Player *player, int8_t inv, int16_t slot, int16_t amount, bool takeStar = false);
	void useChair(Player *player, ReadPacket *packet);
	void useItemEffect(Player *player, ReadPacket *packet);
	void stopChair(Player *player, ReadPacket *packet);
	void useSummonBag(Player *player, ReadPacket *packet);
	void useReturnScroll(Player *player, ReadPacket *packet);
	void useScroll(Player *player, ReadPacket *packet);
	void useCashItem(Player *player, ReadPacket *packet);
};

#endif
