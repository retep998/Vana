#ifndef INVPACK_H
#define INVPACK_H

#include <vector>

using namespace std;

class Player;
struct Equip;
struct Item;
short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

class InventoryPacket {
public:
	static void moveItem(Player* player, char inv, short slot1, short slot2);
	static void updatePlayer(Player* player);
	static void addEquip(Player* player, Equip* equip, bool is);
	static void bought(Player* player);
	static void newMesos(Player* player, int mesos, bool is);
	static void addItem(Player* player, Item* item, bool is);
	static void addNewItem(Player* player, Item* item, bool is);
	static void moveItemS(Player* player, char inv, short slot, short amount);
	static void moveItemS2(Player* player, char inv, short slot1, short amount1, short slot2, short amount2);
	static void sitChair(Player* player, vector <Player*> players, int chairid);
	static void stopChair(Player* player, vector <Player*> players);
	static void useScroll(Player* player, vector <Player*> players, char s);
};

#endif