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
#include "reactor_data_provider.hpp"
#include "common/database_temp.hpp"
#include "common/initialize_common.hpp"
#include "common/string_utilities.hpp"
#include <iomanip>
#include <iostream>
#include <string>

namespace vana {

auto reactor_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Reactors... ";

	load_reactors();
	load_states();
	load_trigger_skills();

	std::cout << "DONE" << std::endl;
}

auto reactor_data_provider::load_reactors() -> void {
	m_reactor_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("reactor_data"));

	for (const auto &row : rs) {
		reactor_info reactor;
		game_reactor_id id = row.get<game_reactor_id>("reactorid");
		reactor.max_states = row.get<int8_t>("max_states");
		reactor.link = row.get<game_reactor_id>("link");

		utilities::str::run_flags(row.get<opt_string>("flags"), [&reactor](const string &cmp) {
			if (cmp == "remove_in_field_set") reactor.remove_in_field_set = true;
			else if (cmp == "activate_by_touch") reactor.activate_by_touch = true;
		});

		m_reactor_info[id] = reactor;
	}
}

auto reactor_data_provider::load_states() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("reactor_events") << " ORDER BY reactorId, state ASC");

	for (const auto &row : rs) {
		reactor_state_info state;
		game_reactor_id id = row.get<game_reactor_id>("reactorid");
		int8_t state_id = row.get<int8_t>("state");
		state.item_id = row.get<game_item_id>("itemid");
		state.item_quantity = row.get<game_slot_qty>("quantity");
		state.dimensions = rect{
			point{row.get<game_coord>("ltx"), row.get<game_coord>("lty")},
			point{row.get<game_coord>("rbx"), row.get<game_coord>("rby")}
		};
		state.next_state = row.get<int8_t>("next_state");
		state.timeout = row.get<int32_t>("timeout");

		utilities::str::run_enum(row.get<string>("event_type"), [&state](const string &cmp) {
			if (cmp == "plain_advance_state") state.type = 0;
			else if (cmp == "no_clue") state.type = 0;
			else if (cmp == "no_clue2") state.type = 0;
			else if (cmp == "hit_from_left") state.type = 2;
			else if (cmp == "hit_from_right") state.type = 3;
			else if (cmp == "hit_by_skill") state.type = 5;
			else if (cmp == "hit_by_item") state.type = 100;
		});

		m_reactor_info[id].states[state_id].push_back(state);
	}
}

auto reactor_data_provider::load_trigger_skills() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("reactor_event_trigger_skills"));

	for (const auto &row : rs) {
		game_reactor_id id = row.get<game_reactor_id>("reactorid");
		int8_t state = row.get<int8_t>("state");
		game_skill_id skill_id = row.get<game_skill_id>("skillid");

		for (size_t j = 0; j < m_reactor_info[id].states[state].size(); ++j) {
			m_reactor_info[id].states[state][j].trigger_skills.push_back(skill_id);
		}
	}
}

auto reactor_data_provider::get_reactor_data(game_reactor_id reactor_id, bool respect_link) const -> const reactor_info & {
	auto kvp = m_reactor_info.find(reactor_id);
	if (respect_link && kvp->second.link != 0) {
		kvp = m_reactor_info.find(kvp->second.link);
	}
	return kvp->second;
}

}