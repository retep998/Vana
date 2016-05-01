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
#include "GmPacket.hpp"
#include "Common/Session.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {
namespace Gm {

PACKET_IMPL(beginHide) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x10)
		.add<bool>(true);
	return builder;
}

PACKET_IMPL(endHide) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x10)
		.add<bool>(false);
	return builder;
}

PACKET_IMPL(warning, bool succeed) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x1d)
		.add<bool>(succeed);
	return builder;
}

PACKET_IMPL(block) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x04)
		.unk<int8_t>(); // Might be something like succeed but it isn't displayed
	return builder;
}

PACKET_IMPL(invalidCharacterName) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x06)
		.unk<int8_t>(1);
	return builder;
}

PACKET_IMPL(hiredMerchantPlace, int8_t mode, int32_t id) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x13)
		.add<int8_t>(mode);
	if (mode == HiredMerchantModes::Channel) {
		// "Not found" can be expressed by -2
		builder.add<int8_t>(static_cast<int8_t>(id));
	}
	else {
		builder.add<int32_t>(id);
	}
	return builder;
}

PACKET_IMPL(setGetVarResult, const string_t &name, const string_t &variable, const string_t &value) {
	PacketBuilder builder;
	builder
		.add<header_t>(SMSG_GM)
		.add<int8_t>(0x09)
		.add<string_t>(name)
		.add<string_t>(variable)
		.add<string_t>(value);
	return builder;
}

}
}
}
}