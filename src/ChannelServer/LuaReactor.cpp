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
#include "LuaReactor.hpp"
#include "Drop.hpp"
#include "GameLogicUtilities.hpp"
#include "Maps.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayerDataProvider.hpp"
#include "Quests.hpp"
#include "ReactorPacket.hpp"
#include "Reactor.hpp"
#include <string>

LuaReactor::LuaReactor(const string_t &filename, player_id_t playerId, reactor_id_t reactorId, map_id_t mapId) :
	LuaScriptable(filename, playerId),
	m_reactorId(reactorId)
{
	set<reactor_id_t>("system_reactorId", reactorId);
	set<map_id_t>("system_mapId", mapId);

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

auto LuaExports::getReactor(lua_State *luaVm) -> Reactor * {
	lua_getglobal(luaVm, "system_reactorId");
	lua_getglobal(luaVm, "system_mapId");
	reactor_id_t reactorId = lua_tointeger(luaVm, -2);
	map_id_t mapId = lua_tointeger(luaVm, -1);
	lua_pop(luaVm, 2);
	return Maps::getMap(mapId)->getReactor(reactorId);
}

// Reactor
auto LuaExports::getState(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getReactor(luaVm)->getState());
	return 1;
}

auto LuaExports::reset(lua_State *luaVm) -> int {
	Reactor *reactor = getReactor(luaVm);
	reactor->revive();
	reactor->setState(0, true);
	reactor->getMap()->send(ReactorPacket::triggerReactor(reactor));
	return 0;
}

auto LuaExports::setStateReactor(lua_State *luaVm) -> int {
	getReactor(luaVm)->setState(lua_tointeger(luaVm, -1), true);
	return 0;
}

// Miscellaneous
auto LuaExports::dropItemReactor(lua_State *luaVm) -> int {
	item_id_t itemId = lua_tointeger(luaVm, 1);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 2)) {
		amount = lua_tointeger(luaVm, 2);
	}
	Reactor *reactor = getReactor(luaVm);
	Player *player = getPlayer(luaVm);
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

auto LuaExports::getDistanceReactor(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getPos() - getReactor(luaVm)->getPos());
	return 1;
}

// Mob
auto LuaExports::spawnMobReactor(lua_State *luaVm) -> int {
	mob_id_t mobId = lua_tointeger(luaVm, -1);
	Reactor *reactor = getReactor(luaVm);
	lua_pushinteger(luaVm, Maps::getMap(reactor->getMapId())->spawnMob(mobId, reactor->getPos())->getMapMobId());
	return 1;
}

auto LuaExports::spawnZakum(lua_State *luaVm) -> int {
	int16_t x = lua_tointeger(luaVm, 1);
	int16_t y = lua_tointeger(luaVm, 2);
	foothold_id_t foothold = 0;
	if (lua_isnumber(luaVm, 3)) {
		foothold = lua_tointeger(luaVm, 3);
	}
	Maps::getMap(getReactor(luaVm)->getMapId())->spawnZakum(Pos(x, y), foothold);
	return 0;
}