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
#include "Common/GameLogicUtilities.hpp"
#include "ChannelServer/Drop.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Mob.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerPacket.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/Quests.hpp"
#include "ChannelServer/ReactorPacket.hpp"
#include "ChannelServer/Reactor.hpp"
#include <string>

namespace Vana {
namespace ChannelServer {

LuaReactor::LuaReactor(const string_t &filename, player_id_t playerId, reactor_id_t reactorId, map_id_t mapId) :
	LuaScriptable{filename, playerId},
	m_reactorId{reactorId}
{
	set<reactor_id_t>("system_reactor_id", reactorId);
	set<map_id_t>("system_map_id", mapId);

	// Reactor
	expose("getState", &LuaExports::getState);
	expose("reset", &LuaExports::reset);
	expose("setState", &LuaExports::setStateReactor);

	// Miscellaneous
	expose("dropItem", &LuaExports::dropItemReactor);
	expose("getDistanceToPlayer", &LuaExports::getDistanceReactor);

	// Mob
	expose("spawnMob", &LuaExports::spawnMobReactor);
	expose("spawnZakum", &LuaExports::spawnZakum);

	run();
}

auto LuaExports::getReactor(lua_State *luaVm, LuaEnvironment &env) -> Reactor * {
	reactor_id_t reactorId = env.get<reactor_id_t>(luaVm, "system_reactor_id");
	map_id_t mapId = env.get<map_id_t>(luaVm, "system_map_id");
	return Maps::getMap(mapId)->getReactor(reactorId);
}

// Reactor
auto LuaExports::getState(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int8_t>(luaVm, getReactor(luaVm, env)->getState());
	return 1;
}

auto LuaExports::reset(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	Reactor *reactor = getReactor(luaVm, env);
	reactor->revive();
	reactor->setState(0, true);
	reactor->getMap()->send(Packets::triggerReactor(reactor));
	return 0;
}

auto LuaExports::setStateReactor(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getReactor(luaVm, env)->setState(env.get<int8_t>(luaVm, 1), true);
	return 0;
}

// Miscellaneous
auto LuaExports::dropItemReactor(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	item_id_t itemId = env.get<item_id_t>(luaVm, 1);
	slot_qty_t amount = 1;
	if (env.is(luaVm, 2, LuaType::Number)) {
		amount = env.get<slot_qty_t>(luaVm, 2);
	}
	Reactor *reactor = getReactor(luaVm, env);
	auto player = getPlayer(luaVm, env);
	Drop *drop;
	if (GameLogicUtilities::isEquip(itemId)) {
		Item f(itemId, true);
		drop = new Drop(reactor->getMapId(), f, reactor->getPos(), player != nullptr ? player->getId() : 0);
	}
	else {
		Item f(itemId, amount);
		drop = new Drop(reactor->getMapId(), f, reactor->getPos(), player != nullptr ? player->getId() : 0);
	}
	drop->setTime(player != nullptr ? 100 : 0); // FFA if player isn't around
	drop->doDrop(reactor->getPos());
	return 0;
}

auto LuaExports::getDistanceReactor(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, getPlayer(luaVm, env)->getPos() - getReactor(luaVm, env)->getPos());
	return 1;
}

// Mob
auto LuaExports::spawnMobReactor(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	mob_id_t mobId = env.get<mob_id_t>(luaVm, 1);
	Reactor *reactor = getReactor(luaVm, env);
	env.push<map_object_t>(luaVm, reactor->getMap()->spawnMob(mobId, reactor->getPos())->getMapMobId());
	return 1;
}

auto LuaExports::spawnZakum(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	coord_t x = env.get<coord_t>(luaVm, 1);
	coord_t y = env.get<coord_t>(luaVm, 2);
	foothold_id_t foothold = 0;
	if (env.is(luaVm, 3, LuaType::Number)) {
		foothold = env.get<foothold_id_t>(luaVm, 3);
	}
	Maps::getMap(getReactor(luaVm, env)->getMapId())->spawnZakum(Point{x, y}, foothold);
	return 0;
}

}
}