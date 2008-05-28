/*
Copyright (C) 2008 Vana Development Team

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
#include "Reactors.h"
#include "ReactorPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "Quests.h"
#include "Maps.h"
#include "Mobs.h"
#include <string>

LuaReactor::LuaReactor(const string &filename, int playerid, int reactorid, int mapid) : filename(filename), playerid(playerid), reactorid(reactorid), mapid(mapid), luaVm(lua_open()) {
	initialize();
	run();
}

LuaReactor::~LuaReactor() {
	lua_close(luaVm);
}

void LuaReactor::initialize() {
	lua_pushinteger(luaVm, playerid); // Pushing id for reference from static functions
	lua_setglobal(luaVm, "playerid");
	lua_pushinteger(luaVm, reactorid);
	lua_setglobal(luaVm, "reactorid");
	lua_pushinteger(luaVm, mapid);
	lua_setglobal(luaVm, "mapid");

	lua_register(luaVm, "getMap", &LuaReactorExports::getMap);
	lua_register(luaVm, "setMusic", &LuaReactorExports::setMusic);
	lua_register(luaVm, "setMap", &LuaReactorExports::setMap);
	lua_register(luaVm, "spawnMob", &LuaReactorExports::spawnMob);
	lua_register(luaVm, "spawnMobPos", &LuaReactorExports::spawnMobPos);
	lua_register(luaVm, "mapMessage", &LuaReactorExports::mapMessage);

	lua_register(luaVm, "reset", &LuaReactorExports::reset);
}

void LuaReactor::run() {
	luaL_dofile(luaVm, filename.c_str());
}

Reactor * LuaReactorExports::getReactor(lua_State *luaVm) {
	lua_getglobal(luaVm, "reactorid");
	lua_getglobal(luaVm, "mapid");
	return Reactors::getReactorByID(lua_tointeger(luaVm, -2), lua_tointeger(luaVm, -1));
}

Player * LuaReactorExports::getPlayer(lua_State *luaVm) {
	lua_getglobal(luaVm, "playerid");
	return Players::players[lua_tointeger(luaVm, -1)];
}

int LuaReactorExports::getMap(lua_State *luaVm) {
	lua_pushnumber(luaVm, getReactor(luaVm)->getMapID());
	return 1;
}

int LuaReactorExports::setState(lua_State *luaVm) {
	getReactor(luaVm)->setState(lua_tointeger(luaVm, -1));
	return 1;
}

int LuaReactorExports::setMusic(lua_State *luaVm) {
	Maps::changeMusic(getReactor(luaVm)->getMapID(), lua_tostring(luaVm, -1));
	return 1;
}

int LuaReactorExports::setMap(lua_State *luaVm) {
	int mapid = lua_tointeger(luaVm, -1);
	if (Maps::info.find(mapid) != Maps::info.end())
		Maps::changeMap(getPlayer(luaVm), mapid, 0);
	return 1;
}

int LuaReactorExports::spawnMob(lua_State *luaVm) {
	int mobid = lua_tointeger(luaVm, -1);
	Reactor *reactor = getReactor(luaVm);
	Mobs::spawnMobPos(getPlayer(luaVm), mobid, reactor->getPos().x, reactor->getPos().y-10);
	return 1;
}

int LuaReactorExports::spawnMobPos(lua_State *luaVm) {
	int mobid = lua_tointeger(luaVm, -3);
	short x = lua_tointeger(luaVm, -2);
	short y = lua_tointeger(luaVm, -1);
	Mobs::spawnMobPos(getPlayer(luaVm), mobid, x, y);
	return 1;
}

int LuaReactorExports::mapMessage(lua_State *luaVm) {
	int type = lua_tointeger(luaVm, -1);
	for (unsigned int i=0; i<Maps::info[getReactor(luaVm)->getMapID()].Players.size(); i++) {
		PlayerPacket::showMessage(Maps::info[getReactor(luaVm)->getMapID()].Players[i], lua_tostring(luaVm, -2), type);
	}
	return 1;
}

int LuaReactorExports::reset(lua_State *luaVm) {
	getReactor(luaVm)->revive();
	getReactor(luaVm)->setState(0);
	ReactorPacket::showReactor(getPlayer(luaVm), getReactor(luaVm));
	return 1;
}
