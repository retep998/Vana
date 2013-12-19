/*
Copyright (C) 2008-2013 Vana Development Team

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
#include <iomanip>
#include <iostream>

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
	ShopInfo shop;
	int32_t shopId;

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM shop_data");

	for (const auto &row : rs) {
		shop = ShopInfo();
		shopId = row.get<int32_t>("shopid");
		shop.npc = row.get<int32_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		m_shops[shopId] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM shop_items ORDER BY shopid, sort DESC");
	ShopItemInfo item;

	for (const auto &row : rs) {
		item = ShopItemInfo();
		shopId = row.get<int32_t>("shopid");
		item.itemId = row.get<int32_t>("itemid");
		item.quantity = row.get<int16_t>("quantity");
		item.price = row.get<int32_t>("price");

		m_shops[shopId].items.push_back(item);
	}
}

void ShopDataProvider::loadUserShops() {
	ShopInfo shop;
	int32_t shopId;

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM user_shop_data");

	for (const auto &row : rs) {
		shop = ShopInfo();
		shopId = row.get<int32_t>("shopid");
		shop.npc = row.get<int32_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		if (m_shops.find(shopId) != m_shops.end()) {
			m_shops.erase(shopId);
		}

		m_shops[shopId] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM user_shop_items ORDER BY shopid, sort DESC");
	ShopItemInfo item;

	for (const auto &row : rs) {
		item = ShopItemInfo();
		shopId = row.get<int32_t>("shopid");
		item.itemId = row.get<int32_t>("itemid");
		item.quantity = row.get<int16_t>("quantity");
		item.price = row.get<int32_t>("price");

		m_shops[shopId].items.push_back(item);
	}
}

void ShopDataProvider::loadRechargeTiers() {
	m_rechargeCosts.clear();
	int8_t rechargeTier;
	int32_t itemId;
	double price;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM shop_recharge_data");

	for (const auto &row : rs) {
		rechargeTier = row.get<int8_t>("tierid");
		itemId = row.get<int32_t>("itemid");
		price = row.get<double>("price");

		m_rechargeCosts[rechargeTier][itemId] = price;
	}
}

void ShopDataProvider::showShop(int32_t id, int16_t rechargeableBonus, PacketCreator &packet) {
	ShopInfo &info = m_shops[id];
	int8_t rechargeTier = info.rechargeTier;
	const map<int32_t, double> &rechargables = m_rechargeCosts[rechargeTier];
	int16_t shopCount = static_cast<int16_t>(info.items.size() + rechargables.size());
	unordered_map<int32_t, bool> idsDone;

	packet.add<header_t>(SMSG_SHOP);
	packet.add<int32_t>(info.npc);
	packet.add<int16_t>(0); // To be set later

	// Items
	for (const auto &item : info.items) {
		packet.add<int32_t>(item.itemId);
		packet.add<int32_t>(item.price);
		if (GameLogicUtilities::isRechargeable(item.itemId)) {
			idsDone[item.itemId] = true;
			double cost = 0.0;
			if (rechargeTier != 0) {
				shopCount--;
				auto kvp = rechargables.find(item.itemId);
				if (kvp != rechargables.end()) {
					cost = kvp->second;
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
	for (const auto &kvp : rechargables) {
		if (idsDone.find(kvp.first) == idsDone.end()) {
			packet.add<int32_t>(kvp.first);
			packet.add<int32_t>(0);
			packet.add<double>(kvp.second);
			packet.add<int16_t>(ItemDataProvider::Instance()->getMaxSlot(kvp.first) + rechargeableBonus);
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
	auto kvp = m_shops.find(shopId);
	if (kvp != m_shops.end()) {
		int8_t tier = kvp->second.rechargeTier;
		auto rechargeKvp = m_rechargeCosts.find(tier);
		if (rechargeKvp != m_rechargeCosts.end()) {
			auto itemKvp = rechargeKvp->second.find(itemId);
			if (itemKvp != rechargeKvp->second.end()) {
				cost = -1 * static_cast<int32_t>(itemKvp->second * amount);
			}
		}
	}
	return cost;
}