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
#pragma once

#include "common/script_types.hpp"
#include "common/types_temp.hpp"
#include <string>
#include <unordered_map>

namespace vana {
	class abstract_server;

	class script_data_provider {
	public:
		auto load_data() -> void;

		auto get_quest_script(abstract_server *server, game_quest_id quest_id, int8_t state) const -> string;
		auto get_script(abstract_server *server, int32_t object_id, script_types type) const -> string;
		auto has_quest_script(game_quest_id quest_id, int8_t state) const -> bool;
		auto has_script(int32_t object_id, script_types type) const -> bool;
		auto build_script_path(script_types type, const string &location) const -> string;
	private:
		auto resolve(script_types type) const -> const hash_map<int32_t, string> &;
		auto resolve_path(script_types type) const -> string;

		hash_map<game_npc_id, string> m_npc_scripts;
		hash_map<game_reactor_id, string> m_reactor_scripts;
		hash_map<game_map_id, string> m_map_entry_scripts;
		hash_map<game_map_id, string> m_first_map_entry_scripts;
		hash_map<game_item_id, string> m_item_scripts;
		hash_map<game_quest_id, hash_map<int8_t, string>> m_quest_scripts;
	};
}