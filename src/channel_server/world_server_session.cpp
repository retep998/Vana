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
#include "world_server_session.hpp"
#include "common/common_header.hpp"
#include "common/config/world.hpp"
#include "common/exit_code.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/server_type.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/party_handler.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/sync_handler.hpp"
#include "channel_server/world_server_session_handler.hpp"
#include "channel_server/world_server_packet.hpp"
#include <iostream>

namespace vana {
namespace channel_server {

auto world_server_session::handle(packet_reader &reader) -> result {
	switch (reader.get<packet_header>()) {
		case IMSG_CHANNEL_CONNECT: world_server_session_handler::connect(shared_from_this(), reader); break;
		case IMSG_TO_PLAYER: {
			game_player_id player_id = reader.get<game_player_id>();
			channel_server::get_instance().get_player_data_provider().send(player_id, vana::packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector<game_player_id> player_ids = reader.get<vector<game_player_id>>();
			channel_server::get_instance().get_player_data_provider().send(player_ids, vana::packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: channel_server::get_instance().get_player_data_provider().send(vana::packets::identity(reader)); break;
		case IMSG_REFRESH_DATA: world_server_session_handler::reload_mcdb(reader); break;
		case IMSG_REHASH_CONFIG: channel_server::get_instance().set_config(reader.get<config::world>()); break;
		case IMSG_SYNC: sync_handler::handle(reader); break;

		case CMSG_PONG:
		case SMSG_PING:
			/* Intentionally blank */
			break;

		default: return result::failure;
	}
	return result::success;
}

auto world_server_session::on_connect() -> void {
	channel_server::get_instance().on_connect_to_world(shared_from_this());
}

auto world_server_session::on_disconnect() -> void {
	channel_server::get_instance().on_disconnect_from_world();
}

}
}