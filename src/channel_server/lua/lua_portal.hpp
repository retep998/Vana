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
#include <unordered_map>

namespace vana {
	namespace data {
		namespace type {
			struct portal_info;
		}
	}

	namespace channel_server {
		namespace lua {
			class lua_portal : public lua_scriptable {
				NONCOPYABLE(lua_portal);
				NO_DEFAULT_CONSTRUCTOR(lua_portal);
			public:
				lua_portal(const string &filename, game_player_id player_id, game_map_id map_id, const data::type::portal_info * const portal);
				auto player_warped() -> bool;
				auto player_map_changed() -> bool;
				auto portal_failed() -> bool;
			};

			namespace lua_exports {
				// Portal exports

				// Portal
				auto instant_warp(lua_State *lua_vm) -> lua_return;
				auto play_portal_se(lua_State *lua_vm) -> lua_return;
				auto portal_failed(lua_State *lua_vm) -> lua_return;
			}
		}
	}
}