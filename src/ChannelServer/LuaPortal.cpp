/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ChannelServer/EffectPacket.hpp"
#include "ChannelServer/MapPacket.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"

namespace Vana {
namespace ChannelServer {

LuaPortal::LuaPortal(const string_t &filename, player_id_t playerId, map_id_t mapId, const PortalInfo * const portal) :
	LuaScriptable{filename, playerId}
{
	set<portal_id_t>("system_portal_id", portal->id);
	set<string_t>("system_portal_name", portal->name);
	set<map_id_t>("system_map_id", mapId);

	// Portal
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

// Portal
auto LuaExports::instantWarp(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	Player *player = getPlayer(luaVm, env);
	string_t portal = env.get<string_t>(luaVm, 1);
	portal_id_t portalId = player->getMap()->getPortal(portal)->id;
	player->send(Packets::Map::instantWarp(portalId));
	env.set<bool>(luaVm, "player_warped", true);
	return 0;
}

auto LuaExports::playPortalSe(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getPlayer(luaVm, env)->send(Packets::playPortalSoundEffect());
	return 0;
}

auto LuaExports::portalFailed(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.set<bool>(luaVm, "player_portal_failed", true);
	return 0;
}

}
}