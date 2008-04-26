#ifndef SHOPS_H
#define SHOPS_H

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

class Player;

struct ShopItemInfo {
	int id;
	int price;
};

struct ShopInfo {
	int npc;
	vector <ShopItemInfo> items;
};

class Shops{
public:
	static hash_map <int, ShopInfo> shops;
	static void addShop(int id, ShopInfo shop){
		shops[id] = shop;
	}
	static void showShop(Player* player, int id);
	static int getPrice(Player* player, int itemid);
};

#endif