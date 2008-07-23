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
#ifndef SHOPS_H
#define SHOPS_H

#include <hash_map>
#include <vector>

using std::vector;
using stdext::hash_map;

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
	extern vector <int> rechargables; // IDs of items which are rechargable i.e. stars
	void addShop(int id, ShopInfo shop);
	void showShop(Player *player, int id);
	int getPrice(Player *player, int itemid);
};

#endif
