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
#include "gm_packet.hpp"
#include "common/session.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace gm {

PACKET_IMPL(begin_hide) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x10)
		.add<bool>(true);
	return builder;
}

PACKET_IMPL(end_hide) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x10)
		.add<bool>(false);
	return builder;
}

PACKET_IMPL(warning, bool succeed) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x1d)
		.add<bool>(succeed);
	return builder;
}

PACKET_IMPL(block) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x04)
		.unk<int8_t>(); // Might be something like succeed but it isn't displayed
	return builder;
}

PACKET_IMPL(invalid_character_name) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x06)
		.unk<int8_t>(1);
	return builder;
}

PACKET_IMPL(hired_merchant_place, int8_t mode, int32_t id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x13)
		.add<int8_t>(mode);
	if (mode == hired_merchant_modes::channel) {
		// "Not found" can be expressed by -2
		builder.add<int8_t>(static_cast<int8_t>(id));
	}
	else {
		builder.add<int32_t>(id);
	}
	return builder;
}

PACKET_IMPL(set_get_var_result, const string &name, const string &variable, const string &value) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_GM)
		.add<int8_t>(0x09)
		.add<string>(name)
		.add<string>(variable)
		.add<string>(value);
	return builder;
}

}
}
}
}