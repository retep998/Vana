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
#include "WorldServerAcceptPacket.hpp"
#include "Common/InterHeader.hpp"
#include "Common/InterHelper.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Session.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/WorldConfig.hpp"
#include "WorldServer/Channels.hpp"
#include "WorldServer/PlayerDataProvider.hpp"
#include "WorldServer/WorldServer.hpp"
#include "WorldServer/WorldServerAcceptedSession.hpp"
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
		.add<world_config>(world_server::get_instance().get_config());
	return builder;
}

PACKET_IMPL(rehash_config, const world_config &config) {
	packet_builder builder;
	builder
		.add<packet_header>(IMSG_REHASH_CONFIG)
		.add<world_config>(config);
	return builder;
}

}
}
}
}