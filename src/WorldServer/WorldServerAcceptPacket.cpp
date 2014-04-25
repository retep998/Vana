/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Channels.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "InterHelper.hpp"
#include "PacketReader.hpp"
#include "PlayerDataProvider.hpp"
#include "Session.hpp"
#include "TimeUtilities.hpp"
#include "WorldServer.hpp"
#include "WorldServerAcceptConnection.hpp"
#include <unordered_map>
#include <map>

namespace WorldServerAcceptPacket {

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