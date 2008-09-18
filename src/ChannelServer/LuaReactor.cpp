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

#include "LuaReactor.h"
#include "Reactors.h"
#include "ReactorPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "Quests.h"
#include "Maps.h"
#include "Mobs.h"
#include <string>

LuaReactor::LuaReactor(const string &filename, int32_t playerid, int32_t reactorid, int32_t mapid) : LuaScriptable(filename, playerid), reactorid(reactorid) {
	lua_pushinteger(luaVm, reactorid);
	lua_setglobal(luaVm, "reactorid");
	lua_pushinteger(luaVm, mapid);
	lua_setglobal(luaVm, "mapid");

	lua_register(luaVm, "setState", &LuaExports::setReactorState);
	lua_register(luaVm, "spawnMob", &LuaExports::spawnMobReactor);
	lua_register(luaVm, "reset", &LuaExports::reset);

	run();
}

Reactor * LuaExports::getReactor(lua_State *luaVm) {
	lua_getglobal(luaVm, "reactorid");
	lua_getglobal(luaVm, "mapid");
	int32_t reactorid = (lua_tointeger(luaVm, -2) - 200);
	int32_t mapid = lua_tointeger(luaVm, -1);
	return Maps::maps[mapid]->getReactor(reactorid);
}

int LuaExports::setReactorState(lua_State *luaVm) {
	getReactor(luaVm)->setState(lua_tointeger(luaVm, -1), true);
	return 1;
}

int LuaExports::spawnMobReactor(lua_State *luaVm) {
	int32_t mobid = lua_tointeger(luaVm, -1);
	Reactor *reactor = getReactor(luaVm);
	Mobs::spawnMobPos(reactor->getMapID(), mobid, reactor->getPos());
	return 1;
}

int LuaExports::reset(lua_State *luaVm) {
	getReactor(luaVm)->revive();
	getReactor(luaVm)->setState(0, true);
	ReactorPacket::triggerReactor(getReactor(luaVm));
	return 1;
}
