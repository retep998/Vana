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

#include "common/point.hpp"
#include "common/types.hpp"
#include "channel_server/lua/lua_npc.hpp"
#include <memory>
#include <string>
#include <vector>

namespace vana {
	namespace channel_server {
		class player;
		namespace lua {
			class lua_npc;
		}

		class npc {
			NONCOPYABLE(npc);
			NO_DEFAULT_CONSTRUCTOR(npc);
		public:
			npc(game_npc_id npc_id, ref_ptr<player> player, game_quest_id quest_id = 0, bool is_start = false);
			npc(game_npc_id npc_id, ref_ptr<player> player, const point &pos, game_quest_id quest_id = 0, bool is_start = false);
			npc(game_npc_id npc_id, ref_ptr<player> player, const string &script);

			static auto has_script(game_npc_id npc_id, game_quest_id quest_id, bool start) -> bool;

			auto run() -> void;

			auto send_simple() -> void;
			auto send_yes_no() -> void;
			auto send_dialog(bool back, bool next, bool save = true) -> void;
			auto send_accept_decline() -> void;
			auto send_accept_decline_no_exit() -> void;
			auto send_get_text(int16_t min, int16_t max, const string &def = "") -> void;
			auto send_get_number(int32_t def, int32_t min, int32_t max) -> void;
			auto send_style(vector<int32_t> styles) -> void;
			auto send_quiz(int8_t type, int32_t object_id, int32_t correct, int32_t questions, int32_t time) -> void;
			auto send_question(const string &question, const string &clue, int32_t min_length, int32_t max_length, int32_t time) -> void;
			auto add_text(const string &text) -> void { m_text += text; }
			auto end() -> void { m_cend = true; }

			auto proceed_back() -> void;
			auto proceed_next() -> void;
			auto proceed_selection(uint8_t selected) -> void;
			auto proceed_number(int32_t number) -> void;
			auto proceed_text(const string &text) -> void;

			auto get_player() const -> ref_ptr<player> { return m_player; }
			auto get_sent_dialog() const -> uint8_t { return m_sent_dialog; }
			auto get_npc_id() const -> game_npc_id { return m_npc_id; }
			auto get_number() const -> int32_t { return m_get_num; }
			auto get_selected() const -> int32_t { return m_selected; }
			auto get_text() -> string & { return m_get_text; }

			auto is_end() const -> bool { return m_cend; }
			auto get_pos() const -> point { return m_pos; }

			auto set_end_script(int32_t npc_id, const string &fullscript) -> void;

			auto check_end() -> bool;
			auto show_shop() -> void;
		private:
			struct npc_chat_state {
				NONCOPYABLE(npc_chat_state);
			public:
				npc_chat_state(const string &text, bool back, bool next) :
					text(text),
					back(back),
					next(next)
				{
				}

				bool back = false;
				bool next = false;
				string text;
			};

			auto send_dialog(ref_ptr<npc_chat_state> npc_state) -> void;
			auto get_script(game_quest_id quest_id, bool start) -> string;
			auto init_script(const string &filename) -> void;

			bool m_cend = false;
			uint8_t m_sent_dialog = 0; // Used to check if the user respond with the same type of the dialog sent
			uint8_t m_selected = 0;
			int32_t m_next_npc = 0;
			game_npc_id m_npc_id = 0;
			int32_t m_get_num = 0;
			uint32_t m_state = 0;
			ref_ptr<player> m_player = nullptr;
			string m_text;
			string m_get_text;
			string m_script;
			point m_pos;
			owned_ptr<lua::lua_npc> m_lua_npc;
			vector<ref_ptr<npc_chat_state>> m_previous_states;
		};
	}
}