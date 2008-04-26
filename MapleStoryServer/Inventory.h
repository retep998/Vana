#ifndef INVENTORY_H
#define INVENTORY_H

#define ISSTAR(x) (x/10000 == 207)

class Player;
struct Equip;
struct Item;

class Inventory {
public:
	static void useShop(Player* player, unsigned char* packet);
	static void itemMove(Player* player, unsigned char* packet);
	static int findSlot(Player* player, int itemid ,char inv, short amount);
	static Equip* setEquipStats(Player* player, int equipid);
	static void addEquip(Player* player, Equip* equip, bool is=0);
	static void addItem(Player* player, Item* item, bool is=0);
	static void addNewItem(Player* player, int item, int howmany);
	static void takeItem(Player* player, int item, int howmany);
	static void useItem(Player* player, unsigned char* packet);
	static void takeItemSlot(Player* player, short slot, char inv, short amount);
	static void useChair(Player* player, unsigned char* packet);
	static void stopChair(Player* player, unsigned char* packet);
	static int isCash(int itemid);
	static void useSummonBag(Player* player, unsigned char* packet);
	static void useReturnScroll(Player* player, unsigned char* packet);
	static void useScroll(Player* player, unsigned char* packet);
};

#endif