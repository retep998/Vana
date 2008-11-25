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
#ifndef SHOPDATA_H
#define SHOPDATA_H

#include "Types.h"
#include <tr1/unordered_map>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;

struct ShopInfo {
	int32_t npc;
	vector<int32_t> items;
	unordered_map<int32_t, int32_t> prices;
};

class ShopDataProvider {
public:
	static ShopDataProvider * Instance() {
		if (singleton == 0)
			singleton = new ShopDataProvider();
		return singleton;
	}
	void loadData();
	void addRechargable(int32_t itemid) {
		rechargables.push_back(itemid);
	}
	bool showShop(Player *player, int32_t id);
	int32_t getPrice(int32_t shopid, int32_t itemid);

private:
	ShopDataProvider() {}
	static ShopDataProvider *singleton;

	unordered_map<int32_t, ShopInfo> shops;
	vector<int32_t> rechargables;
};

#endif
