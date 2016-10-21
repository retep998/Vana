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

auto movement_handler::read_movement(movable_life *life, packet_reader &reader, point* original_position) -> const std::list<movement_element> {
	game_foothold_id foothold = 0;
	int8_t stance = 0;

	game_coord x = reader.get<game_coord>();
	game_coord y = reader.get<game_coord>();

	if (original_position != nullptr) {
		*original_position = {x, y};
	}

	uint8_t movement_count = reader.get<uint8_t>();

	std::list<movement_element> move_path;

	for (uint8_t i = 0; i < movement_count; ++i) {
		movement_types type = (movement_types)reader.get<int8_t>();
		movement_element elem;
		elem.type = type;

		switch (type) {
		case movement_types::normal_movement:
		case movement_types::normal_movement2:
		case movement_types::jump_down:
		case movement_types::wings_falling:
			elem.x = reader.get<game_coord>();
			elem.y = reader.get<game_coord>();
			elem.x_velocity = reader.get<int16_t>();
			elem.y_velocity = reader.get<int16_t>();
			elem.foothold = reader.get<game_foothold_id>();
				
			if (type == movement_types::jump_down) {
				elem.fall_start = reader.get<int16_t>();
			}

			stance = elem.stance = reader.get<int8_t>();
			elem.time_elapsed = reader.get<int16_t>();
			break;
		case movement_types::jump:
		case movement_types::jump_kb:
		case movement_types::flash_jump:
		case movement_types::recoil_shot:
		case movement_types::wings:
		case movement_types::unk3:
		case movement_types::excessive_kb:
			elem.x = x;
			elem.y = y;
			elem.x_velocity = reader.get<int16_t>();
			elem.y_velocity = reader.get<int16_t>();
			stance = elem.stance = reader.get<int8_t>();
			elem.time_elapsed = reader.get<int16_t>();
			break;
		case movement_types::immediate:
		case movement_types::teleport:
		case movement_types::assaulter:
		case movement_types::assassinate:
		case movement_types::rush:
		case movement_types::chair:
			elem.x = reader.get<game_coord>();
			elem.y = reader.get<game_coord>();
			elem.foothold = reader.get<game_foothold_id>();
			stance = elem.stance = reader.get<int8_t>();
			elem.time_elapsed = reader.get<int16_t>();
			break;
		case movement_types::unk2:
			elem.x = x;
			elem.y = y;
			elem.x_velocity = reader.get<int16_t>();
			elem.y_velocity = reader.get<int16_t>();
			elem.fall_start = reader.get<int16_t>();
			break;
		case movement_types::falling:
			elem.stat = reader.get<int8_t>();
			break;
		default:
			stance = elem.stance = reader.get<int8_t>();
			elem.time_elapsed = reader.get<int16_t>();
			break;
		}
		x = elem.x;
		y = elem.y;
		move_path.push_back(elem);
	}

	uint8_t keypad_states = reader.get<uint8_t>();
	for (uint8_t i = 0; i < keypad_states; i++) {
		if ((i % 2) == 0) {
			reader.unk<uint8_t>();
		}
	}
	
	// Rectangle for bounds checking. Not used by us.
	reader.get<int16_t>(); // left
	reader.get<int16_t>(); // top
	reader.get<int16_t>(); // right
	reader.get<int16_t>(); // bottom

	point final_pos = {x, y};
	life->reset_movement(foothold, final_pos, stance);
	return move_path;
}

auto movement_handler::write_movement(packet_builder &builder, const point &original_position, const std::list<movement_element> &move_path) -> void {
	builder.add<point>(original_position);
	
	builder.add<uint8_t>((uint8_t)move_path.size());
	
	for (auto &elem : move_path) {
		movement_types type = elem.type;
		builder.add<int8_t>((int8_t)type);

		switch (type) {
		case movement_types::normal_movement:
		case movement_types::normal_movement2:
		case movement_types::jump_down:
		case movement_types::wings_falling:
			builder
				.add<game_coord>(elem.x)
				.add<game_coord>(elem.y)
				.add<int16_t>(elem.x_velocity)
				.add<int16_t>(elem.y_velocity)
				.add<game_foothold_id>(elem.foothold);

			if (type == movement_types::jump_down) {
				builder.add<int16_t>(elem.fall_start);
			}

			builder
				.add<int8_t>(elem.stance)
				.add<int16_t>(elem.time_elapsed);

			break;
		case movement_types::jump:
		case movement_types::jump_kb:
		case movement_types::flash_jump:
		case movement_types::recoil_shot:
		case movement_types::wings:
		case movement_types::unk3:
		case movement_types::excessive_kb:
			builder
				.add<int16_t>(elem.x_velocity)
				.add<int16_t>(elem.y_velocity)
				.add<int8_t>(elem.stance)
				.add<int16_t>(elem.time_elapsed);
			break;
		case movement_types::immediate:
		case movement_types::teleport:
		case movement_types::assaulter:
		case movement_types::assassinate:
		case movement_types::rush:
		case movement_types::chair:
			builder
				.add<game_coord>(elem.x)
				.add<game_coord>(elem.y)
				.add<game_foothold_id>(elem.foothold)
				.add<int8_t>(elem.stance)
				.add<int16_t>(elem.time_elapsed);
			break;
		case movement_types::unk2:
			builder
				.add<int16_t>(elem.x_velocity)
				.add<int16_t>(elem.y_velocity)
				.add<int16_t>(elem.fall_start);
			break;
		case movement_types::falling:
			builder.add<int8_t>(elem.stat);
			break;
		default:
			builder
				.add<int8_t>(elem.stance)
				.add<int16_t>(elem.time_elapsed);
			break;
		}
	}
}

}
}