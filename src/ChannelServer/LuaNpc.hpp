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
#pragma once

#include "LuaScriptable.hpp"
#include <string>

class Npc;

class LuaNpc : public LuaScriptable {
	NONCOPYABLE(LuaNpc);
	NO_DEFAULT_CONSTRUCTOR(LuaNpc);
public:
	LuaNpc(const string_t &filename, player_id_t playerId);

	auto proceedNext() -> Result;
	auto proceedSelection(uint8_t selected) -> Result;
	auto proceedNumber(int32_t number) -> Result;
	auto proceedText(const string_t &text) -> Result;
protected:
	auto handleError(const string_t &filename, const string_t &error) -> void override;
	auto handleThreadCompletion() -> void override;
	auto setNpcEnvironmentVariables() -> void;
};

namespace LuaExports {
	auto getNpc(lua_State *luaVm) -> Npc *;

	// NPC exports

	// Miscellaneous
	auto showStorage(lua_State *luaVm) -> int;
	auto getDistanceNpc(lua_State *luaVm) -> int;
	auto getNpcId(lua_State *luaVm) -> int;
	auto npcRunNpc(lua_State *luaVm) -> int;

	// NPC interaction
	auto addText(lua_State *luaVm) -> int;
	auto sendBackNext(lua_State *luaVm) -> int;
	auto sendBackOk(lua_State *luaVm) -> int;
	auto sendNext(lua_State *luaVm) -> int;
	auto sendOk(lua_State *luaVm) -> int;
	auto askAcceptDecline(lua_State *luaVm) -> int;
	auto askAcceptDeclineNoExit(lua_State *luaVm) -> int;
	auto askChoice(lua_State *luaVm) -> int;
	auto askNumber(lua_State *luaVm) -> int;
	auto askStyle(lua_State *luaVm) -> int;
	auto askText(lua_State *luaVm) -> int;
	auto askYesNo(lua_State *luaVm) -> int;
	auto askQuiz(lua_State *luaVm) -> int;
	auto askQuestion(lua_State *luaVm) -> int;

	// Quest
	auto addQuest(lua_State *luaVm) -> int;
	auto endQuest(lua_State *luaVm) -> int;
}