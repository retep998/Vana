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
#include "PacketBuilder.hpp"
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
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << Database::makeDataTable("shop_data"));

	for (const auto &row : rs) {
		ShopInfo shop;
		shop_id_t shopId = row.get<shop_id_t>("shopid");
		shop.npc = row.get<npc_id_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		m_shops[shopId] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM " << Database::makeDataTable("shop_items") << " ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		ShopItemInfo item;
		shop_id_t shopId = row.get<shop_id_t>("shopid");
		item.itemId = row.get<item_id_t>("itemid");
		item.quantity = row.get<slot_qty_t>("quantity");
		item.price = row.get<mesos_t>("price");

		m_shops[shopId].items.push_back(item);
	}
}

auto ShopDataProvider::loadUserShops() -> void {
	soci::session &sql = Database::getDataDb();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << Database::makeDataTable("user_shop_data"));

	for (const auto &row : rs) {
		ShopInfo shop;
		shop_id_t shopId = row.get<shop_id_t>("shopid");
		shop.npc = row.get<npc_id_t>("npcid");
		shop.rechargeTier = row.get<int8_t>("recharge_tier");
		if (m_shops.find(shopId) != std::end(m_shops)) {
			m_shops.erase(shopId);
		}

		m_shops[shopId] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM " << Database::makeDataTable("user_shop_items") << " ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		ShopItemInfo item;
		shop_id_t shopId = row.get<shop_id_t>("shopid");
		item.itemId = row.get<item_id_t>("itemid");
		item.quantity = row.get<slot_qty_t>("quantity");
		item.price = row.get<mesos_t>("price");

		m_shops[shopId].items.push_back(item);
	}
}

auto ShopDataProvider::loadRechargeTiers() -> void {
	m_rechargeCosts.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("shop_recharge_data"));

	for (const auto &row : rs) {
		int8_t rechargeTier = row.get<int8_t>("tierid");
		item_id_t itemId = row.get<item_id_t>("itemid");
		double price = row.get<double>("price");

		m_rechargeCosts[rechargeTier][itemId] = price;
	}
}

auto ShopDataProvider::isShop(shop_id_t id) const -> bool {
	return ext::is_element(m_shops, id);
}

auto ShopDataProvider::getShop(shop_id_t id) const -> BuiltShopInfo {
	const auto &info = m_shops.find(id)->second;

	BuiltShopInfo ret;
	ret.npc = info.npc;
	for (const auto &item : info.items) {
		ret.items.push_back(&item);
	}

	if (info.rechargeTier > 0) {
		ret.rechargeables = m_rechargeCosts.find(info.rechargeTier)->second;
	}

	return ret;
}

auto ShopDataProvider::getShopItem(shop_id_t shopId, uint16_t shopIndex) const -> const ShopItemInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_shops, shopId)->items, shopIndex);
}

auto ShopDataProvider::getRechargeCost(shop_id_t shopId, item_id_t itemId, int16_t amount) const -> mesos_t {
	auto price = ext::find_value_ptr(
		ext::find_value_ptr(m_rechargeCosts,
			ext::find_value_ptr(m_shops, shopId)->rechargeTier), itemId);

	if (price != nullptr) {
		return -1 * static_cast<mesos_t>(*price * amount);
	}

	return 1;
}