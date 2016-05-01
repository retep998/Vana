/*
Copyright (C) 2008-2015 Vana Development Team

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

namespace Vana {
namespace WorldServer {
namespace Packets {
namespace Interserver {

PACKET_IMPL(connect, channel_id_t channel, port_t port) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_CHANNEL_CONNECT)
		.add<channel_id_t>(channel)
		.add<port_t>(port)
		.add<WorldConfig>(WorldServer::getInstance().getConfig());
	return builder;
}

PACKET_IMPL(rehashConfig, const WorldConfig &config) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_REHASH_CONFIG)
		.add<WorldConfig>(config);
	return builder;
}

}
}
}
}