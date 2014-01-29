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
#include "Algorithm.hpp"
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

	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM shop_data");

	for (const auto &row : rs) {
		ShopInfo shop;
		int32_t shopId = row.get<int32_t>("shopid");
		shop.npc = row.get<int32_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		m_shops[shopId] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM shop_items ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		ShopItemInfo item;
		int32_t shopId = row.get<int32_t>("shopid");
		item.itemId = row.get<int32_t>("itemid");
		item.quantity = row.get<int16_t>("quantity");
		item.price = row.get<int32_t>("price");

		m_shops[shopId].items.push_back(item);
	}
}

auto ShopDataProvider::loadUserShops() -> void {
	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM user_shop_data");

	for (const auto &row : rs) {
		ShopInfo shop;
		int32_t shopId = row.get<int32_t>("shopid");
		shop.npc = row.get<int32_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		if (m_shops.find(shopId) != std::end(m_shops)) {
			m_shops.erase(shopId);
		}

		m_shops[shopId] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM user_shop_items ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		ShopItemInfo item;
		int32_t shopId = row.get<int32_t>("shopid");
		item.itemId = row.get<int32_t>("itemid");
		item.quantity = row.get<int16_t>("quantity");
		item.price = row.get<int32_t>("price");

		m_shops[shopId].items.push_back(item);
	}
}

auto ShopDataProvider::loadRechargeTiers() -> void {
	m_rechargeCosts.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM shop_recharge_data");

	for (const auto &row : rs) {
		int8_t rechargeTier = row.get<int8_t>("tierid");
		int32_t itemId = row.get<int32_t>("itemid");
		double price = row.get<double>("price");

		m_rechargeCosts[rechargeTier][itemId] = price;
	}
}

auto ShopDataProvider::isShop(int32_t id) const -> bool {
	return ext::is_element(m_shops, id);
}

auto ShopDataProvider::showShop(int32_t id, int16_t rechargeableBonus, PacketCreator &packet) const -> void {
	const auto &info = m_shops.find(id)->second;
	int8_t rechargeTier = info.rechargeTier;
	const auto &rechargables = m_rechargeCosts.find(rechargeTier)->second;
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
		auto itemInfo = ItemDataProvider::getInstance().getItemInfo(item.itemId);
		uint16_t maxSlot = itemInfo->maxSlot;
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
			packet.add<int16_t>(ItemDataProvider::getInstance().getItemInfo(kvp.first)->maxSlot + rechargeableBonus);
		}
	}

	packet.set<int16_t>(shopCount, 6);
}

auto ShopDataProvider::getShopItem(int32_t shopId, uint16_t shopIndex) const -> const ShopItemInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_shops, shopId)->items, shopIndex);
}

auto ShopDataProvider::getRechargeCost(int32_t shopId, int32_t itemId, int16_t amount) const -> int32_t {
	auto price = ext::find_value_ptr(
		ext::find_value_ptr(m_rechargeCosts,
			ext::find_value_ptr(m_shops, shopId)->rechargeTier), itemId);

	if (price != nullptr) {
		return -1 * static_cast<int32_t>(*price * amount);
	}

	return 1;
}