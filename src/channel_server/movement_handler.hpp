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
#pragma once

#include <list>

namespace vana {
	class packet_reader;
	struct point;

	namespace channel_server {
		class movable_life;

		namespace movement_handler {
			enum class movement_types : int8_t {
				normal_movement = 0,
				jump = 1,
				jump_kb = 2,
				immediate = 3, // GM F1 teleport
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
				wings_falling = 17,
				unk3 = 18
			};

			struct movement_element {
				movement_types type;
				int16_t x, x_velocity;
				int16_t y, y_velocity;
				int16_t fall_start;
				int16_t foothold;
				int16_t time_elapsed;
				int8_t stance, stat;
			};

			auto read_movement(movable_life *life, packet_reader &reader, point* original_position) -> const std::list<movement_element>;
			auto write_movement(packet_builder &builder, const point &original_position, const std::list<movement_element> &move_path) -> void;
		}
	}
}