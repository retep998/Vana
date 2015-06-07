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
#include "MapleTvPacket.hpp"
#include "MapleTvs.hpp"
#include "Maps.hpp"
#include "Player.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"

namespace MapleTvPacket {

PACKET_IMPL(showMessage, const MapleTvMessage &message, seconds_t timeLeft) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_MAPLETV_ON)
		.add<int8_t>(message.hasReceiver ? 3 : 1)
		.add<int8_t>(static_cast<int8_t>(message.megaphoneId - 5075000))
		.addBuffer(message.sendDisplay)
		.add<string_t>(message.sendName)
		.add<string_t>(message.hasReceiver ? message.recvName : "")
		.add<string_t>(message.msg1)
		.add<string_t>(message.msg2)
		.add<string_t>(message.msg3)
		.add<string_t>(message.msg4)
		.add<string_t>(message.msg5)
		.add<int32_t>(timeLeft.count() == 0 ? message.time : static_cast<int32_t>(timeLeft.count()));
	if (message.hasReceiver) {
		builder.addBuffer(message.recvDisplay);
	}
	return builder;
}

PACKET_IMPL(endDisplay) {
	PacketBuilder builder;
	builder.add<header_t>(SMSG_MAPLETV_OFF);
	return builder;
}

}