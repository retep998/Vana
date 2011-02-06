/*
Copyright (C) 2008-2011 Vana Development Team

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

#include "LuaScriptable.h"
#include <string>

using std::string;

class Reactor;

class LuaReactor : LuaScriptable {
public:
	LuaReactor(const string &filename, int32_t playerid, int32_t reactorid, int32_t mapid);
private:
	int32_t reactorid;
	int32_t mapid;
	Reactor *reactor;
};

namespace LuaExports {
	Reactor * getReactor(lua_State *luaVm);

	// Reactor exports

	// Reactor
	int getState(lua_State *luaVm);
	int reset(lua_State *luaVm);
	int setStateReactor(lua_State *luaVm);

	// Miscellaneous
	int dropItemReactor(lua_State *luaVm);
	int getDistanceReactor(lua_State *luaVm);

	// Mob
	int spawnMobReactor(lua_State *luaVm);
	int spawnZakum(lua_State *luaVm);
};
