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
#include "common/session.hpp"
#include "common/skill_constants.hpp"
#include "channel_server/Buffs.hpp"
#include "channel_server/BuffsPacketHelper.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {

SPLIT_PACKET_IMPL(add_buff, game_player_id player_id, int32_t buff_id, const seconds &time, const buff_packet_values &buff, int16_t added_info) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_SKILL_USE)
		.add_buffer(helpers::add_buff_bytes(buff.player.types));

	for (const auto &val : buff.player.values) {
		if (val.type == buff_packet_value_type::special_packet) continue;
		if (val.type == buff_packet_value_type::packet) {
			builder.player.add_buffer(val.builder);
			continue;
		}
		if (val.type != buff_packet_value_type::value) throw not_implemented_exception{"buff_packet_value_type"};

		switch (val.value_size) {
			case 1: builder.player.add<int8_t>(static_cast<int8_t>(val.value)); break;
			case 2: builder.player.add<int16_t>(static_cast<int16_t>(val.value)); break;
			case 4: builder.player.add<int32_t>(static_cast<int32_t>(val.value)); break;
			case 8: builder.player.add<int64_t>(val.value); break;
			default: throw not_implemented_exception{"value_size"};
		}
		builder.player
			.add<int32_t>(buff_id)
			.add<int32_t>(static_cast<int32_t>(time.count() * 1000));
	}

	builder.player
		.unk<int8_t>()
		.unk<int8_t>();

	for (const auto &val : buff.player.values) {
		if (val.type != buff_packet_value_type::special_packet) continue;
		builder.player.add_buffer(val.builder);
	}

	builder.player.add<int16_t>(static_cast<int16_t>(buff.delay.count()));

	if (buff.player.any_movement_buffs) {
		builder.player.unk<int8_t>();
	}

	if (buff.map.is_initialized()) {
		const auto &map_buff = buff.map.get();
		builder.map
			.add<packet_header>(SMSG_3RD_PARTY_SKILL)
			.add<game_player_id>(player_id)
			.add_buffer(helpers::add_buff_map_values(map_buff));

		builder.map.add<int16_t>(static_cast<int16_t>(buff.delay.count()));
	}
	return builder;
}

SPLIT_PACKET_IMPL(end_buff, game_player_id player_id, const buff_packet_values &buff) {
	split_packet_builder builder;

	builder.player
		.add<packet_header>(SMSG_SKILL_CANCEL)
		.add_buffer(helpers::add_buff_bytes(buff.player.types))
		.unk<int8_t>();

	if (buff.map.is_initialized()) {
		builder.map
			.add<packet_header>(SMSG_3RD_PARTY_BUFF_END)
			.add<game_player_id>(player_id)
			.add_buffer(helpers::add_buff_bytes(buff.map.get().types));
	}

	return builder;
}

}
}
}