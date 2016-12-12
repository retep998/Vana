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
#include "script.hpp"
#include "common/abstract_server.hpp"
#include "common/algorithm.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/file.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace vana {
namespace data {
namespace provider {

auto script::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Scripts... ";

	m_npc_scripts.clear();
	m_reactor_scripts.clear();
	m_quest_scripts.clear();
	m_map_entry_scripts.clear();
	m_first_map_entry_scripts.clear();
	m_item_scripts.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::scripts));

	for (const auto &row : rs) {
		int32_t object_id = row.get<int32_t>("objectid");
		string script = row.get<string>("script");
		int8_t modifier = row.get<int8_t>("helper");

		vana::util::str::run_enum(row.get<string>("script_type"), [&](const string &cmp) {
			if (cmp == "npc") m_npc_scripts.push_back(std::make_pair(object_id, script));
			else if (cmp == "reactor") m_reactor_scripts.push_back(std::make_pair(object_id, script));
			else if (cmp == "map_enter") m_map_entry_scripts.push_back(std::make_pair(object_id, script));
			else if (cmp == "map_first_enter") m_first_map_entry_scripts.push_back(std::make_pair(object_id, script));
			else if (cmp == "item") m_item_scripts.push_back(std::make_pair(object_id, script));
			else if (cmp == "quest") {
				vector<pair<int8_t, string>> value;
				value.push_back(std::make_pair(modifier, script));
				m_quest_scripts.push_back(std::make_pair(static_cast<game_quest_id>(object_id), value));
			}
		});
	}

	std::cout << "DONE" << std::endl;
}

auto script::get_script(abstract_server *server, int32_t object_id, data::type::script_type type) const -> string {
	if (has_script(object_id, type)) {
		auto &scripts = resolve(type);
		for (const auto &script : scripts) {
			if (script.first == object_id) {
				string s = build_script_path(type, script.second);
				if (vana::util::file::exists(s)) {
					return s;
				}
#ifdef DEBUG
				server->log(vana::log::type::debug_error, "Missing script '" + s + "'");
#endif
				break;
			}
		}
	}
	return build_script_path(type, std::to_string(object_id));
}

auto script::get_quest_script(abstract_server *server, game_quest_id quest_id, int8_t state) const -> string {
	if (has_quest_script(quest_id, state)) {
		for (const auto &script : m_quest_scripts) {
			if (script.first != quest_id) {
				continue;
			}

			for (const auto &script_state : script.second) {
				if (script_state.first != state) {
					continue;
				}

				string s = build_script_path(data::type::script_type::quest, script_state.second);
				if (vana::util::file::exists(s)) {
					return s;
				}
#ifdef DEBUG
				server->log(vana::log::type::debug_error, "Missing quest script '" + s + "'");
#endif
				break;
			}

			break;
		}
	}

	return build_script_path(data::type::script_type::quest, std::to_string(quest_id) + (state == 0 ? "s" : "e"));
}

auto script::build_script_path(data::type::script_type type, const string &location) const -> string {
	string s = "scripts/" + resolve_path(type) + "/" + location + ".lua";
	return s;
}

auto script::has_script(int32_t object_id, data::type::script_type type) const -> bool {
	return ext::any_of(resolve(type), [object_id](pair<int32_t, string> value) -> bool {
		return value.first == object_id;
	});

}

auto script::has_quest_script(game_quest_id quest_id, int8_t state) const -> bool {
	return ext::any_of(m_quest_scripts, [quest_id](pair<game_quest_id, vector<pair<int8_t, string>>> value) -> bool {
		return value.first == quest_id;
	});
}

auto script::register_npc_script(game_npc_id npc_id, const string &script) -> void {
	int32_t index = 0;
	for (auto &script : m_npc_scripts) {
		if (script.first == npc_id) {
			m_npc_scripts.erase(m_npc_scripts.begin() + index);
			break;
		}
		index++;
	}

	m_npc_scripts.push_back({npc_id, script});
}

auto script::resolve(data::type::script_type type) const -> const vector<pair<int32_t, string>> & {
	switch (type) {
		case data::type::script_type::item: return m_item_scripts;
		case data::type::script_type::map_entry: return m_map_entry_scripts;
		case data::type::script_type::first_map_entry: return m_first_map_entry_scripts;
		case data::type::script_type::npc: return m_npc_scripts;
		case data::type::script_type::reactor: return m_reactor_scripts;
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "script_type");
}

auto script::resolve_path(data::type::script_type type) const -> string {
	switch (type) {
		case data::type::script_type::item: return "items";
		case data::type::script_type::map_entry: return "map_entry";
		case data::type::script_type::first_map_entry: return "first_map_entry";
		case data::type::script_type::npc: return "npcs";
		case data::type::script_type::reactor: return "reactors";
		case data::type::script_type::quest: return "quests";
		case data::type::script_type::instance: return "instances";
		case data::type::script_type::portal: return "portals";
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "script_type");
}

}
}
}