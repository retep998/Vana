#ifndef INVENTORY_H
#define INVENTORY_H

#define ISSTAR(x) (x/10000 == 207)

class Player;
struct Equip;
struct Item;

namespace Inventory {
	extern int findSlot(Player* player, int itemid ,char inv, short amount);
	extern int isCash(int itemid);
	extern Equip* setEquipStats(Player* player, int equipid);
	void useShop(Player* player, unsigned char* packet);
	void itemMove(Player* player, unsigned char* packet);
	void addEquip(Player* player, Equip* equip, bool is=0);
	void addItem(Player* player, Item* item, bool is=0);
	void addNewItem(Player* player, int item, int howmany);
	void takeItem(Player* player, int item, int howmany);
	void useItem(Player* player, unsigned char* packet);
	void takeItemSlot(Player* player, short slot, char inv, short amount);
	void useChair(Player* player, unsigned char* packet);
	void stopChair(Player* player, unsigned char* packet);
	void useSummonBag(Player* player, unsigned char* packet);
	void useReturnScroll(Player* player, unsigned char* packet);
	void useScroll(Player* player, unsigned char* packet);
};

#endif