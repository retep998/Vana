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
#include "BuffsPacket.hpp"
#include "Common/Session.hpp"
#include "Common/SkillConstants.hpp"
#include "ChannelServer/Buffs.hpp"
#include "ChannelServer/BuffsPacketHelper.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/SmsgHeader.hpp"

namespace Vana {
namespace ChannelServer {
namespace Packets {

SPLIT_PACKET_IMPL(addBuff, player_id_t playerId, int32_t buffId, const seconds_t &time, const BuffPacketValues &buff, int16_t addedInfo) {
	SplitPacketBuilder builder;
	builder.player
		.add<header_t>(SMSG_SKILL_USE)
		.addBuffer(Helpers::addBuffBytes(buff.player.types));

	for (const auto &val : buff.player.values) {
		if (val.type == BuffPacketValueType::SpecialPacket) continue;
		if (val.type == BuffPacketValueType::Packet) {
			builder.player.addBuffer(val.packet);
			continue;
		}
		if (val.type != BuffPacketValueType::Value) throw NotImplementedException{"BuffPacketValueType"};

		switch (val.valueSize) {
			case 1: builder.player.add<int8_t>(static_cast<int8_t>(val.value)); break;
			case 2: builder.player.add<int16_t>(static_cast<int16_t>(val.value)); break;
			case 4: builder.player.add<int32_t>(static_cast<int32_t>(val.value)); break;
			case 8: builder.player.add<int64_t>(val.value); break;
			default: throw NotImplementedException{"valueSize"};
		}
		builder.player
			.add<int32_t>(buffId)
			.add<int32_t>(static_cast<int32_t>(time.count() * 1000));
	}

	builder.player
		.unk<int8_t>()
		.unk<int8_t>();

	for (const auto &val : buff.player.values) {
		if (val.type != BuffPacketValueType::SpecialPacket) continue;
		builder.player.addBuffer(val.packet);
	}

	builder.player.add<int16_t>(static_cast<int16_t>(buff.delay.count()));

	if (buff.player.anyMovementBuffs) {
		builder.player.unk<int8_t>();
	}

	if (buff.map.is_initialized()) {
		const auto &mapBuff = buff.map.get();
		builder.map
			.add<header_t>(SMSG_3RD_PARTY_SKILL)
			.add<player_id_t>(playerId)
			.addBuffer(Helpers::addBuffMapValues(mapBuff));

		builder.map.add<int16_t>(static_cast<int16_t>(buff.delay.count()));
	}
	return builder;
}

SPLIT_PACKET_IMPL(endBuff, player_id_t playerId, const BuffPacketValues &buff) {
	SplitPacketBuilder builder;

	builder.player
		.add<header_t>(SMSG_SKILL_CANCEL)
		.addBuffer(Helpers::addBuffBytes(buff.player.types))
		.unk<int8_t>();

	if (buff.map.is_initialized()) {
		builder.map
			.add<header_t>(SMSG_3RD_PARTY_BUFF_END)
			.add<player_id_t>(playerId)
			.addBuffer(Helpers::addBuffBytes(buff.map.get().types));
	}

	return builder;
}

}
}
}