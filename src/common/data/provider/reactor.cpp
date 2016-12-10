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
#include "reactor.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <string>

namespace vana {
namespace data {
namespace provider {

auto reactor::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Reactors... ";

	load_reactors();
	load_states();
	load_trigger_skills();

	std::cout << "DONE" << std::endl;
}

auto reactor::load_reactors() -> void {
	m_reactor_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::reactor_data));

	for (const auto &row : rs) {
		data::type::reactor_info reactor;
		reactor.id = row.get<game_reactor_id>("reactorid");
		reactor.max_states = row.get<int8_t>("max_states");
		reactor.link = row.get<game_reactor_id>("link");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&reactor](const string &cmp) {
			if (cmp == "remove_in_field_set") reactor.remove_in_field_set = true;
			else if (cmp == "activate_by_touch") reactor.activate_by_touch = true;
		});

		m_reactor_info.push_back(reactor);
	}
}

auto reactor::load_states() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::reactor_events) << " ORDER BY reactorId, state ASC");

	for (const auto &row : rs) {
		data::type::reactor_state_info state;
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

		vana::util::str::run_enum(row.get<string>("event_type"), [&state](const string &cmp) {
			if (cmp == "plain_advance_state") state.type = 0;
			else if (cmp == "no_clue") state.type = 0;
			else if (cmp == "no_clue2") state.type = 0;
			else if (cmp == "hit_from_left") state.type = 2;
			else if (cmp == "hit_from_right") state.type = 3;
			else if (cmp == "hit_by_skill") state.type = 5;
			else if (cmp == "hit_by_item") state.type = 100;
		});

		bool found = false;
		for (auto &reactor : m_reactor_info) {
			if (reactor.id == id) {
				found = true;
				reactor.states[state_id].push_back(state);
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto reactor::load_trigger_skills() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::reactor_event_trigger_skills));

	for (const auto &row : rs) {
		game_reactor_id id = row.get<game_reactor_id>("reactorid");
		int8_t state = row.get<int8_t>("state");
		game_skill_id skill_id = row.get<game_skill_id>("skillid");

		bool found = false;
		for (auto &reactor : m_reactor_info) {
			if (reactor.id == id) {
				found = true;
				for (size_t j = 0; j < reactor.states[state].size(); ++j) {
					reactor.states[state][j].trigger_skills.push_back(skill_id);
				}
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto reactor::get_reactor_data(game_reactor_id reactor_id, bool respect_link) const -> const data::type::reactor_info & {
	for (const auto &reactor : m_reactor_info) {
		if (reactor.id == reactor_id) {
			if (!respect_link || reactor.link == 0) {
				return reactor;
			}

			for (const auto &link : m_reactor_info) {
				if (link.id == reactor.link) {
					return link;
				}
			}
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

}
}
}