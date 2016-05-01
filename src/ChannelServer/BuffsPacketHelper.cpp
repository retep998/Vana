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

namespace Vana {
namespace ChannelServer {
namespace Packets {
namespace Helpers {

PACKET_IMPL(addBuffBytes, const buff_array_t &bytes) {
	PacketBuilder builder;
	for (uint8_t i = 0; i < Vana::Buffs::ByteQuantity; i++) {
		size_t packetIndex = 0;
		// This is the order they show up in, ludicrously enough
		switch (i) {
			case 12: packetIndex = 0; break;
			case 13: packetIndex = 1; break;
			case 14: packetIndex = 2; break;
			case 15: packetIndex = 3; break;
			case 8: packetIndex = 4; break;
			case 9: packetIndex = 5; break;
			case 10: packetIndex = 6; break;
			case 11: packetIndex = 7; break;
			case 4: packetIndex = 8; break;
			case 5: packetIndex = 9; break;
			case 6: packetIndex = 10; break;
			case 7: packetIndex = 11; break;
			case 0: packetIndex = 12; break;
			case 1: packetIndex = 13; break;
			case 2: packetIndex = 14; break;
			case 3: packetIndex = 15; break;
		}
		builder.add<uint8_t>(bytes[packetIndex]);
	}
	return builder;
}

PACKET_IMPL(addBuffMapValues, const BuffPacketStructure &buff) {
	PacketBuilder builder;
	builder.addBuffer(addBuffBytes(buff.types));

	for (const auto &buffValue : buff.values) {
		if (buffValue.type == BuffPacketValueType::SpecialPacket) continue;
		if (buffValue.type == BuffPacketValueType::Packet) {
			builder.addBuffer(buffValue.packet);
			continue;
		}
		if (buffValue.type != BuffPacketValueType::Value) throw NotImplementedException{"BuffPacketValueType"};

		switch (buffValue.valueSize) {
			case 1: builder.add<int8_t>(static_cast<int8_t>(buffValue.value)); break;
			case 2: builder.add<int16_t>(static_cast<int16_t>(buffValue.value)); break;
			case 4: builder.add<int32_t>(static_cast<int32_t>(buffValue.value)); break;
			case 8: builder.add<int64_t>(buffValue.value); break;
		}
	}

	builder
		.unk<uint8_t>()
		.unk<uint8_t>();

	for (const auto &buffValue : buff.values) {
		if (buffValue.type != BuffPacketValueType::SpecialPacket) continue;
		builder.addBuffer(buffValue.packet);
	}

	return builder;
}

}
}
}
}