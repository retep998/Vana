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

hash_map_t<player_id_t, PortalInfo *> LuaExports::portals;

LuaPortal::LuaPortal(const string_t &filename, player_id_t playerId, PortalInfo *portal) :
	LuaScriptable(filename, playerId)
{
	LuaExports::portals[playerId] = portal;

	// Portal
	expose("getPortalName", &LuaExports::getPortalName);
	expose("instantWarp", &LuaExports::instantWarp);
	expose("playPortalSe", &LuaExports::playPortalSe);
	expose("portalFailed", &LuaExports::portalFailed);

	run();
}

auto LuaPortal::playerWarped() -> bool {
	return exists("player_warped");
}

auto LuaPortal::playerMapChanged() -> bool {
	return exists("player_map_changed");
}

auto LuaPortal::portalFailed() -> bool {
	return exists("player_portal_failed");
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
	auto &env = getEnvironment(luaVm);
	Player *player = getPlayer(luaVm);
	string_t portal = lua_tostring(luaVm, 1);
	portal_id_t portalId = player->getMap()->getPortal(portal)->id;
	player->send(MapPacket::instantWarp(portalId));
	env.set<bool>(luaVm, "player_warped", true);
	return 0;
}

auto LuaExports::playPortalSe(lua_State *luaVm) -> int {
	getPlayer(luaVm)->send(EffectPacket::playPortalSoundEffect());
	return 0;
}

auto LuaExports::portalFailed(lua_State *luaVm) -> int {
	auto &env = getEnvironment(luaVm);
	env.set<bool>(luaVm, "player_portal_failed", true);
	return 0;
}