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
#include "login_server_session.hpp"
#include "common/common_header.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/server_type.hpp"
#include "world_server/channels.hpp"
#include "world_server/login_server_connect_handler.hpp"
#include "world_server/sync_handler.hpp"
#include "world_server/world_server.hpp"
#include <iostream>

namespace vana {
namespace world_server {

auto login_server_session::handle(packet_reader &reader) -> result {
	switch (reader.get<packet_header>()) {
		case IMSG_WORLD_CONNECT: login_server_connect_handler::connect(shared_from_this(), reader); break;
		case IMSG_REHASH_CONFIG: world_server::get_instance().rehash_config(reader.get<config::world>()); break;
		case IMSG_TO_CHANNEL: {
			game_channel_id channel_id = reader.get<game_channel_id>();
			world_server::get_instance().get_channels().send(channel_id, packets::identity(reader));
			break;
		}
		case IMSG_TO_CHANNEL_LIST: {
			vector<game_channel_id> channels = reader.get<vector<game_channel_id>>();
			world_server::get_instance().get_channels().send(channels, packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_CHANNELS: world_server::get_instance().get_channels().send(packets::identity(reader)); break;
		case IMSG_SYNC: sync_handler::handle(shared_from_this(), reader); break;

		case CMSG_PONG:
		case SMSG_PING:
			/* Intentionally blank */
			break;

		default: return result::failure;
	}
	return result::success;
}

auto login_server_session::on_connect() -> void {
	world_server::get_instance().on_connect_to_login(shared_from_this());
}

auto login_server_session::on_disconnect() -> void {
	world_server::get_instance().on_disconnect_from_login();
}

}
}