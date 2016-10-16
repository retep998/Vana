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
#include "chat_handler.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "common/util/string.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/chat_handler_functions.hpp"
#include "channel_server/map.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/players_packet.hpp"
#include "channel_server/world_server_packet.hpp"
#include <algorithm>

namespace vana {
namespace channel_server {

auto chat_handler::initialize_commands() -> void {
	chat_handler_functions::initialize();
}

auto chat_handler::handle_chat(ref_ptr<player> player, packet_reader &reader) -> void {
	game_chat message = reader.get<game_chat>();
	bool bubble_only = reader.get<bool>(); // Skill macros only display chat bubbles

	if (chat_handler::handle_command(player, message) == handle_result::unhandled) {
		if (player->is_gm_chat()) {
			channel_server::get_instance().get_player_data_provider().handle_gm_chat(player, message);
			return;
		}

		player->send_map(packets::players::show_chat(player->get_id(), player->is_gm(), message, bubble_only));
	}
}

auto chat_handler::handle_command(ref_ptr<player> player, const game_chat &message) -> handle_result {
	using chat_handler_functions::g_command_list;

	if (player->is_admin() && message[0] == '/') {
		// Prevent command printing for Admins
		return handle_result::handled;
	}

	if (player->is_gm() && message[0] == '!' && message.size() > 2) {
		char *chat = const_cast<char *>(message.c_str());
		game_chat command = strtok(chat + 1, " ");
		game_chat args = message.length() > command.length() + 2 ? message.substr(command.length() + 2) : "";
		auto kvp = g_command_list.find(command);
		if (kvp == std::end(g_command_list)) {
			chat_handler_functions::show_error(player, "Invalid command: " + command);
		}
		else {
			auto &cmd = kvp->second;
			if (player->get_gm_level() < cmd.level) {
				chat_handler_functions::show_error(player, "You are not at a high enough GM level to use the command");
			}
			else if (cmd.command(player, args) == chat_result::show_syntax) {
				chat_handler_functions::show_syntax(player, command);
			}
		}
		return handle_result::handled;
	}

	return handle_result::unhandled;
}

auto chat_handler::handle_group_chat(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	uint8_t amount = reader.get<uint8_t>();
	vector<game_player_id> receivers = reader.get<vector<game_player_id>>(amount);
	game_chat chat = reader.get<game_chat>();

	if (chat_handler::handle_command(player, chat) == handle_result::unhandled) {
		channel_server::get_instance().get_player_data_provider().handle_group_chat(type, player->get_id(), receivers, chat);
	}
}

}
}