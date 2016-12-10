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
#include "world_server_accept_packet.hpp"
#include "common/config/world.hpp"
#include "common/inter_header.hpp"
#include "common/inter_helper.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "common/util/time.hpp"
#include "world_server/channels.hpp"
#include "world_server/player_data_provider.hpp"
#include "world_server/world_server.hpp"
#include "world_server/world_server_accepted_session.hpp"
#include <unordered_map>
#include <map>

namespace vana {
namespace world_server {
namespace packets {
namespace interserver {

PACKET_IMPL(connect, game_channel_id channel, connection_port port) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_CHANNEL_CONNECT)
		.add<game_channel_id>(channel)
		.add<connection_port>(port)
		.add<vana::config::world>(world_server::get_instance().get_config());
	return builder;
}

PACKET_IMPL(rehash_config, const vana::config::world &config) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_REHASH_CONFIG)
		.add<vana::config::world>(config);
	return builder;
}

}
}
}
}