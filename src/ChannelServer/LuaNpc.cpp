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
#include "LuaNpc.hpp"
#include "ChannelServer.hpp"
#include "Npc.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "Quests.hpp"
#include "ScriptDataProvider.hpp"
#include "StoragePacket.hpp"
#include <vector>

LuaNpc::LuaNpc(const string_t &filename, player_id_t playerId) :
	LuaScriptable{filename, playerId, true}
{
	setNpcEnvironmentVariables();

	// Miscellaneous
	expose("getDistanceToPlayer", &LuaExports::getDistanceNpc);
	expose("getNpcId", &LuaExports::getNpcId);
	expose("runNpc", &LuaExports::runNpcNpc);
	expose("showStorage", &LuaExports::showStorage);

	// NPC interaction
	expose("addText", &LuaExports::addText);
	expose("askAcceptDecline", &LuaExports::askAcceptDecline);
	expose("askAcceptDeclineNoExit", &LuaExports::askAcceptDeclineNoExit);
	expose("askChoice", &LuaExports::askChoice);
	expose("askNumber", &LuaExports::askNumber);
	expose("askQuestion", &LuaExports::askQuestion);
	expose("askQuiz", &LuaExports::askQuiz);
	expose("askStyle", &LuaExports::askStyle);
	expose("askText", &LuaExports::askText);
	expose("askYesNo", &LuaExports::askYesNo);
	expose("sendBackNext", &LuaExports::sendBackNext);
	expose("sendBackOk", &LuaExports::sendBackOk);
	expose("sendNext", &LuaExports::sendNext);
	expose("sendOk", &LuaExports::sendOk);

	// Quest
	expose("addQuest", &LuaExports::addQuest);
	expose("endQuest", &LuaExports::endQuest);
}

auto LuaNpc::setNpcEnvironmentVariables() -> void {
	set<int32_t>("answer_yes", 1);
	set<int32_t>("answer_no", 0);

	set<int32_t>("answer_accept", 1);
	set<int32_t>("answer_decline", 0);

	set<int32_t>("quiz_npc", 0);
	set<int32_t>("quiz_mob", 1);
	set<int32_t>("quiz_item", 2);
}

auto LuaNpc::handleThreadCompletion() -> void {
	ChannelServer::getInstance().getPlayerDataProvider().getPlayer(m_playerId)->getNpc()->end();
}

auto LuaNpc::proceedNext() -> Result {
	return resume(0);
}

auto LuaNpc::proceedSelection(uint8_t selected) -> Result {
	pushThread<uint8_t>(selected);
	return resume(1);
}

auto LuaNpc::proceedNumber(int32_t number) -> Result {
	pushThread<int32_t>(number);
	return resume(1);
}

auto LuaNpc::proceedText(const string_t &text) -> Result {
	pushThread<string_t>(text);
	return resume(1);
}

auto LuaNpc::handleError(const string_t &filename, const string_t &error) -> void {
	printError(error);
	ChannelServer::getInstance().getPlayerDataProvider().getPlayer(m_playerId)->getNpc()->end();
}

auto LuaExports::getNpc(lua_State *luaVm, LuaEnvironment &env) -> Npc * {
	return getPlayer(luaVm, env)->getNpc();
}

// Miscellaneous
auto LuaExports::getDistanceNpc(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<int32_t>(luaVm, getPlayer(luaVm, env)->getPos() - getNpc(luaVm, env)->getPos());
	return 1;
}

auto LuaExports::getNpcId(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	env.push<npc_id_t>(luaVm, getNpc(luaVm, env)->getNpcId());
	return 1;
}

auto LuaExports::runNpcNpc(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	npc_id_t npcId = env.get<npc_id_t>(luaVm, 1);
	string_t script;
	auto &channel = ChannelServer::getInstance();
	if (env.is(luaVm, 2, LuaType::String)) {
		// We already have our script name
		string_t specified = env.get<string_t>(luaVm, 2);
		script = channel.getScriptDataProvider().buildScriptPath(ScriptTypes::Npc, specified);
	}
	else {
		script = channel.getScriptDataProvider().getScript(&channel, npcId, ScriptTypes::Npc);
	}
	getNpc(luaVm, env)->setEndScript(npcId, script);
	return 0;
}

