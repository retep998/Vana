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
#include "LuaPortal.hpp"
#include "EffectPacket.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "Player.hpp"

hash_map_t<int32_t, PortalInfo *> LuaExports::portals;

LuaPortal::LuaPortal(const string_t &filename, int32_t playerId, PortalInfo *portal) :
	LuaScriptable(filename, playerId)
{
	LuaExports::portals[playerId] = portal;

	// Portal
	lua_register(luaVm, "getPortalName", &LuaExports::getPortalName);
	lua_register(luaVm, "instantWarp", &LuaExports::instantWarp);
	lua_register(luaVm, "playPortalSe", &LuaExports::playPortalSe);

	run();
}

auto LuaExports::getPortal(lua_State *luaVm) -> PortalInfo * {
	return portals[getPlayer(luaVm)->getId()];
}

// Portal
auto LuaExports::getPortalName(lua_State *luaVm) -> int {
	lua_pushstring(luaVm, getPortal(luaVm)->name.c_str());
	return 1;
}

auto LuaExports::instantWarp(lua_State *luaVm) -> int {
	Player *player = getPlayer(luaVm);
	string_t portal = lua_tostring(luaVm, 1);
	int8_t portalId = player->getMap()->getPortal(portal)->id;
	MapPacket::instantWarp(player, portalId);
	return 0;
}

auto LuaExports::playPortalSe(lua_State *luaVm) -> int {
	EffectPacket::playPortalSoundEffect(getPlayer(luaVm));
	return 0;
}