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
#include "BuffsPacketHelper.hpp"
#include "ChannelServer/Buffs.hpp"
#include "ChannelServer/PlayerActiveBuffs.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace helpers {

PACKET_IMPL(add_buff_bytes, const buff_array &bytes) {
	packet_builder builder;
	for (uint8_t i = 0; i < vana::buffs::byte_quantity; i++) {
		size_t packet_index = 0;
		// This is the order they show up in, ludicrously enough
		switch (i) {
			case 12: packet_index = 0; break;
			case 13: packet_index = 1; break;
			case 14: packet_index = 2; break;
			case 15: packet_index = 3; break;
			case 8: packet_index = 4; break;
			case 9: packet_index = 5; break;
			case 10: packet_index = 6; break;
			case 11: packet_index = 7; break;
			case 4: packet_index = 8; break;
			case 5: packet_index = 9; break;
			case 6: packet_index = 10; break;
			case 7: packet_index = 11; break;
			case 0: packet_index = 12; break;
			case 1: packet_index = 13; break;
			case 2: packet_index = 14; break;
			case 3: packet_index = 15; break;
		}
		builder.add<uint8_t>(bytes[packet_index]);
	}
	return builder;
}

PACKET_IMPL(add_buff_map_values, const buff_packet_structure &buff) {
	packet_builder builder;
	builder.add_buffer(add_buff_bytes(buff.types));

	for (const auto &buff_value : buff.values) {
		if (buff_value.type == buff_packet_value_type::special_packet) continue;
		if (buff_value.type == buff_packet_value_type::packet) {
			builder.add_buffer(buff_value.builder);
			continue;
		}
		if (buff_value.type != buff_packet_value_type::value) throw not_implemented_exception{"buff_packet_value_type"};

		switch (buff_value.value_size) {
			case 1: builder.add<int8_t>(static_cast<int8_t>(buff_value.value)); break;
			case 2: builder.add<int16_t>(static_cast<int16_t>(buff_value.value)); break;
			case 4: builder.add<int32_t>(static_cast<int32_t>(buff_value.value)); break;
			case 8: builder.add<int64_t>(buff_value.value); break;
		}
	}

	builder
		.unk<uint8_t>()
		.unk<uint8_t>();

	for (const auto &buff_value : buff.values) {
		if (buff_value.type != buff_packet_value_type::special_packet) continue;
		builder.add_buffer(buff_value.builder);
	}

	return builder;
}

}
}
}
}