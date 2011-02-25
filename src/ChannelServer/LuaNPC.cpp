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
#include "LuaNpc.h"
#include "Npc.h"
#include "Player.h"
#include "Players.h"
#include "Quests.h"
#include "ScriptDataProvider.h"
#include "StoragePacket.h"
#include <vector>

using std::vector;

LuaNPC::LuaNPC(const string &filename, int32_t playerid) : LuaScriptable(filename, playerid) {
	luaThread = lua_newthread(luaVm);

	// Miscellaneous
	lua_register(luaVm, "showStorage", &LuaExports::showStorage);
	lua_register(luaVm, "getDistanceToPlayer", &LuaExports::getDistanceNpc);
	lua_register(luaVm, "getNPCID", &LuaExports::getNpcId);
	lua_register(luaVm, "runNPC", &LuaExports::npcRunNpc);

	// NPC interaction
	lua_register(luaVm, "addText", &LuaExports::addText);
	lua_register(luaVm, "sendBackNext", &LuaExports::sendBackNext);
	lua_register(luaVm, "sendBackOK", &LuaExports::sendBackOK);
	lua_register(luaVm, "sendNext", &LuaExports::sendNext);
	lua_register(luaVm, "sendOK", &LuaExports::sendOK);
	lua_register(luaVm, "askAcceptDecline", &LuaExports::askAcceptDecline);
	lua_register(luaVm, "askChoice", &LuaExports::askChoice);
	lua_register(luaVm, "askNumber", &LuaExports::askNumber);
	lua_register(luaVm, "askStyle", &LuaExports::askStyle);
	lua_register(luaVm, "askText", &LuaExports::askText);
	lua_register(luaVm, "askYesNo", &LuaExports::askYesNo);

	// Quest
	lua_register(luaVm, "addQuest", &LuaExports::addQuest);
	lua_register(luaVm, "endQuest", &LuaExports::endQuest);
}

bool LuaNPC::run() {
	if (luaL_loadfile(luaThread, filename.c_str())) {
		// Error in lua script
		handleError();
		return false;
	}
	return resume(0); // Start running the script
}

bool LuaNPC::resume(int32_t nargs) {
	int32_t ret = lua_resume(luaThread, nargs);
	if (ret == 0) { // NPC finished
		Players::Instance()->getPlayer(playerid)->getNPC()->end();
	}
	else if (ret != LUA_YIELD) { // error, a working NPC returns either 0 or LUA_YIELD
		handleError();
		return false;
	}
	return true;
}

bool LuaNPC::proceedNext() {
	return resume(0);
}

bool LuaNPC::proceedSelection(uint8_t selected) {
	lua_pushinteger(luaThread, selected);
	return resume(1);
}

bool LuaNPC::proceedNumber(int32_t number) {
	lua_pushinteger(luaThread, number);
	return resume(1);
}

bool LuaNPC::proceedText(const string &text) {
	lua_pushstring(luaThread, text.c_str());
	return resume(1);
}

void LuaNPC::handleError() {
	printError(lua_tostring(luaThread, -1));
	Players::Instance()->getPlayer(playerid)->getNPC()->end();
}

NPC * LuaExports::getNPC(lua_State *luaVm) {
	return getPlayer(luaVm)->getNPC();
}

// Miscellaneous
int LuaExports::showStorage(lua_State *luaVm) {
	StoragePacket::showStorage(getPlayer(luaVm), getNPC(luaVm)->getNpcId());
	return 0;
}

int LuaExports::getDistanceNpc(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getPos() - getNPC(luaVm)->getPos());
	return 1;
}

int LuaExports::getNpcId(lua_State *luaVm) {
	lua_pushinteger(luaVm, getNPC(luaVm)->getNpcId());
	return 1;
}

int LuaExports::npcRunNpc(lua_State *luaVm) {
	int32_t npcid = lua_tointeger(luaVm, 1);
	string script;
	if (lua_type(luaVm, 2) == LUA_TSTRING) { // We already have our script name
		string specified = lua_tostring(luaVm, 2);
		script = "scripts/npcs/" + specified + ".lua";
	}
	else {
		script = ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	getNPC(luaVm)->setEndScript(npcid, script);
	return 0;
}

// NPC interaction
int LuaExports::addText(lua_State *luaVm) {
	getNPC(luaVm)->addText(lua_tostring(luaVm, -1));
	return 0;
}

int LuaExports::sendBackNext(lua_State *luaVm) {
	getNPC(luaVm)->sendDialog(true, true);
	return lua_yield(luaVm, 0);
}

int LuaExports::sendBackOK(lua_State *luaVm) {
	getNPC(luaVm)->sendDialog(true, false);
	return lua_yield(luaVm, 0);
}

int LuaExports::sendNext(lua_State *luaVm) {
	getNPC(luaVm)->sendDialog(false, true);
	return lua_yield(luaVm, 0);
}

int LuaExports::sendOK(lua_State *luaVm) {
	getNPC(luaVm)->sendDialog(false, false);
	return lua_yield(luaVm, 0);
}

int LuaExports::askAcceptDecline(lua_State *luaVm) {
	getNPC(luaVm)->sendAcceptDecline();
	return lua_yield(luaVm, 1);
}

int LuaExports::askChoice(lua_State *luaVm) {
	getNPC(luaVm)->sendSimple();
	return lua_yield(luaVm, 1);
}

int LuaExports::askNumber(lua_State *luaVm) {
	getNPC(luaVm)->sendGetNumber(lua_tointeger(luaVm, -3), lua_tointeger(luaVm, -2), lua_tointeger(luaVm, -1));
	return lua_yield(luaVm, 1);
}

int LuaExports::askStyle(lua_State *luaVm) {
	vector<int32_t> styles;

	lua_pushnil(luaVm);
	while (lua_next(luaVm, 1) != 0) {
		styles.push_back(lua_tointeger(luaVm, -1));
		lua_pop(luaVm, 1);
	}
	if (styles.size() > 0)
		getNPC(luaVm)->sendStyle(&styles[0], styles.size());
	return lua_yield(luaVm, 1);
}

int LuaExports::askText(lua_State *luaVm) {
	int16_t min = 0;
	int16_t max = 0;
	if (lua_isnumber(luaVm, -2) && lua_isnumber(luaVm, -1)) {
		min = lua_tointeger(luaVm, -2);
		max = lua_tointeger(luaVm, -1);
	}
	getNPC(luaVm)->sendGetText(min, max);
	return lua_yield(luaVm, 1);
}

int LuaExports::askYesNo(lua_State *luaVm) {
	getNPC(luaVm)->sendYesNo();
	return lua_yield(luaVm, 1);
}

// Quest
int LuaExports::addQuest(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->addQuest(questid, getNPC(luaVm)->getNpcId());
	return 0;
}

int LuaExports::endQuest(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->finishQuest(questid, getNPC(luaVm)->getNpcId());
	return 0;
}
