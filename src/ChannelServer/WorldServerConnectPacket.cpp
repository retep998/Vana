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
#include "WorldServerConnectPacket.hpp"
#include "ChannelServer.hpp"
#include "Configuration.hpp"
#include "InterHeader.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include "WorldServerConnection.hpp"

namespace WorldServerConnectPacket {

PACKET_IMPL(rankingCalculation) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_TO_LOGIN)
		.add<header_t>(IMSG_CALCULATE_RANKING);
	return builder;
}

PACKET_IMPL(reloadMcdb, const string_t &type) {
	PacketBuilder builder;
	builder
		.add<int16_t>(IMSG_TO_ALL_CHANNELS)
		.add<header_t>(IMSG_REFRESH_DATA)
		.add<string_t>(type);
	return builder;
}

PACKET_IMPL(rehashConfig) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_TO_LOGIN)
		.add<header_t>(IMSG_REHASH_CONFIG);
	return builder;
}

}