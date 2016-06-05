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
#include "LoginServerAcceptPacket.hpp"
#include "common_temp/InterHeader.hpp"
#include "common_temp/InterHelper.hpp"
#include "common_temp/WorldConfig.hpp"
#include "login_server/World.hpp"

namespace vana {
namespace login_server {
namespace packets {
namespace interserver {

PACKET_IMPL(connect, world *world_value) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_WORLD_CONNECT)
		.add<game_world_id>(world_value->get_id().get(-1))
		.add<connection_port>(world_value->get_port())
		.add<world_config>(world_value->get_config());
	return builder;
}

PACKET_IMPL(no_more_world) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_WORLD_CONNECT)
		.add<int8_t>(-1);
	return builder;
}

PACKET_IMPL(connect_channel, optional<game_world_id> world_id, optional<ip> ip_value, optional<connection_port> port) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_LOGIN_CHANNEL_CONNECT)
		.add<game_world_id>(world_id.get(-1))
		.add<ip>(ip_value.get(ip{0}))
		.add<connection_port>(port.get(0));
	return builder;
}

PACKET_IMPL(player_connecting_to_channel, game_channel_id chan_id, game_player_id char_id, const ip &ip_value) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_TO_CHANNEL)
		.add<game_channel_id>(chan_id)
		.add<packet_header>(IMSG_SYNC)
		.add<protocol_sync>(sync::sync_types::player)
		.add<protocol_sync>(sync::player::new_connectable)
		.add<game_player_id>(char_id)
		.add<ip>(ip_value)
		// The size of the held packet that should be there - there isn't one
		.add<uint16_t>(0);
	return builder;
}

PACKET_IMPL(rehash_config, world *world_value) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_REHASH_CONFIG)
		.add<world_config>(world_value->get_config());
	return builder;
}

}
}
}
}