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
#include "LuaReactor.hpp"
#include "common/GameLogicUtilities.hpp"
#include "channel_server/Drop.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Mob.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerPacket.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/Quests.hpp"
#include "channel_server/ReactorPacket.hpp"
#include "channel_server/Reactor.hpp"
#include <string>

namespace vana {
namespace channel_server {

lua_reactor::lua_reactor(const string &filename, game_player_id player_id, game_reactor_id reactor_id, game_map_id map_id) :
	lua_scriptable{filename, player_id},
	m_reactor_id{reactor_id}
{
	set<game_reactor_id>("system_reactor_id", reactor_id);
	set<game_map_id>("system_map_id", map_id);

	// Reactor
	expose("getState", &lua_exports::get_state);
	expose("reset", &lua_exports::reset);
	expose("setState", &lua_exports::set_state_reactor);

	// Miscellaneous
	expose("dropItem", &lua_exports::drop_item_reactor);
	expose("getDistanceToPlayer", &lua_exports::get_distance_reactor);

	// Mob
	expose("spawnMob", &lua_exports::spawn_mob_reactor);
	expose("spawnZakum", &lua_exports::spawn_zakum);

	run();
}

auto lua_exports::get_reactor(lua_State *lua_vm, lua_environment &env) -> reactor * {
	game_reactor_id reactor_id = env.get<game_reactor_id>(lua_vm, "system_reactor_id");
	game_map_id map_id = env.get<game_map_id>(lua_vm, "system_map_id");
	return maps::get_map(map_id)->get_reactor(reactor_id);
}

// Reactor
auto lua_exports::get_state(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int8_t>(lua_vm, get_reactor(lua_vm, env)->get_state());
	return 1;
}

auto lua_exports::reset(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	reactor *reactor = get_reactor(lua_vm, env);
	reactor->revive();
	reactor->set_state(0, true);
	reactor->get_map()->send(packets::trigger_reactor(reactor));
	return 0;
}

auto lua_exports::set_state_reactor(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	get_reactor(lua_vm, env)->set_state(env.get<int8_t>(lua_vm, 1), true);
	return 0;
}

// Miscellaneous
auto lua_exports::drop_item_reactor(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_item_id item_id = env.get<game_item_id>(lua_vm, 1);
	game_slot_qty amount = 1;
	if (env.is(lua_vm, 2, lua::lua_type::number)) {
		amount = env.get<game_slot_qty>(lua_vm, 2);
	}
	reactor *reactor = get_reactor(lua_vm, env);
	auto player = get_player(lua_vm, env);
	drop *value;
	if (game_logic_utilities::is_equip(item_id)) {
		item f{item_id, true};
		value = new drop{reactor->get_map_id(), f, reactor->get_pos(), player != nullptr ? player->get_id() : 0};
	}
	else {
		item f{item_id, amount};
		value = new drop{reactor->get_map_id(), f, reactor->get_pos(), player != nullptr ? player->get_id() : 0};
	}
	value->set_time(player != nullptr ? 100 : 0); // FFA if player isn't around
	value->do_drop(reactor->get_pos());
	return 0;
}

auto lua_exports::get_distance_reactor(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	env.push<int32_t>(lua_vm, get_player(lua_vm, env)->get_pos() - get_reactor(lua_vm, env)->get_pos());
	return 1;
}

// Mob
auto lua_exports::spawn_mob_reactor(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_mob_id mob_id = env.get<game_mob_id>(lua_vm, 1);
	reactor *reactor = get_reactor(lua_vm, env);
	env.push<game_map_object>(lua_vm, reactor->get_map()->spawn_mob(mob_id, reactor->get_pos())->get_map_mob_id());
	return 1;
}

auto lua_exports::spawn_zakum(lua_State *lua_vm) -> lua::lua_return {
	auto &env = get_environment(lua_vm);
	game_coord x = env.get<game_coord>(lua_vm, 1);
	game_coord y = env.get<game_coord>(lua_vm, 2);
	game_foothold_id foothold = 0;
	if (env.is(lua_vm, 3, lua::lua_type::number)) {
		foothold = env.get<game_foothold_id>(lua_vm, 3);
	}
	maps::get_map(get_reactor(lua_vm, env)->get_map_id())->spawn_zakum(point{x, y}, foothold);
	return 0;
}

}
}