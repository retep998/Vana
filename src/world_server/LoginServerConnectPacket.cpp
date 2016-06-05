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
#include "LoginServerConnectPacket.hpp"
#include "common/inter_header.hpp"
#include "common/session.hpp"
#include "world_server/LoginServerSession.hpp"
#include "world_server/WorldServer.hpp"

namespace vana {
namespace world_server {
namespace packets {

PACKET_IMPL(register_channel, game_channel_id channel, const ip &channel_ip, const ip_matrix &ext_ip, connection_port port) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_REGISTER_CHANNEL)
		.add<game_channel_id>(channel)
		.add<ip>(channel_ip)
		.add<vector<external_ip>>(ext_ip)
		.add<connection_port>(port);
	return builder;
}

PACKET_IMPL(update_channel_pop, game_channel_id channel, int32_t population) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_UPDATE_CHANNEL_POP)
		.add<game_channel_id>(channel)
		.add<int32_t>(population);
	return builder;
}

PACKET_IMPL(remove_channel, game_channel_id channel) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_REMOVE_CHANNEL)
		.add<game_channel_id>(channel);
	return builder;
}

}
}
}