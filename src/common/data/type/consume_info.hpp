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

#include "common/data/type/card_map_range_info.hpp"
#include "common/data/type/morph_chance_info.hpp"
#include "common/types.hpp"
#include <vector>

namespace vana {
	namespace data {
		namespace type {
			struct consume_info {
				bool auto_consume = false;
				bool ignore_wdef = false;
				bool ignore_mdef = false;
				bool party = false;
				bool mouse_cancel = true;
				bool ignore_continent = false;
				bool ghost = false;
				bool barrier = false;
				bool override_traction = false;
				bool prevent_drown = false;
				bool prevent_freeze = false;
				bool meso_up = false;
				bool drop_up = false;
				bool party_drop_up = false;
				uint8_t effect = 0;
				uint8_t dec_hunger = 0;
				uint8_t dec_fatigue = 0;
				uint8_t cp = 0;
				game_health hp = 0;
				game_health mp = 0;
				int16_t hp_rate = 0;
				int16_t mp_rate = 0;
				game_stat watk = 0;
				game_stat matk = 0;
				game_stat avo = 0;
				game_stat acc = 0;
				game_stat wdef = 0;
				game_stat mdef = 0;
				game_stat speed = 0;
				game_stat jump = 0;
				int16_t fire_resist = 0;
				int16_t ice_resist = 0;
				int16_t lightning_resist = 0;
				int16_t poison_resist = 0;
				int16_t curse_def = 0;
				int16_t stun_def = 0;
				int16_t weakness_def = 0;
				int16_t darkness_def = 0;
				int16_t seal_def = 0;
				int16_t drop_up_item_range = 0;
				uint16_t chance = 0;
				game_item_id drop_up_item = 0;
				game_map_id move_to = 0;
				game_item_id item_id = 0;
				int32_t ailment = 0;
				seconds buff_time = seconds{0};
				vector<morph_chance_info> morphs;
				vector<card_map_range_info> map_ranges;
			};
		}
	}
}