auto LuaExports::showStorage(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	Player *player = getPlayer(luaVm, env);
	player->send(StoragePacket::showStorage(player, getNpc(luaVm, env)->getNpcId()));
	return 0;
}

// NPC interaction
auto LuaExports::addText(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->addText(env.get<string_t>(luaVm, 1));
	return 0;
}

auto LuaExports::askAcceptDecline(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendAcceptDecline();
	return env.yield(1);
}

auto LuaExports::askAcceptDeclineNoExit(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendAcceptDeclineNoExit();
	return env.yield(1);
}

auto LuaExports::askChoice(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendSimple();
	return env.yield(1);
}

auto LuaExports::askNumber(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	int32_t def = env.get<int32_t>(luaVm, 1);
	int32_t min = env.get<int32_t>(luaVm, 2);
	int32_t max = env.get<int32_t>(luaVm, 3);
	getNpc(luaVm, env)->sendGetNumber(def, min, max);
	return env.yield(1);
}

auto LuaExports::askQuestion(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	string_t question = env.get<string_t>(luaVm, 1);
	string_t clue = env.get<string_t>(luaVm, 2);
	int32_t minLength = env.get<int32_t>(luaVm, 3);
	int32_t maxLength = env.get<int32_t>(luaVm, 4);
	int32_t time = env.get<int32_t>(luaVm, 5);

	getNpc(luaVm, env)->sendQuestion(question, clue, minLength, maxLength, time);
	return env.yield(1);
}

auto LuaExports::askQuiz(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	int8_t type = env.get<int32_t>(luaVm, 1);
	int32_t objectId = env.get<int32_t>(luaVm, 2);
	int32_t correct = env.get<int32_t>(luaVm, 3);
	int32_t questions = env.get<int32_t>(luaVm, 4);
	int32_t time = env.get<int32_t>(luaVm, 5);

	getNpc(luaVm, env)->sendQuiz(type, objectId, correct, questions, time);
	return env.yield(1);
}

auto LuaExports::askStyle(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	vector_t<int32_t> styles = env.get<vector_t<int32_t>>(luaVm, 1);

	if (styles.size() > 0) {
		getNpc(luaVm, env)->sendStyle(styles);
	}

	return env.yield(1);
}

auto LuaExports::askText(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	int16_t min = 0;
	int16_t max = 0;
	string_t def = "";
	if (env.is(luaVm, 1, LuaType::Number) && env.is(luaVm, 2, LuaType::Number)) {
		min = env.get<int32_t>(luaVm, 1);
		max = env.get<int32_t>(luaVm, 2);
		if (env.is(luaVm, 3, LuaType::String)) {
			def = env.get<string_t>(luaVm, 3);
		}
	}
	else if (env.is(luaVm, 1, LuaType::String)) {
		def = env.get<string_t>(luaVm, 1);
	}
	getNpc(luaVm, env)->sendGetText(min, max, def);
	return env.yield(1);
}

auto LuaExports::askYesNo(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendYesNo();
	return env.yield(1);
}

auto LuaExports::sendBackNext(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendDialog(true, true);
	return env.yield(0);
}

auto LuaExports::sendBackOk(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendDialog(true, false);
	return env.yield(0);
}

auto LuaExports::sendNext(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendDialog(false, true);
	return env.yield(0);
}

auto LuaExports::sendOk(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	getNpc(luaVm, env)->sendDialog(false, false);
	return env.yield(0);
}

// Quest
auto LuaExports::addQuest(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	getPlayer(luaVm, env)->getQuests()->addQuest(questId, getNpc(luaVm, env)->getNpcId());
	return 0;
}

auto LuaExports::endQuest(lua_State *luaVm) -> lua_return_t {
	auto &env = getEnvironment(luaVm);
	quest_id_t questId = env.get<quest_id_t>(luaVm, 1);
	getPlayer(luaVm, env)->getQuests()->finishQuest(questId, getNpc(luaVm, env)->getNpcId());
	return 0;
}