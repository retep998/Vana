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

namespace Shops{
	extern hash_map <int, ShopInfo> shops;
	void addShop(int id, ShopInfo shop);
	void showShop(Player* player, int id);
	int getPrice(Player* player, int itemid);
};

#endif