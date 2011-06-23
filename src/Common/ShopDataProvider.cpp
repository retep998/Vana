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
#include "PacketCreator.h"
#include "Session.h"
#include "SmsgHeader.h"

using Initializing::OutputWidth;

ShopDataProvider * ShopDataProvider::singleton = nullptr;

void ShopDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Shops... ";

	loadShops();
	loadUserShops();
	loadRechargeTiers();

	std::cout << "DONE" << std::endl;
}

void ShopDataProvider::loadShops() {
	m_shops.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM shop_data");
	mysqlpp::UseQueryResult res = query.use();
	ShopInfo shop;
	int32_t shopId;
	MYSQL_ROW row;

	enum ShopData {
		ShopId = 0,
		NpcId, RechargeTier
	};

	while (row = res.fetch_raw_row()) {
		shopId = atoi(row[ShopId]);

		shop.npc = atoi(row[NpcId]);
		shop.rechargeTier = atoi(row[RechargeTier]);
		m_shops[shopId] = shop;
	}

	query << "SELECT * FROM shop_items ORDER BY shopId, sort DESC";
	res = query.use();
	ShopItemInfo item;

	enum ShopItemData {
		ItemShopId = 0,
		ItemId, Quantity, Price, Sort
	};

	while (row = res.fetch_raw_row()) {
		shopId = atoi(row[ItemShopId]);

		item.itemId = atoi(row[ItemId]);
		item.quantity = atoi(row[Quantity]);
		item.price = atoi(row[Price]);

		m_shops[shopId].items.push_back(item);
	}
}

void ShopDataProvider::loadUserShops() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM user_shop_data");
	mysqlpp::UseQueryResult res = query.use();
	ShopInfo shop;
	int32_t shopId;
	MYSQL_ROW row;

	enum ShopData {
		ShopId = 0,
		NpcId, RechargeTier
	};

	while (row = res.fetch_raw_row()) {
		shopId = atoi(row[ShopId]);

		shop.npc = atoi(row[NpcId]);
		shop.rechargeTier = atoi(row[RechargeTier]);
		if (m_shops.find(shopId) != m_shops.end()) {
			m_shops.erase(shopId);
		}
		m_shops[shopId] = shop;
	}

	query << "SELECT * FROM user_shop_items ORDER BY shopId, sort DESC";
	res = query.use();
	ShopItemInfo item;

	enum ShopItemData {
		ItemShopId = 0,
		ItemId, Quantity, Price, Sort
	};

	while (row = res.fetch_raw_row()) {
		shopId = atoi(row[ItemShopId]);

		item.itemId = atoi(row[ItemId]);
		item.quantity = atoi(row[Quantity]);
		item.price = atoi(row[Price]);

		m_shops[shopId].items.push_back(item);
	}
}

void ShopDataProvider::loadRechargeTiers() {
	m_rechargeCosts.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM shop_recharge_data");
	mysqlpp::UseQueryResult res = query.use();
	int8_t rechargeTier;
	int32_t itemId;
	double price;

	enum RechargeData {
		TierId = 0,
		ItemId, Price
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		rechargeTier = atoi(row[TierId]);
		itemId = atoi(row[ItemId]);
		price = atof(row[Price]);

		m_rechargeCosts[rechargeTier][itemId] = price;
	}
}

void ShopDataProvider::showShop(int32_t id, int16_t rechargeableBonus, PacketCreator &packet) {
	ShopInfo &info = m_shops[id];
	int8_t rechargeTier = info.rechargeTier;
	map<int32_t, double> &rechargables = m_rechargeCosts[rechargeTier];
	int16_t shopCount = info.items.size() + rechargables.size();
	unordered_map<int32_t, bool> idsDone;

	packet.addHeader(SMSG_SHOP);
	packet.add<int32_t>(info.npc);
	packet.add<int16_t>(0); // To be set later

	// Items
	for (size_t i = 0; i < info.items.size(); i++) {
		ShopItemInfo &item = info.items[i];
		packet.add<int32_t>(item.itemId);
		packet.add<int32_t>(item.price);
		if (GameLogicUtilities::isRechargeable(item.itemId)) {
			idsDone[item.itemId] = true;
			double cost = 0.0;
			if (rechargeTier != 0) {
				shopCount--;
				if (rechargables.find(item.itemId) != rechargables.end()) {
					cost = rechargables[item.itemId];
				}
			}
			packet.add<double>(cost);
		}
		else {
			packet.add<int16_t>(item.quantity); // Item amount
		}
		int16_t maxSlot = ItemDataProvider::Instance()->getMaxSlot(item.itemId);
		if (GameLogicUtilities::isRechargeable(item.itemId)) {
			maxSlot += rechargeableBonus;
		}
		packet.add<int16_t>(maxSlot);
	}

	// Rechargables
	for (map<int32_t, double>::iterator iter = rechargables.begin(); iter != rechargables.end(); ++iter) {
		if (idsDone.find(iter->first) == idsDone.end()) {
			packet.add<int32_t>(iter->first);
			packet.add<int32_t>(0);
			packet.add<double>(iter->second);
			packet.add<int16_t>(ItemDataProvider::Instance()->getMaxSlot(iter->first) + rechargeableBonus);
		}
	}

	packet.set<int16_t>(shopCount, 6);
}

int32_t ShopDataProvider::getPrice(int32_t shopId, uint16_t shopIndex) {
	vector<ShopItemInfo> &s = m_shops[shopId].items;
	return (shopIndex < s.size() ? s[shopIndex].price : 0);
}

int16_t ShopDataProvider::getAmount(int32_t shopId, uint16_t shopIndex) {
	vector<ShopItemInfo> &s = m_shops[shopId].items;
	return (shopIndex < s.size() ? s[shopIndex].quantity : 0);
}

int32_t ShopDataProvider::getItemId(int32_t shopId, uint16_t shopIndex) {
	vector<ShopItemInfo> &s = m_shops[shopId].items;
	return (shopIndex < s.size() ? s[shopIndex].itemId : 0);
}

int32_t ShopDataProvider::getRechargeCost(int32_t shopId, int32_t itemId, int16_t amount) {
	int32_t cost = 1;
	if (m_shops.find(shopId) != m_shops.end()) {
		int8_t tier = m_shops[shopId].rechargeTier;
		if (m_rechargeCosts.find(tier) != m_rechargeCosts.end()) {
			if (m_rechargeCosts[tier].find(itemId) != m_rechargeCosts[tier].end()) {
				cost = -1 * static_cast<int32_t>(m_rechargeCosts[tier][itemId] * amount);
			}
		}
	}
	return cost;
}