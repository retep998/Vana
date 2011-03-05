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
#include "LuaPortal.h"
#include "EffectPacket.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Player.h"

unordered_map<int32_t, PortalInfo *> LuaExports::portals;

LuaPortal::LuaPortal(const string &filename, int32_t playerid, PortalInfo *portal) : LuaScriptable(filename, playerid), portal(portal) {
	LuaExports::portals[playerid] = portal;

	// Portal
	lua_register(luaVm, "getPortalName", &LuaExports::getPortalName);
	lua_register(luaVm, "instantWarp", &LuaExports::instantWarp);
	lua_register(luaVm, "playPortalSE", &LuaExports::playPortalSe);

	run();
}

PortalInfo * LuaExports::getPortal(lua_State *luaVm) {
	return portals[getPlayer(luaVm)->getId()];
}

// Portal
int LuaExports::getPortalName(lua_State *luaVm) {
	lua_pushstring(luaVm, getPortal(luaVm)->name.c_str());
	return 1;
}

int LuaExports::instantWarp(lua_State *luaVm) {
	Player *p = getPlayer(luaVm);
	string portal = lua_tostring(luaVm, 1);
	int8_t pid = Maps::getMap(p->getMap())->getPortal(portal)->id;
	MapPacket::instantWarp(p, pid);
	return 0;
}

int LuaExports::playPortalSe(lua_State *luaVm) {
	EffectPacket::playPortalSoundEffect(getPlayer(luaVm));
	return 0;
}