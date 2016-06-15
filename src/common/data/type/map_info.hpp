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

#include "common/data/type/field_limit.hpp"
#include "common/data/type/map_link_info.hpp"
#include "common/rect.hpp"
#include "common/types.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace data {
		namespace type {
			struct map_info {
				bool clock = false;
				bool town = false;
				bool swim = false;
				bool fly = false;
				bool everlast = false;
				bool no_leader_pass = false;
				bool shop = false;
				bool scroll_disable = false;
				bool shuffle_reactors = false;
				bool force_map_equip = false;
				int8_t continent = -1;
				int8_t regen_rate = 0;
				int8_t ship_kind = -1;
				game_player_level min_level = 0;
				uint8_t regular_hp_decrease = 0;
				game_map_id return_map = 0;
				game_map_id forced_return = 0;
				game_map_id link = 0;
				game_map_id id = 0;
				int32_t time_limit = 0;
				game_item_id protect_item = 0;
				game_damage damage_per_second = 0;
				double spawn_rate = 0.;
				double traction = 0.;
				string default_music;
				string shuffle_name;
				string message;
				rect dimensions;
				field_limit limitations;
				ref_ptr<const map_link_info> link_info;
			};
		}
	}
}