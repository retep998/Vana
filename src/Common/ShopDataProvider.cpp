/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "ShopDataProvider.hpp"
#include "Database.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "ItemDataProvider.hpp"
#include "PacketCreator.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include <iomanip>
#include <iostream>

auto ShopDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Shops... ";

	loadShops();
	loadUserShops();
	loadRechargeTiers();

	std::cout << "DONE" << std::endl;
}

auto ShopDataProvider::loadShops() -> void {
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

auto ShopDataProvider::loadUserShops() -> void {
	ShopInfo shop;
	int32_t shopId;

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM user_shop_data");

	for (const auto &row : rs) {
		shop = ShopInfo();
		shopId = row.get<int32_t>("shopid");
		shop.npc = row.get<int32_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		if (m_shops.find(shopId) != std::end(m_shops)) {
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

auto ShopDataProvider::loadRechargeTiers() -> void {
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

auto ShopDataProvider::showShop(int32_t id, int16_t rechargeableBonus, PacketCreator &packet) -> void {
	ShopInfo &info = m_shops[id];
	int8_t rechargeTier = info.rechargeTier;
	const ord_map_t<int32_t, double> &rechargables = m_rechargeCosts[rechargeTier];
	int16_t shopCount = static_cast<int16_t>(info.items.size() + rechargables.size());
	hash_map_t<int32_t, bool> idsDone;

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
				if (kvp != std::end(rechargables)) {
					cost = kvp->second;
				}
			}
			packet.add<double>(cost);
		}
		else {
			packet.add<int16_t>(item.quantity); // Item amount
		}
		int16_t maxSlot = ItemDataProvider::getInstance().getMaxSlot(item.itemId);
		if (GameLogicUtilities::isRechargeable(item.itemId)) {
			maxSlot += rechargeableBonus;
		}
		packet.add<int16_t>(maxSlot);
	}

	// Rechargables
	for (const auto &kvp : rechargables) {
		if (idsDone.find(kvp.first) == std::end(idsDone)) {
			packet.add<int32_t>(kvp.first);
			packet.add<int32_t>(0);
			packet.add<double>(kvp.second);
			packet.add<int16_t>(ItemDataProvider::getInstance().getMaxSlot(kvp.first) + rechargeableBonus);
		}
	}

	packet.set<int16_t>(shopCount, 6);
}

auto ShopDataProvider::getPrice(int32_t shopId, uint16_t shopIndex) -> int32_t {
	vector_t<ShopItemInfo> &s = m_shops[shopId].items;
	return (shopIndex < s.size() ? s[shopIndex].price : 0);
}

auto ShopDataProvider::getAmount(int32_t shopId, uint16_t shopIndex) -> int16_t {
	vector_t<ShopItemInfo> &s = m_shops[shopId].items;
	return (shopIndex < s.size() ? s[shopIndex].quantity : 0);
}

auto ShopDataProvider::getItemId(int32_t shopId, uint16_t shopIndex) -> int32_t {
	vector_t<ShopItemInfo> &s = m_shops[shopId].items;
	return (shopIndex < s.size() ? s[shopIndex].itemId : 0);
}

auto ShopDataProvider::getRechargeCost(int32_t shopId, int32_t itemId, int16_t amount) -> int32_t {
	int32_t cost = 1;
	auto kvp = m_shops.find(shopId);
	if (kvp != std::end(m_shops)) {
		int8_t tier = kvp->second.rechargeTier;
		auto rechargeKvp = m_rechargeCosts.find(tier);
		if (rechargeKvp != std::end(m_rechargeCosts)) {
			auto itemKvp = rechargeKvp->second.find(itemId);
			if (itemKvp != std::end(rechargeKvp->second)) {
				cost = -1 * static_cast<int32_t>(itemKvp->second * amount);
			}
		}
	}
	return cost;
}