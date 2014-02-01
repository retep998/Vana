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
#include "LuaNpc.hpp"
#include "Npc.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Quests.hpp"
#include "ScriptDataProvider.hpp"
#include "StoragePacket.hpp"
#include <vector>

LuaNpc::LuaNpc(const string_t &filename, int32_t playerId) :
	LuaScriptable(filename, playerId)
{
	m_luaThread = lua_newthread(m_luaVm);

	// Miscellaneous
	expose("showStorage", &LuaExports::showStorage);
	expose("getDistanceToPlayer", &LuaExports::getDistanceNpc);
	expose("getNpcId", &LuaExports::getNpcId);
	expose("runNpc", &LuaExports::npcRunNpc);

	// NPC interaction
	expose("addText", &LuaExports::addText);
	expose("sendBackNext", &LuaExports::sendBackNext);
	expose("sendBackOk", &LuaExports::sendBackOk);
	expose("sendNext", &LuaExports::sendNext);
	expose("sendOk", &LuaExports::sendOk);
	expose("askAcceptDecline", &LuaExports::askAcceptDecline);
	expose("askAcceptDeclineNoExit", &LuaExports::askAcceptDeclineNoExit);
	expose("askChoice", &LuaExports::askChoice);
	expose("askNumber", &LuaExports::askNumber);
	expose("askStyle", &LuaExports::askStyle);
	expose("askText", &LuaExports::askText);
	expose("askYesNo", &LuaExports::askYesNo);
	expose("askQuiz", &LuaExports::askQuiz);
	expose("askQuestion", &LuaExports::askQuestion);

	// Quest
	expose("addQuest", &LuaExports::addQuest);
	expose("endQuest", &LuaExports::endQuest);
}

auto LuaNpc::run() -> Result {
	if (luaL_loadfile(m_luaThread, m_filename.c_str())) {
		// Error in lua script
		handleError();
		return Result::Failure;
	}
	return resume(0); // Start running the script
}

auto LuaNpc::resume(int32_t nArgs) -> Result {
	int32_t ret = lua_resume(m_luaThread, m_luaVm, nArgs);
	if (ret == 0) {
		// NPC finished
		PlayerDataProvider::getInstance().getPlayer(m_playerId)->getNpc()->end();
	}
	else if (ret != LUA_YIELD) {
		// Error, a working NPC returns either 0 or LUA_YIELD
		handleError();
		return Result::Failure;
	}
	return Result::Successful;
}

auto LuaNpc::proceedNext() -> Result {
	return resume(0);
}

auto LuaNpc::proceedSelection(uint8_t selected) -> Result {
	lua_pushinteger(m_luaThread, selected);
	return resume(1);
}

auto LuaNpc::proceedNumber(int32_t number) -> Result {
	lua_pushinteger(m_luaThread, number);
	return resume(1);
}

auto LuaNpc::proceedText(const string_t &text) -> Result {
	lua_pushstring(m_luaThread, text.c_str());
	return resume(1);
}

auto LuaNpc::handleError() -> void {
	printError(lua_tostring(m_luaThread, -1));
	PlayerDataProvider::getInstance().getPlayer(m_playerId)->getNpc()->end();
}

auto LuaExports::getNpc(lua_State *luaVm) -> Npc * {
	return getPlayer(luaVm)->getNpc();
}

// Miscellaneous
auto LuaExports::showStorage(lua_State *luaVm) -> int {
	Player *player = getPlayer(luaVm);
	player->send(StoragePacket::showStorage(player, getNpc(luaVm)->getNpcId()));
	return 0;
}

auto LuaExports::getDistanceNpc(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getPlayer(luaVm)->getPos() - getNpc(luaVm)->getPos());
	return 1;
}

auto LuaExports::getNpcId(lua_State *luaVm) -> int {
	lua_pushinteger(luaVm, getNpc(luaVm)->getNpcId());
	return 1;
}

auto LuaExports::npcRunNpc(lua_State *luaVm) -> int {
	int32_t npcId = lua_tointeger(luaVm, 1);
	string_t script;
	if (lua_type(luaVm, 2) == LUA_TSTRING) {
		// We already have our script name
		string_t specified = lua_tostring(luaVm, 2);
		script = "scripts/npcs/" + specified + ".lua";
	}
	else {
		script = ScriptDataProvider::getInstance().getScript(npcId, ScriptTypes::Npc);
	}
	getNpc(luaVm)->setEndScript(npcId, script);
	return 0;
}

