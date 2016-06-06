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
#pragma once

#include "channel_server/lua_scriptable.hpp"
#include <string>

namespace vana {
	namespace channel_server {
		class npc;

		class lua_npc : public lua_scriptable {
			NONCOPYABLE(lua_npc);
			NO_DEFAULT_CONSTRUCTOR(lua_npc);
		public:
			lua_npc(const string &filename, game_player_id player_id);

			auto proceed_next() -> result;
			auto proceed_selection(uint8_t selected) -> result;
			auto proceed_number(int32_t number) -> result;
			auto proceed_text(const string &text) -> result;
		protected:
			auto handle_error(const string &filename, const string &error) -> void override;
			auto handle_thread_completion() -> void override;
			auto set_npc_environment_variables() -> void;
		};

		namespace lua_exports {
			auto get_npc(lua_State *lua_vm, lua_environment &env) -> npc *;

			// NPC exports

			// Miscellaneous
			auto get_distance_npc(lua_State *lua_vm) -> lua::lua_return;
			auto get_npc_id(lua_State *lua_vm) -> lua::lua_return;
			auto run_npc_npc(lua_State *lua_vm) -> lua::lua_return;
			auto show_storage(lua_State *lua_vm) -> lua::lua_return;

			// NPC interaction
			auto add_text(lua_State *lua_vm) -> lua::lua_return;
			auto ask_accept_decline(lua_State *lua_vm) -> lua::lua_return;
			auto ask_accept_decline_no_exit(lua_State *lua_vm) -> lua::lua_return;
			auto ask_choice(lua_State *lua_vm) -> lua::lua_return;
			auto ask_number(lua_State *lua_vm) -> lua::lua_return;
			auto ask_question(lua_State *lua_vm) -> lua::lua_return;
			auto ask_quiz(lua_State *lua_vm) -> lua::lua_return;
			auto ask_style(lua_State *lua_vm) -> lua::lua_return;
			auto ask_text(lua_State *lua_vm) -> lua::lua_return;
			auto ask_yes_no(lua_State *lua_vm) -> lua::lua_return;
			auto send_back_next(lua_State *lua_vm) -> lua::lua_return;
			auto send_back_ok(lua_State *lua_vm) -> lua::lua_return;
			auto send_next(lua_State *lua_vm) -> lua::lua_return;
			auto send_ok(lua_State *lua_vm) -> lua::lua_return;

			// Quest
			auto add_quest(lua_State *lua_vm) -> lua::lua_return;
			auto end_quest(lua_State *lua_vm) -> lua::lua_return;
		}
	}
}