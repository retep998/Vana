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

#include "common/Types.hpp"
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace channel_server {
		class player;

		enum class chat_result {
			handled_display,
			show_syntax,
		};

		using chat_handler_function = chat_result(*)(ref_ptr<player>, const game_chat &args);
		struct chat_command {
			auto add_to_map() -> chat_command {
				// Duplicates the command and then clears the data to ease addition syntax, intentionally leaving out level
				chat_command x = *this;
				notes.clear();
				syntax = "";
				return x;
			}

			chat_handler_function command;
			int32_t level = 0;
			game_chat syntax;
			vector<game_chat> notes;
		};

		struct map_pair {
			map_pair(game_map_id map_id, game_chat category) :
				map_id{map_id},
				category{category}
			{
			}

			game_map_id map_id;
			game_chat category;
		};

		namespace chat_handler_functions {
			extern case_insensitive_hash_map<chat_command, game_chat> g_command_list;
			extern const case_insensitive_hash_map<map_pair, game_chat> g_map_associations;

			auto initialize() -> void;
			auto get_message_type(const game_chat &query) -> int8_t;
			auto get_map(const game_chat &query, ref_ptr<player> player) -> game_map_id;
			auto get_job(const game_chat &query) -> game_job_id;
			auto get_ban_string(int8_t reason) -> game_chat;
			auto run_regex_pattern(const game_chat &args, const game_chat &pattern, match &matches) -> match_result;
			auto show_syntax(ref_ptr<player> player, const game_chat &command, bool from_help = false) -> void;
			auto show_error(ref_ptr<player> player, const game_chat &message) -> void;
			auto show_info(ref_ptr<player> player, const game_chat &message) -> void;
			auto show_error(ref_ptr<player> player, function<void(game_chat_stream &)> produce_message) -> void;
			auto show_info(ref_ptr<player> player, function<void(game_chat_stream &)> produce_message) -> void;
			auto show_error(ref_ptr<player> player, const char *message) -> void;
			auto show_info(ref_ptr<player> player, const char *message) -> void;
		}
	}
}