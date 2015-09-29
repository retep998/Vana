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
#pragma once

#include "ChannelServer/LuaScriptable.hpp"
#include <string>

namespace Vana {
	namespace ChannelServer {
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
			auto getNpc(lua_State *luaVm, LuaEnvironment &env) -> Npc *;

			// NPC exports

			// Miscellaneous
			auto getDistanceNpc(lua_State *luaVm) -> lua_return_t;
			auto getNpcId(lua_State *luaVm) -> lua_return_t;
			auto runNpcNpc(lua_State *luaVm) -> lua_return_t;
			auto showStorage(lua_State *luaVm) -> lua_return_t;

			// NPC interaction
			auto addText(lua_State *luaVm) -> lua_return_t;
			auto askAcceptDecline(lua_State *luaVm) -> lua_return_t;
			auto askAcceptDeclineNoExit(lua_State *luaVm) -> lua_return_t;
			auto askChoice(lua_State *luaVm) -> lua_return_t;
			auto askNumber(lua_State *luaVm) -> lua_return_t;
			auto askQuestion(lua_State *luaVm) -> lua_return_t;
			auto askQuiz(lua_State *luaVm) -> lua_return_t;
			auto askStyle(lua_State *luaVm) -> lua_return_t;
			auto askText(lua_State *luaVm) -> lua_return_t;
			auto askYesNo(lua_State *luaVm) -> lua_return_t;
			auto sendBackNext(lua_State *luaVm) -> lua_return_t;
			auto sendBackOk(lua_State *luaVm) -> lua_return_t;
			auto sendNext(lua_State *luaVm) -> lua_return_t;
			auto sendOk(lua_State *luaVm) -> lua_return_t;

			// Quest
			auto addQuest(lua_State *luaVm) -> lua_return_t;
			auto endQuest(lua_State *luaVm) -> lua_return_t;
		}
	}
}