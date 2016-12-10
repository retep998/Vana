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
#include "world_server_accepted_session.hpp"
#include "common/common_header.hpp"
#include "common/inter_header.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/server_type.hpp"
#include "common/session.hpp"
#include "common/util/misc.hpp"
#include "common/util/string.hpp"
#include "world_server/channels.hpp"
#include "world_server/login_server_connect_packet.hpp"
#include "world_server/player_data_provider.hpp"
#include "world_server/sync_handler.hpp"
#include "world_server/sync_packet.hpp"
#include "world_server/world_server.hpp"
#include "world_server/world_server_accept_packet.hpp"
#include <iostream>

namespace vana {
namespace world_server {

world_server_accepted_session::world_server_accepted_session(abstract_server &server) :
	server_accepted_session{server}
{
}

auto world_server_accepted_session::handle(packet_reader &reader) -> result {
	if (server_accepted_session::handle(reader) == result::failure) {
		return result::failure;
	}

	auto &server = world_server::get_instance();
	switch (reader.get<packet_header>()) {
		case IMSG_SYNC: sync_handler::handle(shared_from_this(), reader); break;
		case IMSG_TO_LOGIN: server.send_login(vana::packets::identity(reader)); break;
		case IMSG_TO_PLAYER: {
			game_player_id player_id = reader.get<game_player_id>();
			server.get_player_data_provider().send(player_id, vana::packets::identity(reader));
			break;
		}
		case IMSG_TO_PLAYER_LIST: {
			vector<game_player_id> player_ids = reader.get<vector<game_player_id>>();
			server.get_player_data_provider().send(player_ids, vana::packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_PLAYERS: server.get_player_data_provider().send(vana::packets::identity(reader)); break;
		case IMSG_TO_CHANNEL: {
			game_channel_id channel_id = reader.get<game_channel_id>();
			server.get_channels().send(channel_id, vana::packets::identity(reader));
			break;
		}
		case IMSG_TO_CHANNEL_LIST: {
			vector<game_channel_id> channels = reader.get<vector<game_channel_id>>();
			server.get_channels().send(channels, vana::packets::identity(reader));
			break;
		}
		case IMSG_TO_ALL_CHANNELS: server.get_channels().send(vana::packets::identity(reader)); break;

		case CMSG_PONG:
		case SMSG_PING:
		case IMSG_PASSWORD:
			/* Intentionally blank */
			break;

		default: return result::failure;
	}
	return result::success;
}

auto world_server_accepted_session::authenticated(server_type type) -> void {
	if (type == server_type::channel) {
		auto &server = world_server::get_instance();
		m_channel = server.get_channels().get_first_available_channel_id();
		if (m_channel != -1) {
			auto ip_value = get_ip().get(ip{0});
			connection_port port = server.make_channel_port(m_channel);
			const ip_matrix &ips = get_external_ips();
			server.get_channels().register_channel(shared_from_this(), m_channel, ip_value, ips, port);

			send(packets::interserver::connect(m_channel, port));

			// TODO FIXME packet - a more elegant way to do this?
			send(packets::interserver::send_sync_data([&](packet_builder &builder) {
				server.get_player_data_provider().get_channel_connect_packet(builder);
			}));

			server.send_login(packets::register_channel(m_channel, ip_value, ips, port));

			server.log(vana::log::type::server_connect, [&](out_stream &log) {
				log << "Channel " << static_cast<int32_t>(m_channel);
			});
		}
		else {
			send(packets::interserver::connect(-1, 0));
			server.log(vana::log::type::error, "No more channels to assign.");
			disconnect();
		}
	}
}

auto world_server_accepted_session::get_channel() const -> game_channel_id {
	return m_channel;
}

auto world_server_accepted_session::on_disconnect() -> void {
	if (is_authenticated()) {
		if (get_type() == server_type::channel) {
			auto &server = world_server::get_instance();
			if (server.is_connected()) {
				server.send_login(packets::remove_channel(m_channel));
			}
			server.get_player_data_provider().channel_disconnect(m_channel);
			server.get_channels().remove_channel(m_channel);

			server.log(vana::log::type::server_disconnect, [&](out_stream &log) { log << "Channel " << static_cast<int32_t>(m_channel); });
		}
	}
}

}
}