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

#include "channel_server/lua/lua_scriptable.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class reactor;

		namespace lua {
			class lua_reactor : lua_scriptable {
				NONCOPYABLE(lua_reactor);
				NO_DEFAULT_CONSTRUCTOR(lua_reactor);
			public:
				lua_reactor(const string &filename, game_player_id player_id, game_reactor_id reactor_id, game_map_id map_id);
			private:
				game_reactor_id m_reactor_id = -1;
				game_map_id m_map_id = -1;
			};

			namespace lua_exports {
				auto get_reactor(lua_State *lua_vm, ::vana::lua::lua_environment &env) -> reactor *;

				// Reactor exports

				// Reactor
				auto get_state(lua_State *lua_vm) -> lua_return;
				auto reset(lua_State *lua_vm) -> lua_return;
				auto set_state_reactor(lua_State *lua_vm) -> lua_return;

				// Miscellaneous
				auto drop_item_reactor(lua_State *lua_vm) -> lua_return;
				auto get_distance_reactor(lua_State *lua_vm) -> lua_return;

				// Mob
				auto spawn_mob_reactor(lua_State *lua_vm) -> lua_return;
				auto spawn_zakum(lua_State *lua_vm) -> lua_return;
			}
		}
	}
}