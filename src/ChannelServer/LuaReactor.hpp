/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "LuaScriptable.hpp"
#include <string>

class Reactor;

class LuaReactor : LuaScriptable {
	NONCOPYABLE(LuaReactor);
	NO_DEFAULT_CONSTRUCTOR(LuaReactor);
public:
	LuaReactor(const string_t &filename, player_id_t playerId, reactor_id_t reactorId, map_id_t mapId);
private:
	reactor_id_t m_reactorId = -1;
	map_id_t m_mapId = -1;
};

namespace LuaExports {
	auto getReactor(lua_State *luaVm) -> Reactor *;

	// Reactor exports

	// Reactor
	auto getState(lua_State *luaVm) -> int;
	auto reset(lua_State *luaVm) -> int;
	auto setStateReactor(lua_State *luaVm) -> int;

	// Miscellaneous
	auto dropItemReactor(lua_State *luaVm) -> int;
	auto getDistanceReactor(lua_State *luaVm) -> int;

	// Mob
	auto spawnMobReactor(lua_State *luaVm) -> int;
	auto spawnZakum(lua_State *luaVm) -> int;
}