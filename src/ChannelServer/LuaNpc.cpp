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
#include "NpcHandler.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerNpc.h"
#include "PlayerNpcDataProvider.h"
#include "Quests.h"
#include "ScriptDataProvider.h"
#include "StoragePacket.h"
#include <vector>

using std::vector;

LuaNpc::LuaNpc(const string &filename, int32_t playerid) : LuaScriptable(filename, playerid) {
	luaThread = lua_newthread(luaVm);

	// Miscellaneous
	lua_register(luaVm, "showStorage", &LuaExports::showStorage);
	lua_register(luaVm, "getDistanceToPlayer", &LuaExports::getDistanceNpc);
	lua_register(luaVm, "getNPCID", &LuaExports::getNpcId);
	lua_register(luaVm, "runNPC", &LuaExports::npcRunNpc);

	// Imitating NPC's (Player NPC's)
	lua_register(luaVm, "getPlayerNpcName", &LuaExports::getPlayerNpcName);
	lua_register(luaVm, "getPlayerNpcLevel", &LuaExports::getPlayerNpcLevel);
	lua_register(luaVm, "makeNewImitation", &LuaExports::makeNewImitation);

	// NPC interaction
	lua_register(luaVm, "addText", &LuaExports::addText);
	lua_register(luaVm, "sendBackNext", &LuaExports::sendBackNext);
	lua_register(luaVm, "sendBackOK", &LuaExports::sendBackOk);
	lua_register(luaVm, "sendNext", &LuaExports::sendNext);
	lua_register(luaVm, "sendOK", &LuaExports::sendOk);
	lua_register(luaVm, "askAcceptDecline", &LuaExports::askAcceptDecline);
	lua_register(luaVm, "askAcceptDeclineNoExit", &LuaExports::askAcceptDeclineNoExit);
	lua_register(luaVm, "askChoice", &LuaExports::askChoice);
	lua_register(luaVm, "askNumber", &LuaExports::askNumber);
	lua_register(luaVm, "askStyle", &LuaExports::askStyle);
	lua_register(luaVm, "askText", &LuaExports::askText);
	lua_register(luaVm, "askYesNo", &LuaExports::askYesNo);
	lua_register(luaVm, "askQuiz", &LuaExports::askQuiz);
	lua_register(luaVm, "askQuestion", &LuaExports::askQuestion);

	// Quest
	lua_register(luaVm, "addQuest", &LuaExports::addQuest);
	lua_register(luaVm, "endQuest", &LuaExports::endQuest);
}

bool LuaNpc::run() {
	if (luaL_loadfile(luaThread, filename.c_str())) {
		// Error in lua script
		handleError();
		return false;
	}
	return resume(0); // Start running the script
}

bool LuaNpc::resume(int32_t nargs) {
	int32_t ret = lua_resume(luaThread, nargs);
	if (ret == 0) { // NPC finished
		PlayerDataProvider::Instance()->getPlayer(playerid)->getNpc()->end();
	}
	else if (ret != LUA_YIELD) { // error, a working NPC returns either 0 or LUA_YIELD
		handleError();
		return false;
	}
	return true;
}

bool LuaNpc::proceedNext() {
	return resume(0);
}

bool LuaNpc::proceedSelection(uint8_t selected) {
	lua_pushinteger(luaThread, selected);
	return resume(1);
}

bool LuaNpc::proceedNumber(int32_t number) {
	lua_pushinteger(luaThread, number);
	return resume(1);
}

bool LuaNpc::proceedText(const string &text) {
	lua_pushstring(luaThread, text.c_str());
	return resume(1);
}

void LuaNpc::handleError() {
	printError(lua_tostring(luaThread, -1));
	PlayerDataProvider::Instance()->getPlayer(playerid)->getNpc()->end();
}

Npc * LuaExports::getNpc(lua_State *luaVm) {
	return getPlayer(luaVm)->getNpc();
}

// Miscellaneous
int LuaExports::showStorage(lua_State *luaVm) {
	NpcHandler::showStorage(getPlayer(luaVm), getNpc(luaVm)->getNpcId());
	return 0;
}

int LuaExports::getDistanceNpc(lua_State *luaVm) {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getPos() - getNpc(luaVm)->getPos());
	return 1;
}

int LuaExports::getNpcId(lua_State *luaVm) {
	lua_pushinteger(luaVm, getNpc(luaVm)->getNpcId());
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
	getNpc(luaVm)->setEndScript(npcid, script);
	return 0;
}

// Imitating NPC's (Player NPC's)
int LuaExports::getPlayerNpcName(lua_State *luaVm) {
	PlayerNpc *pNpc = PlayerNpcDataProvider::Instance()->getPlayerNpc(getNpc(luaVm)->getNpcId());
	lua_pushstring(luaVm, pNpc == nullptr ? "" : pNpc->getName().c_str());
	return 1;
}

