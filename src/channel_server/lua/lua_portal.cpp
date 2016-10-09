/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "lua_portal.hpp"
#include "channel_server/effect_packet.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {
namespace lua {

lua_portal::lua_portal(const string &filename, game_player_id player_id, game_map_id map_id, const data::type::portal_info * const portal) :
	lua_scriptable{filename, player_id}
{
	set<game_portal_id>("system_portal_id", portal->id);
	set<string>("system_portal_name", portal->name);
	set<game_map_id>("system_map_id", map_id);

	// Portal
	expose("instantWarp", &lua_exports::instant_warp);
	expose("playPortalSe", &lua_exports::play_portal_se);
	expose("portalFailed", &lua_exports::portal_failed);

	run();
}

auto lua_portal::player_warped() -> bool {
	return exists("player_warped");
}

auto lua_portal::player_map_changed() -> bool {
	return exists("player_map_changed");
}

auto lua_portal::portal_failed() -> bool {
	return exists("player_portal_failed");
}

// Portal
auto lua_exports::instant_warp(lua_State *lua_vm) -> lua_return {
	auto &env = get_environment(lua_vm);
	auto player = get_player(lua_vm, env);
	string portal = env.get<string>(lua_vm, 1);
	game_portal_id portal_id = player->get_map()->get_portal(portal)->id;
	player->send(packets::map::instant_warp(portal_id));
	env.set<bool>(lua_vm, "player_warped", true);
	return 0;
}

auto lua_exports::play_portal_se(lua_State *lua_vm) -> lua_return {
	auto &env = get_environment(lua_vm);
	get_player(lua_vm, env)->send(packets::play_portal_sound_effect());
	return 0;
}

auto lua_exports::portal_failed(lua_State *lua_vm) -> lua_return {
	auto &env = get_environment(lua_vm);
	env.set<bool>(lua_vm, "player_portal_failed", true);
	return 0;
}

}
}
}