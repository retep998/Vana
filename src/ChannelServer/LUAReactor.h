/*
Copyright (C) 2008 Vana Development Team

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
#ifndef LUAREACTOR_H
#define LUAREACTOR_H

extern "C" {
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}

#include <hash_map>
#include <string>

using stdext::hash_map;
using std::string;

class Reactor;
class Player;

class LuaReactor {
public:
	LuaReactor(const string &filename, int playerid, int reactorid, int mapid);
	~LuaReactor();

	void initialize();
	void run();
private:
	string filename;
	int playerid;
	int reactorid;
	int mapid;
	Reactor *reactor;
	lua_State *luaVm;
};

namespace LuaReactorExports {
	Reactor * getReactor(lua_State *luaVm);
	Player * getPlayer(lua_State *luaVm);

	// The exports
	int getMap(lua_State *luaVm);
	int setMusic(lua_State *luaVm);
	int setMap(lua_State *luaVm);
	int setState(lua_State *luaVm);
	int spawnMob(lua_State *luaVm);
	int spawnMobPos(lua_State *luvaVm);
	int killMob(lua_State *luaVm);
	int mapMessage(lua_State *luaVm);

	int reset(lua_State *luaVm);
};

#endif
