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

#include "common/types.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/lua/lua_scriptable.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		namespace lua {
			class lua_instance : public lua_scriptable {
				NONCOPYABLE(lua_instance);
				NO_DEFAULT_CONSTRUCTOR(lua_instance);
			public:
				lua_instance(const string &name, game_player_id player_id);
			};

			namespace lua_exports {
				auto create_instance_instance(lua_State *lua_vm) -> lua_return;
			}
		}
	}
}