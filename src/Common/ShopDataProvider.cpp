/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "ItemDataProvider.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "SendHeader.h"

using Initializing::outputWidth;

ShopDataProvider * ShopDataProvider::singleton = 0;

void ShopDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Shops... ";
	shops.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT shopdata.shopid, shopdata.npcid, shopdata.rechargetier FROM shopdata");
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW shopRow;
	ShopInfo shop;
	while (shopRow = res.fetch_raw_row()) {
		// Col0 : Shop ID
		//    1 : NPC ID
		//    2 : Recharge Tier
		int32_t shopid = atoi(shopRow[0]);
		shop.npc = atoi(shopRow[1]);
		shop.rechargetier = atoi(shopRow[2]);
		shops[shopid] = shop;
	}

	query << "SELECT * FROM shopitemdata ORDER BY shopitemdata.shopid, shopitemdata.sort DESC";
	res = query.use();

	int32_t currentid = 0;
	int32_t previousid = -1;
	ShopItemInfo item;
	while (shopRow = res.fetch_raw_row()) {
		// Col0 : Shop ID
		//    1 : Item ID
		//    2 : Quantity
		//    3 : Price
		//    4 : Sort
		currentid = atoi(shopRow[0]);
		if (previousid == -1) {
			shop = shops[currentid];
		}
		else if (previousid != -1 && currentid != previousid) { // Add the items into the cache
			shops[previousid] = shop;
			shop = shops[currentid];
		}

		item.itemid = atoi(shopRow[1]);
		item.quantity = atoi(shopRow[2]);
		item.price = atoi(shopRow[3]);

		shop.items.push_back(item);

		previousid = currentid;
	}

	if (previousid != -1) {
		shop.items.push_back(item);
		shops[previousid] = shop;
	}

	rechargecosts.clear();
	query << "SELECT * FROM rechargedata";
	res = query.use();

	while (shopRow = res.fetch_raw_row()) {
		// Col0 : Recharge Tier ID
		//    1 : Item ID
		//    2 : Price
		int8_t rechargetier = atoi(shopRow[0]);
		int32_t itemid = atoi(shopRow[1]);
		double price = atof(shopRow[2]);

		rechargecosts[rechargetier][itemid] = price;
	}

	std::cout << "DONE" << std::endl;
}

void ShopDataProvider::showShop(int32_t id, int16_t rechargeablebonus, PacketCreator &packet) {
	unordered_map<int32_t, bool> idsdone;
	int8_t rechargetier = shops[id].rechargetier;
	map<int32_t, double> rechargables = rechargecosts[rechargetier];
	int16_t shopcount = shops[id].items.size() + rechargables.size();

	packet.add<int16_t>(SEND_SHOP_OPEN);
	packet.add<int32_t>(shops[id].npc);
	packet.add<int16_t>(0); // To be set later

	// Items
	ShopItemInfo item;
	for (size_t i = 0; i < shops[id].items.size(); i++) {
		item = shops[id].items[i];
		packet.add<int32_t>(item.itemid);
		packet.add<int32_t>(item.price);
		if (GameLogicUtilities::isRechargeable(item.itemid)) {
			idsdone[item.itemid] = true;
			double cost = 0.0;
			if (rechargetier != 0) {
				shopcount--;
				if (rechargables.find(item.itemid) != rechargables.end())
					cost = rechargables[item.itemid];
			}
			packet.add<double>(cost);
		}
		else {
			packet.add<int16_t>(item.quantity); // Item amount
		}
		int16_t maxslot = ItemDataProvider::Instance()->getMaxSlot(item.itemid);
		if (GameLogicUtilities::isRechargeable(item.itemid))
			maxslot += rechargeablebonus;
		packet.add<int16_t>(maxslot);
	}

	// Rechargables
	for (map<int32_t, double>::iterator iter = rechargables.begin(); iter != rechargables.end(); iter++) {
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
	vector<ShopItemInfo> s = shops[shopid].items;
	return (shopindex < s.size() ? s[shopindex].price : 0);
}

int16_t ShopDataProvider::getAmount(int32_t shopid, uint16_t shopindex) {
	vector<ShopItemInfo> s = shops[shopid].items;
	return (shopindex < s.size() ? s[shopindex].quantity : 0);
}

int32_t ShopDataProvider::getItemId(int32_t shopid, uint16_t shopindex) {
	vector<ShopItemInfo> s = shops[shopid].items;
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