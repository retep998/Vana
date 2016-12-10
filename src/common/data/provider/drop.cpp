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
#include "drop.hpp"
#include "common/algorithm.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <string>

namespace vana {
namespace data {
namespace provider {

auto drop::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Drops... ";

	load_drops();
	load_global_drops();

	std::cout << "DONE" << std::endl;
}

auto drop::load_drops() -> void {
	m_drop_info.clear();

	data::type::drop_info info;
	auto drop_flags = [&info](const opt_string &flags) {
		vana::util::str::run_flags(flags, [&info](const string &cmp) {
			if (cmp == "is_mesos") info.is_mesos = true;
		});
	};

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::drop_data));

	for (const auto &row : rs) {
		info = data::type::drop_info{};

		int32_t dropper = row.get<int32_t>("dropperid");
		info.item_id = row.get<game_item_id>("itemid");
		info.min_amount = row.get<int32_t>("minimum_quantity");
		info.max_amount = row.get<int32_t>("maximum_quantity");
		info.quest_id = row.get<game_quest_id>("questid");
		info.chance = row.get<uint32_t>("chance");
		drop_flags(row.get<opt_string>("flags"));

		bool found = false;
		for (auto &drop : m_drop_info) {
			if (drop.first == dropper) {
				found = true;
				drop.second.push_back(info);
				break;
			}
		}

		if (!found) {
			vector<data::type::drop_info> current;
			current.push_back(info);
			m_drop_info.emplace_back(dropper, current);
		}
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::user_drop_data) << " ORDER BY dropperid");
	int32_t last_dropper_id = -1;

	for (const auto &row : rs) {
		info = data::type::drop_info{};

		int32_t dropper = row.get<int32_t>("dropperid");
		info.item_id = row.get<game_item_id>("itemid");
		info.min_amount = row.get<int32_t>("minimum_quantity");
		info.max_amount = row.get<int32_t>("maximum_quantity");
		info.quest_id = row.get<game_quest_id>("questid");
		info.chance = row.get<uint32_t>("chance");
		drop_flags(row.get<opt_string>("flags"));

		if (dropper != last_dropper_id) {
			// Check to see if a drop is required
			bool removed = ext::remove_element_if(
				m_drop_info,
				[&dropper](auto value) {
					return value.first == dropper;
				});

			if (removed) {
				vector<data::type::drop_info> current;
				m_drop_info.emplace_back(dropper, current);
			}
		}

		for (auto &drop : m_drop_info) {
			if (drop.first == dropper) {
				drop.second.push_back(info);
				break;
			}
		}

		last_dropper_id = dropper;
	}
}

auto drop::load_global_drops() -> void {
	m_global_drops.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::drop_global_data));

	for (const auto &row : rs) {
		data::type::global_drop_info drop;

		drop.continent = row.get<int8_t>("continent");
		drop.item_id = row.get<game_item_id>("itemid");
		drop.min_amount = row.get<game_slot_qty>("minimum_quantity");
		drop.max_amount = row.get<game_slot_qty>("maximum_quantity");
		drop.min_level = row.get<game_player_level>("minimum_level");
		drop.max_level = row.get<game_player_level>("maximum_level");
		drop.quest_id = row.get<game_quest_id>("questid");
		drop.chance = row.get<uint32_t>("chance");
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&drop](const string &cmp) {
			if (cmp == "is_mesos") drop.is_mesos = true;
		});

		m_global_drops.push_back(drop);
	}
}

auto drop::get_drops(int32_t object_id) const -> const vector<data::type::drop_info> & {
	for (const auto &drop : m_drop_info) {
		if (drop.first == object_id) {
			return drop.second;
		}
	}

	static vector<data::type::drop_info> empty;
	return empty;
}

auto drop::get_global_drops() const -> const vector<data::type::global_drop_info> & {
	return m_global_drops;
}

}
}
}