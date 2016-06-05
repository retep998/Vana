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
#include "MessageFunctions.hpp"
#include "common/InterHeader.hpp"
#include "common/PacketWrapper.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/PlayerPacket.hpp"

namespace vana {
namespace channel_server {

auto message_functions::world_message(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) (.+))", matches) == match_result::any_matches) {
		string raw_type = matches[1];
		int8_t type = chat_handler_functions::get_message_type(raw_type);
		if (type != -1) {
			string message = matches[2];
			channel_server::get_instance().send_world(
				vana::packets::prepend(packets::player::show_message(message, type), [](packet_builder &builder) {
					builder.add<packet_header>(IMSG_TO_ALL_CHANNELS);
					builder.add<packet_header>(IMSG_TO_ALL_PLAYERS);
				}));
		}
		else {
			chat_handler_functions::show_error(player, "Invalid message type: " + raw_type);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto message_functions::global_message(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) (.+))", matches) == match_result::any_matches) {
		string raw_type = matches[1];
		int8_t type = chat_handler_functions::get_message_type(raw_type);
		if (type != -1) {
			string message = matches[2];
			channel_server::get_instance().send_world(
				vana::packets::prepend(packets::player::show_message(message, type), [](packet_builder &builder) {
					builder.add<packet_header>(IMSG_TO_LOGIN);
					builder.add<packet_header>(IMSG_TO_ALL_WORLDS);
					builder.add<packet_header>(IMSG_TO_ALL_CHANNELS);
					builder.add<packet_header>(IMSG_TO_ALL_PLAYERS);
				}));
		}
		else {
			chat_handler_functions::show_error(player, "Invalid message type: " + raw_type);
		}
		return chat_result::handled_display;
	}
	return chat_result::show_syntax;
}

auto message_functions::channel_message(ref_ptr<player> player, const game_chat &args) -> chat_result {
	match matches;
	if (chat_handler_functions::run_regex_pattern(args, R"((\w+) (.+))", matches) == match_result::any_matches) {
		string raw_type = matches[1];
		int8_t type = chat_handler_functions::get_message_type(raw_type);
		if (type != -1) {
			string message = matches[2];
			channel_server::get_instance().get_player_data_provider().send(packets::player::show_message(message, type));
		}
		else {
			chat_handler_functions::show_error(player, "Invalid message type: " + raw_type);
		}
		return chat_result::handled_display;
	}

	return chat_result::show_syntax;
}

auto message_functions::gm_chat_mode(ref_ptr<player> player, const game_chat &args) -> chat_result {
	player->set_gm_chat(!player->is_gm_chat());
	chat_handler_functions::show_info(player, [&](out_stream &message) { message << "GM chat mode " << (player->is_gm_chat() ? "enabled" : "disabled"); });
	return chat_result::handled_display;
}

}
}