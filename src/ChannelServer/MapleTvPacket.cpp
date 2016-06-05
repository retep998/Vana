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
#include "MapleTvPacket.hpp"
#include "Common/Session.hpp"
#include "ChannelServer/MapleTvs.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace maple_tv {

PACKET_IMPL(show_message, const maple_tv_message &message, seconds time_left) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MAPLETV_ON)
		.add<int8_t>(message.has_receiver ? 3 : 1)
		.add<int8_t>(static_cast<int8_t>(message.megaphone_id - 5075000))
		.add_buffer(message.send_display)
		.add<string>(message.send_name)
		.add<string>(message.has_receiver ? message.recv_name : "")
		.add<string>(message.msg1)
		.add<string>(message.msg2)
		.add<string>(message.msg3)
		.add<string>(message.msg4)
		.add<string>(message.msg5)
		.add<int32_t>(time_left.count() == 0 ? message.time : static_cast<int32_t>(time_left.count()));

	if (message.has_receiver) {
		builder.add_buffer(message.recv_display);
	}
	return builder;
}

PACKET_IMPL(end_display) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_MAPLETV_OFF);
	return builder;
}

}
}
}
}