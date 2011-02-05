/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "ShopDataProvider.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "SendHeader.h"

using Initializing::outputWidth;

ShopDataProvider * ShopDataProvider::singleton = nullptr;

void ShopDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Shops... ";

	loadShops();
	loadUserShops();
	loadRechargeTiers();

	std::cout << "DONE" << std::endl;
}

void ShopDataProvider::loadShops() {
	shops.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM shop_data");
	mysqlpp::UseQueryResult res = query.use();
	ShopInfo shop;
	int32_t shopid;
	MYSQL_ROW row;

	enum ShopData {
		ShopId = 0,
		NpcId, RechargeTier
	};

	while (row = res.fetch_raw_row()) {
		shopid = atoi(row[ShopId]);

		shop.npc = atoi(row[NpcId]);
		shop.rechargetier = atoi(row[RechargeTier]);
		shops[shopid] = shop;
	}

	query << "SELECT * FROM shop_items ORDER BY shopid, sort DESC";
	res = query.use();
	ShopItemInfo item;

	enum ShopItemData {
		ItemShopId = 0,
		ItemId, Quantity, Price, Sort
	};

	while (row = res.fetch_raw_row()) {
		shopid = atoi(row[ItemShopId]);

		item.itemid = atoi(row[ItemId]);
		item.quantity = atoi(row[Quantity]);
		item.price = atoi(row[Price]);

		shops[shopid].items.push_back(item);
	}
}

void ShopDataProvider::loadUserShops() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM user_shop_data");
	mysqlpp::UseQueryResult res = query.use();
	ShopInfo shop;
	int32_t shopid;
	MYSQL_ROW row;

	enum ShopData {
		ShopId = 0,
		NpcId, RechargeTier
	};

	while (row = res.fetch_raw_row()) {
		shopid = atoi(row[ShopId]);

		shop.npc = atoi(row[NpcId]);
		shop.rechargetier = atoi(row[RechargeTier]);
		if (shops.find(shopid) != shops.end()) {
			shops.erase(shopid);
		}
		shops[shopid] = shop;
	}

	query << "SELECT * FROM user_shop_items ORDER BY shopid, sort DESC";
	res = query.use();
	ShopItemInfo item;

	enum ShopItemData {
		ItemShopId = 0,
		ItemId, Quantity, Price, Sort
	};

	while (row = res.fetch_raw_row()) {
		shopid = atoi(row[ItemShopId]);

		item.itemid = atoi(row[ItemId]);
		item.quantity = atoi(row[Quantity]);
		item.price = atoi(row[Price]);

		shops[shopid].items.push_back(item);
	}
}

void ShopDataProvider::loadRechargeTiers() {
	rechargecosts.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM shop_recharge_data");
	mysqlpp::UseQueryResult res = query.use();
	int8_t rechargetier;
	int32_t itemid;
	double price;

	enum RechargeData {
		TierId = 0,
		ItemId, Price
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		rechargetier = atoi(row[TierId]);
		itemid = atoi(row[ItemId]);
		price = atof(row[Price]);

		rechargecosts[rechargetier][itemid] = price;
	}
}

void ShopDataProvider::showShop(int32_t id, int16_t rechargeablebonus, PacketCreator &packet) {
	ShopInfo &info = shops[id];
	int8_t rechargetier = info.rechargetier;
	map<int32_t, double> &rechargables = rechargecosts[rechargetier];
	int16_t shopcount = info.items.size() + rechargables.size();
	unordered_map<int32_t, bool> idsdone;

	packet.addHeader(SMSG_SHOP);
	packet.add<int32_t>(info.npc);
	packet.add<int16_t>(0); // To be set later

	// Items
	for (size_t i = 0; i < info.items.size(); i++) {
		ShopItemInfo &item = info.items[i];
		packet.add<int32_t>(item.itemid);
		packet.add<int32_t>(item.price);
		if (GameLogicUtilities::isRechargeable(item.itemid)) {
			idsdone[item.itemid] = true;
			double cost = 0.0;
			if (rechargetier != 0) {
				shopcount--;
				if (rechargables.find(item.itemid) != rechargables.end()) {
					cost = rechargables[item.itemid];
				}
			}
			packet.add<double>(cost);
		}
		else {
			packet.add<int16_t>(item.quantity); // Item amount
		}
		int16_t maxslot = ItemDataProvider::Instance()->getMaxSlot(item.itemid);
		if (GameLogicUtilities::isRechargeable(item.itemid)) {
			maxslot += rechargeablebonus;
		}
		packet.add<int16_t>(maxslot);
	}

	// Rechargables
	for (map<int32_t, double>::iterator iter = rechargables.begin(); iter != rechargables.end(); ++iter) {
		if (idsdone.find(iter->first) == idsdone.end()) {
			packet.add<int32_t>(iter->first);
			packet.add<int32_t>(0);
			packet.add<double>(iter->second);
			packet.add<int16_t>(ItemDataProvider::Instance()->getMaxSlot(iter->first) + rechargeablebonus);
		}
	}

	packet.set<int16_t>(shopcount, 6);
}

int32_t ShopDataProvider::getPrice(int32_t shopid, uint16_t shopindex) {
	vector<ShopItemInfo> &s = shops[shopid].items;
	return (shopindex < s.size() ? s[shopindex].price : 0);
}

int16_t ShopDataProvider::getAmount(int32_t shopid, uint16_t shopindex) {
	vector<ShopItemInfo> &s = shops[shopid].items;
	return (shopindex < s.size() ? s[shopindex].quantity : 0);
}

int32_t ShopDataProvider::getItemId(int32_t shopid, uint16_t shopindex) {
	vector<ShopItemInfo> &s = shops[shopid].items;
	return (shopindex < s.size() ? s[shopindex].itemid : 0);
}

int32_t ShopDataProvider::getRechargeCost(int32_t shopid, int32_t itemid, int16_t amount) {
	int32_t cost = 1;
	if (shops.find(shopid) != shops.end()) {
		int8_t tier = shops[shopid].rechargetier;
		if (rechargecosts.find(tier) != rechargecosts.end()) {
			if (rechargecosts[tier].find(itemid) != rechargecosts[tier].end()) {
				cost = -1 * static_cast<int32_t>(rechargecosts[tier][itemid] * amount);
			}
		}
	}
	return cost;
}