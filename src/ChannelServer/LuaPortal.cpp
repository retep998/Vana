/*
Copyright (C) 2008-2009 Vana Development Team

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

unordered_map<int32_t, PortalInfo *> LuaExports::portals;

LuaPortal::LuaPortal(const string &filename, int32_t playerid, PortalInfo *portal) : LuaScriptable(filename, playerid), portal(portal) {
	LuaExports::portals[playerid] = portal;

	lua_register(luaVm, "getPortalName", &LuaExports::getPortalName);

	run();
}

PortalInfo * LuaExports::getPortal(lua_State *luaVm) {
	return portals[getPlayer(luaVm)->getId()];
}

int LuaExports::getPortalName(lua_State *luaVm) {
	lua_pushstring(luaVm, getPortal(luaVm)->name.c_str());
	return 1;
}
