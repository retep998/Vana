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
#include "movement_handler.hpp"
#include "common/point.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/movable_life.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace channel_server {

auto movement_handler::parse_movement(movable_life *life, packet_reader &reader) -> point {
	game_foothold_id foothold = 0;
	int8_t stance = 0;
	game_coord x = 0;
	game_coord y = 0;
	uint8_t movement_count = reader.get<uint8_t>();
	bool break_loop = false;

	enum movement_types {
		normal_movement = 0,
		jump = 1,
		jump_kb = 2,
		unk1 = 3,
		teleport = 4,
		normal_movement2 = 5,
		flash_jump = 6,
		assaulter = 7,
		assassinate = 8,
		rush = 9,
		falling = 10,
		chair = 11,
		excessive_kb = 12,
		recoil_shot = 13,
		unk2 = 14,
		jump_down = 15,
		wings = 16,
		wings_falling = 17
	};

	for (uint8_t i = 0; i < movement_count; ++i) {
		int8_t type = reader.get<int8_t>();
		switch (type) {
			case falling:
				reader.unk<uint8_t>();
				break;
			case wings:
				reader.unk<uint8_t>();
				reader.unk<uint16_t>();
				reader.unk<uint32_t>();
				break;
			case wings_falling:
				x = reader.get<game_coord>();
				y = reader.get<game_coord>();
				foothold = reader.get<game_foothold_id>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				reader.unk<uint16_t>();
				reader.unk<uint16_t>();
				break;
			case excessive_kb:
				reader.unk<uint8_t>();
				reader.unk<uint16_t>();
				reader.unk<uint32_t>();
				break;
			case unk2:
				reader.unk<uint8_t>();
				reader.unk<uint32_t>();
				reader.unk<uint32_t>();
				break;
			case normal_movement:
			case normal_movement2:
				x = reader.get<game_coord>();
				y = reader.get<game_coord>();
				reader.unk<uint32_t>();
				foothold = reader.get<game_foothold_id>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				break;
			case jump:
			case jump_kb:
			case flash_jump:
			case recoil_shot:
				x = reader.get<game_coord>();
				y = reader.get<game_coord>();
				stance = reader.get<int8_t>();
				foothold = reader.get<game_foothold_id>();
				break;
			case jump_down:
				x = reader.get<game_coord>();
				y = reader.get<game_coord>();
				reader.unk<uint16_t>();
				reader.unk<uint32_t>();
				foothold = reader.get<game_foothold_id>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				break;
			case chair:
				x = reader.get<game_coord>();
				y = reader.get<game_coord>();
				foothold = reader.get<game_foothold_id>();
				stance = reader.get<int8_t>();
				reader.unk<uint16_t>();
				break;
			case unk1:
			case teleport:
			case assaulter:
			case assassinate:
			case rush:
				x = reader.get<game_coord>();
				y = reader.get<game_coord>();
				reader.unk<uint32_t>();
				stance = reader.get<int8_t>();
				break;
			default:
				// This is a tough call
				// Might be a packet processing issue, might be hacking
				break_loop = true;
				channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &str) {
					str << "New type of movement: 0x" << std::hex << static_cast<int16_t>(type);
				});
				break;
		}

		if (break_loop) {
			break;
		}
	}

	point pos{x, y};
	life->reset_movement(foothold, pos, stance);
	return pos;
}

}
}