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
#include "LuaPortal.h"
#include "Player.h"
#include "Maps.h"

hash_map <int, PortalInfo *> LuaExports::portals;

LuaPortal::LuaPortal(const string &filename, int playerid, PortalInfo *portal) : LuaScriptable(filename, playerid), portal(portal) {
	LuaExports::portals[playerid] = portal;

	lua_register(luaVm, "getPortalFrom", &LuaExports::getPortalFrom);
	lua_register(luaVm, "setPortalTo", &LuaExports::setPortalTo);
	lua_register(luaVm, "setPortalToId", &LuaExports::setPortalToId);

	run();
}

PortalInfo * LuaExports::getPortal(lua_State *luaVm) {
	return portals[getPlayer(luaVm)->getPlayerid()];
}

int LuaExports::getPortalFrom(lua_State *luaVm) {
	lua_pushstring(luaVm, getPortal(luaVm)->from);
	return 1;
}

int LuaExports::setPortalTo(lua_State *luaVm) {
	strcpy_s(getPortal(luaVm)->to, lua_tostring(luaVm, -1));
	return 1;
}

int LuaExports::setPortalToId(lua_State *luaVm) {
	int toid = lua_tointeger(luaVm, -1);
	getPortal(luaVm)->toid = toid;
	return 1;
}
