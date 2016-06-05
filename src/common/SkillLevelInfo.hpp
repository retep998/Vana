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

#include "common/Rect.hpp"
#include "common/Types.hpp"

namespace vana {
	struct skill_level_info {
		int8_t mob_count = 0;
		int8_t hit_count = 0;
		int8_t mastery = 0;
		uint8_t critical_damage = 0;
		game_skill_level level = 0;
		game_health mp = 0;
		game_health hp = 0;
		game_slot_qty item_count = 0;
		game_slot_qty bullet_consume = 0;
		int16_t money_consume = 0;
		int16_t x = 0;
		int16_t y = 0;
		game_stat speed = 0;
		game_stat jump = 0;
		game_stat str = 0;
		game_stat w_atk = 0;
		game_stat w_def = 0;
		game_stat m_atk = 0;
		game_stat m_def = 0;
		game_stat acc = 0;
		game_stat avo = 0;
		int16_t morph = 0;
		int16_t damage = 0;
		int16_t range = 0;
		uint16_t hp_prop = 0;
		uint16_t mp_prop = 0;
		uint16_t prop = 0;
		game_damage fixed_damage = 0;
		game_item_id item = 0;
		game_item_id optional_item = 0;
		seconds buff_time = seconds{0};
		seconds cool_time = seconds{0};
		rect dimensions;
	};
}