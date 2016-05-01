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
#include "FamePacket.hpp"
#include "Common/Session.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/PlayerDataProvider.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {
namespace Fame {

PACKET_IMPL(sendError, int32_t reason) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_FAME)
		.add<int32_t>(reason);
	return builder;
}

PACKET_IMPL(sendFame, const string_t &name, uint8_t type, int32_t newFame) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_FAME)
		.add<int8_t>(0x00)
		.add<string_t>(name)
		.add<int8_t>(type)
		.add<int32_t>(newFame);
	return builder;
}

PACKET_IMPL(receiveFame, const string_t &name, uint8_t type) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_FAME)
		.add<int8_t>(0x05)
		.add<string_t>(name)
		.add<int8_t>(type);
	return builder;
}

}
}
}
}