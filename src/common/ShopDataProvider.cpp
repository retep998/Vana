/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "CommonHeader.hpp"
#include "Database.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "ItemDataProvider.hpp"
#include "PacketBuilder.hpp"
#include "Session.hpp"
#include <iomanip>
#include <iostream>

namespace vana {

auto shop_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Shops... ";

	load_shops();
	load_user_shops();
	load_recharge_tiers();

	std::cout << "DONE" << std::endl;
}

auto shop_data_provider::load_shops() -> void {
	m_shops.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("shop_data"));

	for (const auto &row : rs) {
		shop_info shop;
		game_shop_id shop_id = row.get<game_shop_id>("shopid");
		shop.npc = row.get<game_npc_id>("npcid");
		shop.recharge_tier = row.get<int8_t>("recharge_tier");
		m_shops[shop_id] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table("shop_items") << " ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		shop_item_info item;
		game_shop_id shop_id = row.get<game_shop_id>("shopid");
		item.item_id = row.get<game_item_id>("itemid");
		item.quantity = row.get<game_slot_qty>("quantity");
		item.price = row.get<game_mesos>("price");

		m_shops[shop_id].items.push_back(item);
	}
}

auto shop_data_provider::load_user_shops() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("user_shop_data"));

	for (const auto &row : rs) {
		shop_info shop;
		game_shop_id shop_id = row.get<game_shop_id>("shopid");
		shop.npc = row.get<game_npc_id>("npcid");
		shop.recharge_tier = row.get<int8_t>("recharge_tier");
		if (m_shops.find(shop_id) != std::end(m_shops)) {
			m_shops.erase(shop_id);
		}

		m_shops[shop_id] = shop;
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table("user_shop_items") << " ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		shop_item_info item;
		game_shop_id shop_id = row.get<game_shop_id>("shopid");
		item.item_id = row.get<game_item_id>("itemid");
		item.quantity = row.get<game_slot_qty>("quantity");
		item.price = row.get<game_mesos>("price");

		m_shops[shop_id].items.push_back(item);
	}
}

auto shop_data_provider::load_recharge_tiers() -> void {
	m_recharge_costs.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("shop_recharge_data"));

	for (const auto &row : rs) {
		int8_t recharge_tier = row.get<int8_t>("tierid");
		game_item_id item_id = row.get<game_item_id>("itemid");
		double price = row.get<double>("price");

		m_recharge_costs[recharge_tier][item_id] = price;
	}
}

auto shop_data_provider::is_shop(game_shop_id id) const -> bool {
	return ext::is_element(m_shops, id);
}

auto shop_data_provider::get_shop(game_shop_id id) const -> shop_data {
	const auto &info = m_shops.find(id)->second;

	shop_data ret;
	ret.npc = info.npc;
	for (const auto &item : info.items) {
		ret.items.push_back(&item);
	}

	if (info.recharge_tier > 0) {
		ret.rechargeables = m_recharge_costs.find(info.recharge_tier)->second;
	}

	return ret;
}

auto shop_data_provider::get_shop_item(game_shop_id shop_id, uint16_t shop_index) const -> const shop_item_info * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_shops, shop_id)->items, shop_index);
}

auto shop_data_provider::get_recharge_cost(game_shop_id shop_id, game_item_id item_id, game_slot_qty amount) const -> game_mesos {
	auto price = ext::find_value_ptr(
		ext::find_value_ptr(m_recharge_costs,
			ext::find_value_ptr(m_shops, shop_id)->recharge_tier), item_id);

	if (price != nullptr) {
		return -1 * static_cast<game_mesos>(*price * amount);
	}

	return 1;
}

}