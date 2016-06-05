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
#include "DropDataProvider.hpp"
#include "Algorithm.hpp"
#include "Database.hpp"
#include "InitializeCommon.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <string>

namespace vana {

auto drop_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Drops... ";

	load_drops();
	load_global_drops();

	std::cout << "DONE" << std::endl;
}

auto drop_data_provider::load_drops() -> void {
	m_drop_info.clear();

	drop_info drop;
	auto drop_flags = [&drop](const opt_string &flags) {
		utilities::str::run_flags(flags, [&drop](const string &cmp) {
			if (cmp == "is_mesos") drop.is_mesos = true;
		});
	};

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("drop_data"));

	for (const auto &row : rs) {
		drop = drop_info{};

		int32_t dropper = row.get<int32_t>("dropperid");
		drop.item_id = row.get<game_item_id>("itemid");
		drop.min_amount = row.get<int32_t>("minimum_quantity");
		drop.max_amount = row.get<int32_t>("maximum_quantity");
		drop.quest_id = row.get<game_quest_id>("questid");
		drop.chance = row.get<uint32_t>("chance");
		drop_flags(row.get<opt_string>("flags"));

		m_drop_info[dropper].push_back(drop);
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table("user_drop_data") << " ORDER BY dropperid");
	int32_t last_dropper_id = -1;
	bool dropped = false;

	for (const auto &row : rs) {
		drop = drop_info{};

		int32_t dropper = row.get<int32_t>("dropperid");
		drop.item_id = row.get<game_item_id>("itemid");
		drop.min_amount = row.get<int32_t>("minimum_quantity");
		drop.max_amount = row.get<int32_t>("maximum_quantity");
		drop.quest_id = row.get<game_quest_id>("questid");
		drop.chance = row.get<uint32_t>("chance");
		drop_flags(row.get<opt_string>("flags"));

		if (dropper != last_dropper_id) {
			dropped = false;
		}
		if (!dropped && m_drop_info.find(dropper) != std::end(m_drop_info)) {
			m_drop_info.erase(dropper);
			dropped = true;
		}
		m_drop_info[dropper].push_back(drop);
		last_dropper_id = dropper;
	}
}

auto drop_data_provider::load_global_drops() -> void {
	m_global_drops.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("drop_global_data"));

	for (const auto &row : rs) {
		global_drop_info drop;

		drop.continent = row.get<int8_t>("continent");
		drop.item_id = row.get<game_item_id>("itemid");
		drop.min_amount = row.get<game_slot_qty>("minimum_quantity");
		drop.max_amount = row.get<game_slot_qty>("maximum_quantity");
		drop.min_level = row.get<game_player_level>("minimum_level");
		drop.max_level = row.get<game_player_level>("maximum_level");
		drop.quest_id = row.get<game_quest_id>("questid");
		drop.chance = row.get<uint32_t>("chance");
		utilities::str::run_flags(row.get<opt_string>("flags"), [&drop](const string &cmp) {
			if (cmp == "is_mesos") drop.is_mesos = true;
		});

		m_global_drops.push_back(drop);
	}
}

auto drop_data_provider::has_drops(int32_t object_id) const -> bool {
	return ext::is_element(m_drop_info, object_id);
}

auto drop_data_provider::get_drops(int32_t object_id) const -> const vector<drop_info> & {
	return m_drop_info.find(object_id)->second;
}

auto drop_data_provider::get_global_drops() const -> const vector<global_drop_info> & {
	return m_global_drops;
}

}