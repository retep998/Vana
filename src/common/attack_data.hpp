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

#include "common/point_temp.hpp"
#include "common/types_temp.hpp"
#include <unordered_map>
#include <vector>

namespace vana {
	struct attack_data {
		bool is_meso_explosion = false;
		bool is_shadow_meso = false;
		bool is_charge_skill = false;
		bool is_piercing_arrow = false;
		bool is_heal = false;
		int8_t targets = 0;
		int8_t hits = 0;
		uint8_t display = 0;
		uint8_t weapon_speed = 0;
		uint8_t animation = 0;
		uint8_t weapon_class = 0;
		game_skill_level skill_level = 0;
		uint8_t portals = 0;
		game_inventory_slot star_pos = -1;
		game_inventory_slot cash_star_pos = -1;
		game_skill_id skill_id = 0;
		game_summon_id summon_id = 0;
		game_charge_time charge = 0;
		game_item_id star_id = 0;
		game_tick_count ticks = 0;
		int64_t total_damage = 0;
		point projectile_pos;
		point player_pos;
		hash_map<game_map_object, vector<game_damage>> damages;
	};
}