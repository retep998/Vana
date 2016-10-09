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

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

namespace vana {
	namespace lua {
		enum class lua_type : int {
			none = LUA_TNONE,
			nil = LUA_TNIL,
			boolean = LUA_TBOOLEAN,
			light_userdata = LUA_TLIGHTUSERDATA,
			number = LUA_TNUMBER,
			string = LUA_TSTRING,
			table = LUA_TTABLE,
			function = LUA_TFUNCTION,
			userdata = LUA_TUSERDATA,
			thread = LUA_TTHREAD,
		};

		using lua_return = int;
		using lua_function = lua_return (*)(lua_State *);
	}
}