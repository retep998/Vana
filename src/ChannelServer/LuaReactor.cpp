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
#include "LuaReactor.h"
#include "Drop.h"
#include "GameLogicUtilities.h"
#include "Maps.h"
#include "Mob.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerPacket.h"
#include "Quests.h"
#include "ReactorPacket.h"
#include "Reactor.h"
#include <string>

LuaReactor::LuaReactor(const string &filename, int32_t playerid, int32_t reactorid, int32_t mapid) :
LuaScriptable(filename, playerid),
reactorid(reactorid)
{
	setVariable("_reactorid", reactorid);
	setVariable("_mapid", mapid);

	// Reactor
	lua_register(luaVm, "getState", &LuaExports::getState);
	lua_register(luaVm, "reset", &LuaExports::reset);
	lua_register(luaVm, "setState", &LuaExports::setStateReactor);

	// Miscellaneous
	lua_register(luaVm, "dropItem", &LuaExports::dropItemReactor);
	lua_register(luaVm, "getDistanceToPlayer", &LuaExports::getDistanceReactor);

	// Mob
	lua_register(luaVm, "spawnMob", &LuaExports::spawnMobReactor);
	lua_register(luaVm, "spawnZakum", &LuaExports::spawnZakum);

	run();
}

Reactor * LuaExports::getReactor(lua_State *luaVm) {
	lua_getglobal(luaVm, "_reactorid");
	lua_getglobal(luaVm, "_mapid");
	int32_t reactorid = lua_tointeger(luaVm, -2);
	int32_t mapid = lua_tointeger(luaVm, -1);
	return Maps::getMap(mapid)->getReactor(reactorid);
}

// Reactor
int LuaExports::getState(lua_State *luaVm) {
	lua_pushinteger(luaVm, getReactor(luaVm)->getState());
	return 1;
}

int LuaExports::reset(lua_State *luaVm) {
	getReactor(luaVm)->revive();
	getReactor(luaVm)->setState(0, true);
	ReactorPacket::triggerReactor(getReactor(luaVm));
	return 0;
}

int LuaExports::setStateReactor(lua_State *luaVm) {
	getReactor(luaVm)->setState(lua_tointeger(luaVm, -1), true);
	return 0;
}

// Miscellaneous
int LuaExports::dropItemReactor(lua_State *luaVm) {
	int32_t itemid = lua_tointeger(luaVm, 1);
	int16_t amount = 1;
	if (lua_isnumber(luaVm, 2)) {
		amount = lua_tointeger(luaVm, 2);
	}
	Reactor *reactor = getReactor(luaVm);
	Player *player = getPlayer(luaVm);
	Drop *drop;
	if (GameLogicUtilities::isEquip(itemid)) {
		Item f(itemid, true);
		drop = new Drop(reactor->getMapId(), f, reactor->getPos(), player != nullptr ? player->getId() : 0);
	}
	else {
		Item f(itemid, amount);
		drop = new Drop(reactor->getMapId(), f, reactor->getPos(), player != nullptr ? player->getId() : 0);
	}
	drop->setTime(player != nullptr ? 100 : 0); // FFA if player isn't around
	drop->doDrop(reactor->getPos());
	return 0;
}

int LuaExports::getDistanceReactor(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getPos() - getReactor(luaVm)->getPos());
	return 1;
}

// Mob
int LuaExports::spawnMobReactor(lua_State *luaVm) {
	int32_t mobid = lua_tointeger(luaVm, -1);
	Reactor *reactor = getReactor(luaVm);
	lua_pushinteger(luaVm, Maps::getMap(reactor->getMapId())->spawnMob(mobid, reactor->getPos()));
	return 1;
}

int LuaExports::spawnZakum(lua_State *luaVm) {
	int16_t x = lua_tointeger(luaVm, 1);
	int16_t y = lua_tointeger(luaVm, 2);
	int16_t fh = 0;
	if (lua_isnumber(luaVm, 3)) {
		fh = lua_tointeger(luaVm, 3);
	}
	Maps::getMap(getReactor(luaVm)->getMapId())->spawnZakum(Pos(x, y), fh);
	return 0;
}