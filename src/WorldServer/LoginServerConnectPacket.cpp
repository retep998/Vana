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
#include "LoginServerConnectPacket.hpp"
#include "InterHeader.hpp"
#include "LoginServerConnection.hpp"
#include "Session.hpp"
#include "WorldServer.hpp"

namespace LoginServerConnectPacket {

PACKET_IMPL(registerChannel, channel_id_t channel, const Ip &channelIp, const IpMatrix &extIp, port_t port) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_REGISTER_CHANNEL)
		.add<channel_id_t>(channel)
		.add<Ip>(channelIp)
		.add<vector_t<ExternalIp>>(extIp)
		.add<port_t>(port);
	return builder;
}

PACKET_IMPL(updateChannelPop, channel_id_t channel, int32_t population) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_UPDATE_CHANNEL_POP)
		.add<channel_id_t>(channel)
		.add<int32_t>(population);
	return builder;
}

PACKET_IMPL(removeChannel, channel_id_t channel) {
	PacketBuilder builder;
	builder
		.add<header_t>(IMSG_REMOVE_CHANNEL)
		.add<channel_id_t>(channel);
	return builder;
}

}