int LuaExports::getPlayerNpcLevel(lua_State *luaVm) {
	PlayerNpc *pNpc = PlayerNpcDataProvider::Instance()->getPlayerNpc(getNpc(luaVm)->getNpcId());
	lua_pushinteger(luaVm, pNpc == nullptr ? -1 : pNpc->getLevel());
	return 1;
}

int LuaExports::makeNewImitation(lua_State *luaVm) {
	PlayerNpc *pNpc = PlayerNpcDataProvider::Instance()->getPlayerNpc(getNpc(luaVm)->getNpcId());
	if (pNpc != nullptr) {
		pNpc->renewData(getPlayer(luaVm));
	}
	return 0;
}

// NPC interaction
int LuaExports::addText(lua_State *luaVm) {
	getNpc(luaVm)->addText(lua_tostring(luaVm, -1));
	return 0;
}

int LuaExports::sendBackNext(lua_State *luaVm) {
	getNpc(luaVm)->sendDialog(true, true);
	return lua_yield(luaVm, 0);
}

int LuaExports::sendBackOk(lua_State *luaVm) {
	getNpc(luaVm)->sendDialog(true, false);
	return lua_yield(luaVm, 0);
}

int LuaExports::sendNext(lua_State *luaVm) {
	getNpc(luaVm)->sendDialog(false, true);
	return lua_yield(luaVm, 0);
}

int LuaExports::sendOk(lua_State *luaVm) {
	getNpc(luaVm)->sendDialog(false, false);
	return lua_yield(luaVm, 0);
}

int LuaExports::askAcceptDecline(lua_State *luaVm) {
	getNpc(luaVm)->sendAcceptDecline();
	return lua_yield(luaVm, 1);
}

int LuaExports::askAcceptDeclineNoExit(lua_State *luaVm) {
	getNpc(luaVm)->sendAcceptDeclineNoExit();
	return lua_yield(luaVm, 1);
}

int LuaExports::askChoice(lua_State *luaVm) {
	getNpc(luaVm)->sendSimple();
	return lua_yield(luaVm, 1);
}

int LuaExports::askNumber(lua_State *luaVm) {
	int32_t def = lua_tointeger(luaVm, -3);
	int32_t min = lua_tointeger(luaVm, -2);
	int32_t max = lua_tointeger(luaVm, -1);
	getNpc(luaVm)->sendGetNumber(def, min, max);
	return lua_yield(luaVm, 1);
}

int LuaExports::askStyle(lua_State *luaVm) {
	vector<int32_t> styles;

	lua_pushnil(luaVm);
	while (lua_next(luaVm, 1) != 0) {
		styles.push_back(lua_tointeger(luaVm, -1));
		lua_pop(luaVm, 1);
	}

	if (styles.size() > 0) {
		getNpc(luaVm)->sendStyle(&styles[0], styles.size());
	}

	return lua_yield(luaVm, 1);
}

int LuaExports::askText(lua_State *luaVm) {
	int16_t min = 0;
	int16_t max = 0;
	string def = "";
	if (lua_isnumber(luaVm, -3) && lua_isnumber(luaVm, -2)) {
		min = lua_tointeger(luaVm, -3);
		max = lua_tointeger(luaVm, -2);
		if (lua_isstring(luaVm, -1)) {
			def = lua_tostring(luaVm, -1);
		}
	}
	else {
		if (lua_isstring(luaVm, -1)) {
			def = lua_tostring(luaVm, -1);
		}
	}
	getNpc(luaVm)->sendGetText(min, max, def);
	return lua_yield(luaVm, 1);
}

int LuaExports::askYesNo(lua_State *luaVm) {
	getNpc(luaVm)->sendYesNo();
	return lua_yield(luaVm, 1);
}

int LuaExports::askQuiz(lua_State *luaVm) {
	int8_t type = lua_tointeger(luaVm, -5);
	int32_t objectId = lua_tointeger(luaVm, -4);
	int32_t correct = lua_tointeger(luaVm, -3);
	int32_t questions = lua_tointeger(luaVm, -2);
	int32_t time = lua_tointeger(luaVm, -1);

	getNpc(luaVm)->sendQuiz(type, objectId, correct, questions, time);
	return lua_yield(luaVm, 1);
}

int LuaExports::askQuestion(lua_State *luaVm) {
	string question = lua_tostring(luaVm, -5);
	string clue = lua_tostring(luaVm, -4);
	int32_t minChars = lua_tointeger(luaVm, -3);
	int32_t maxChars = lua_tointeger(luaVm, -2);
	int32_t time = lua_tointeger(luaVm, -1);

	getNpc(luaVm)->sendQuestion(question, clue, minChars, maxChars, time);
	return lua_yield(luaVm, 1);
}

// Quest
int LuaExports::addQuest(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->addQuest(questid, getNpc(luaVm)->getNpcId());
	return 0;
}

int LuaExports::endQuest(lua_State *luaVm) {
	int16_t questid = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->finishQuest(questid, getNpc(luaVm)->getNpcId());
	return 0;
}
