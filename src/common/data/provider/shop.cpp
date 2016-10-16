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
#include "shop.hpp"
#include "common/algorithm.hpp"
#include "common/common_header.hpp"
#include "common/data/initialize.hpp"
#include "common/data/provider/item.hpp"
#include "common/io/database.hpp"
#include "common/packet_builder.hpp"
#include "common/session.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto shop::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Shops... ";

	load_shops();
	load_user_shops();
	load_recharge_tiers();

	std::cout << "DONE" << std::endl;
}

auto shop::load_shops() -> void {
	m_shops.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::shop_data));

	for (const auto &row : rs) {
		data::type::shop_info info;
		info.id = row.get<game_shop_id>("shopid");
		info.npc = row.get<game_npc_id>("npcid");
		info.recharge_tier = row.get<int8_t>("recharge_tier");
		m_shops.push_back(info);
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::shop_items) << " ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		data::type::shop_item_info info;
		game_shop_id shop_id = row.get<game_shop_id>("shopid");
		info.item_id = row.get<game_item_id>("itemid");
		info.quantity = row.get<game_slot_qty>("quantity");
		info.price = row.get<game_mesos>("price");

		bool found = false;
		for (auto &shop : m_shops) {
			if (shop.id == shop_id) {
				found = true;
				shop.items.push_back(info);
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto shop::load_user_shops() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::user_shop_data));

	for (const auto &row : rs) {
		data::type::shop_info info;
		info.id = row.get<game_shop_id>("shopid");
		info.npc = row.get<game_npc_id>("npcid");
		info.recharge_tier = row.get<int8_t>("recharge_tier");
		
		for (size_t i = 0; i < m_shops.size(); ++i) {
			if (m_shops[i].id == info.id) {
				m_shops.erase(std::begin(m_shops) + i);
				break;
			}
		}

		m_shops.push_back(info);
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::user_shop_items) << " ORDER BY shopid, sort DESC");

	for (const auto &row : rs) {
		data::type::shop_item_info info;
		game_shop_id shop_id = row.get<game_shop_id>("shopid");
		info.item_id = row.get<game_item_id>("itemid");
		info.quantity = row.get<game_slot_qty>("quantity");
		info.price = row.get<game_mesos>("price");

		bool found = false;
		for (auto &shop : m_shops) {
			if (shop.id == shop_id) {
				found = true;
				shop.items.push_back(info);
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto shop::load_recharge_tiers() -> void {
	m_recharge_costs.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::shop_recharge_data));

	for (const auto &row : rs) {
		int8_t recharge_tier = row.get<int8_t>("tierid");
		game_item_id item_id = row.get<game_item_id>("itemid");
		double price = row.get<double>("price");

		bool found = false;
		for (auto &tier : m_recharge_costs) {
			if (tier.first == recharge_tier) {
				tier.second.push_back(std::make_pair(item_id, price));
				found = true;
				break;
			}
		}
		if (!found) {
			vector<pair<game_item_id, double>> new_value{std::make_pair(item_id, price)};
			m_recharge_costs.push_back(std::make_pair(recharge_tier, new_value));
		}
	}
}

auto shop::is_shop(game_shop_id id) const -> bool {
	for (const auto &shop : m_shops) {
		if (shop.id == id) {
			return true;
		}
	}

	return false;
}

auto shop::get_shop(game_shop_id id) const -> shop_data {
	const data::type::shop_info * info = nullptr;
	for (const auto &shop : m_shops) {
		if (shop.id == id) {
			info = &shop;
		}
	}

	if (info == nullptr) THROW_CODE_EXCEPTION(codepath_invalid_exception);

	shop_data ret;
	ret.npc = info->npc;
	for (const auto &item : info->items) {
		ret.items.push_back(&item);
	}

	if (info->recharge_tier > 0) {
		bool found = false;
		for (const auto &tier : m_recharge_costs) {
			if (tier.first == info->recharge_tier) {
				for (const auto &item : tier.second) {
					ret.rechargeables[item.first] = item.second;
				}
				found = true;
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}

	return ret;
}

auto shop::get_shop_item(game_shop_id shop_id, uint16_t shop_index) const -> const data::type::shop_item_info * const {
	const data::type::shop_item_info *item = nullptr;
	for (const auto &shop : m_shops) {
		if (shop.id == shop_id) {
			item = &shop.items[shop_index];
			break;
		}
	}

	if (item == nullptr) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	return item;
}

auto shop::get_recharge_cost(game_shop_id shop_id, game_item_id item_id, game_slot_qty amount) const -> game_mesos {
	int8_t recharge_tier = -1;
	bool found = false;
	for (const auto &shop : m_shops) {
		if (shop.id == shop_id) {
			recharge_tier = shop.recharge_tier;
			found = true;
			break;
		}
	}
	if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);

	found = false;
	double recharge_cost = 1.;
	for (const auto &shop_cost : m_recharge_costs) {
		if (shop_cost.first == recharge_tier) {
			for (const auto &item_recharge_cost : shop_cost.second) {
				if (item_recharge_cost.first == item_id) {
					recharge_cost = item_recharge_cost.second;
					found = true;
					break;
				}
			}
			break;
		}
	}

	if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);

	return static_cast<game_mesos>(recharge_cost * amount);
}

}
}
}