// NPC interaction
auto LuaExports::addText(lua_State *luaVm) -> int {
	getNpc(luaVm)->addText(lua_tostring(luaVm, -1));
	return 0;
}

auto LuaExports::sendBackNext(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendDialog(true, true);
	return lua_yield(luaVm, 0);
}

auto LuaExports::sendBackOk(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendDialog(true, false);
	return lua_yield(luaVm, 0);
}

auto LuaExports::sendNext(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendDialog(false, true);
	return lua_yield(luaVm, 0);
}

auto LuaExports::sendOk(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendDialog(false, false);
	return lua_yield(luaVm, 0);
}

auto LuaExports::askAcceptDecline(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendAcceptDecline();
	return lua_yield(luaVm, 1);
}

auto LuaExports::askAcceptDeclineNoExit(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendAcceptDeclineNoExit();
	return lua_yield(luaVm, 1);
}

auto LuaExports::askChoice(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendSimple();
	return lua_yield(luaVm, 1);
}

auto LuaExports::askNumber(lua_State *luaVm) -> int {
	int32_t def = lua_tointeger(luaVm, -3);
	int32_t min = lua_tointeger(luaVm, -2);
	int32_t max = lua_tointeger(luaVm, -1);
	getNpc(luaVm)->sendGetNumber(def, min, max);
	return lua_yield(luaVm, 1);
}

auto LuaExports::askStyle(lua_State *luaVm) -> int {
	vector_t<int32_t> styles;

	lua_pushnil(luaVm);
	while (lua_next(luaVm, 1) != 0) {
		styles.push_back(lua_tointeger(luaVm, -1));
		lua_pop(luaVm, 1);
	}

	if (styles.size() > 0) {
		getNpc(luaVm)->sendStyle(styles);
	}

	return lua_yield(luaVm, 1);
}

auto LuaExports::askText(lua_State *luaVm) -> int {
	int16_t min = 0;
	int16_t max = 0;
	string_t def = "";
	if (lua_isnumber(luaVm, -3) && lua_isnumber(luaVm, -2)) {
		min = lua_tointeger(luaVm, -3);
		max = lua_tointeger(luaVm, -2);
		if (lua_isstring(luaVm, -1)) {
			def = lua_tostring(luaVm, -1);
		}
	}
	else if (lua_isstring(luaVm, -1)) {
		def = lua_tostring(luaVm, -1);
	}
	getNpc(luaVm)->sendGetText(min, max, def);
	return lua_yield(luaVm, 1);
}

auto LuaExports::askYesNo(lua_State *luaVm) -> int {
	getNpc(luaVm)->sendYesNo();
	return lua_yield(luaVm, 1);
}

auto LuaExports::askQuiz(lua_State *luaVm) -> int {
	int8_t type = lua_tointeger(luaVm, -5);
	int32_t objectId = lua_tointeger(luaVm, -4);
	int32_t correct = lua_tointeger(luaVm, -3);
	int32_t questions = lua_tointeger(luaVm, -2);
	int32_t time = lua_tointeger(luaVm, -1);

	getNpc(luaVm)->sendQuiz(type, objectId, correct, questions, time);
	return lua_yield(luaVm, 1);
}

auto LuaExports::askQuestion(lua_State *luaVm) -> int {
	string_t question = lua_tostring(luaVm, -5);
	string_t clue = lua_tostring(luaVm, -4);
	int32_t minChars = lua_tointeger(luaVm, -3);
	int32_t maxChars = lua_tointeger(luaVm, -2);
	int32_t time = lua_tointeger(luaVm, -1);

	getNpc(luaVm)->sendQuestion(question, clue, minChars, maxChars, time);
	return lua_yield(luaVm, 1);
}

// Quest
auto LuaExports::addQuest(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->addQuest(questId, getNpc(luaVm)->getNpcId());
	return 0;
}

auto LuaExports::endQuest(lua_State *luaVm) -> int {
	int16_t questId = lua_tointeger(luaVm, -1);
	getPlayer(luaVm)->getQuests()->finishQuest(questId, getNpc(luaVm)->getNpcId());
	return 0;
}