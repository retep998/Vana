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
#include "script_data_provider.hpp"
#include "common/abstract_server.hpp"
#include "common/algorithm.hpp"
#include "common/database.hpp"
#include "common/file_utilities.hpp"
#include "common/initialize_common.hpp"
#include "common/string_utilities.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace vana {

auto script_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Scripts... ";

	m_npc_scripts.clear();
	m_reactor_scripts.clear();
	m_quest_scripts.clear();
	m_map_entry_scripts.clear();
	m_first_map_entry_scripts.clear();
	m_item_scripts.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("scripts"));

	for (const auto &row : rs) {
		int32_t object_id = row.get<int32_t>("objectid");
		string script = row.get<string>("script");
		int8_t modifier = row.get<int8_t>("helper");

		utilities::str::run_enum(row.get<string>("script_type"), [&](const string &cmp) {
			if (cmp == "npc") m_npc_scripts[object_id] = script;
			else if (cmp == "reactor") m_reactor_scripts[object_id] = script;
			else if (cmp == "map_enter") m_map_entry_scripts[object_id] = script;
			else if (cmp == "map_first_enter") m_first_map_entry_scripts[object_id] = script;
			else if (cmp == "item") m_item_scripts[object_id] = script;
			else if (cmp == "quest") m_quest_scripts[static_cast<game_quest_id>(object_id)][modifier] = script;
		});
	}

	std::cout << "DONE" << std::endl;
}

auto script_data_provider::get_script(abstract_server *server, int32_t object_id, script_types type) const -> string {
	if (has_script(object_id, type)) {
		string s = build_script_path(type, resolve(type).find(object_id)->second);
		if (utilities::file::exists(s)) {
			return s;
		}
#ifdef DEBUG
		else server->log(log_type::debug_error, "Missing script '" + s + "'");
#endif
	}
	return build_script_path(type, std::to_string(object_id));
}

auto script_data_provider::get_quest_script(abstract_server *server, game_quest_id quest_id, int8_t state) const -> string {
	if (has_quest_script(quest_id, state)) {
		string s = build_script_path(script_types::quest, m_quest_scripts.find(quest_id)->second.find(state)->second);
		if (utilities::file::exists(s)) {
			return s;
		}
#ifdef DEBUG
		else server->log(log_type::debug_error, "Missing quest script '" + s + "'");
#endif
	}
	return build_script_path(script_types::quest, std::to_string(quest_id) + (state == 0 ? "s" : "e"));
}

auto script_data_provider::build_script_path(script_types type, const string &location) const -> string {
	string s = "scripts/" + resolve_path(type) + "/" + location + ".lua";
	return s;
}

auto script_data_provider::has_script(int32_t object_id, script_types type) const -> bool {
	return ext::is_element(resolve(type), object_id);
}

auto script_data_provider::has_quest_script(game_quest_id quest_id, int8_t state) const -> bool {
	return ext::is_element(m_quest_scripts, quest_id);
}

auto script_data_provider::resolve(script_types type) const -> const hash_map<int32_t, string> & {
	switch (type) {
		case script_types::item: return m_item_scripts;
		case script_types::map_entry: return m_map_entry_scripts;
		case script_types::first_map_entry: return m_first_map_entry_scripts;
		case script_types::npc: return m_npc_scripts;
		case script_types::reactor: return m_reactor_scripts;
	}
	throw not_implemented_exception{"script_types"};
}

auto script_data_provider::resolve_path(script_types type) const -> string {
	switch (type) {
		case script_types::item: return "items";
		case script_types::map_entry: return "map_entry";
		case script_types::first_map_entry: return "first_map_entry";
		case script_types::npc: return "npcs";
		case script_types::reactor: return "reactors";
		case script_types::quest: return "quests";
		case script_types::instance: return "instances";
		case script_types::portal: return "portals";
	}
	throw not_implemented_exception{"script_types"};
}

}