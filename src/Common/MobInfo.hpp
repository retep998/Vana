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

#include "Common/GameConstants.hpp"
#include "Common/Types.hpp"
#include <vector>

namespace vana {
	struct mob_info {
		bool boss = false;
		bool can_freeze = false;
		bool can_poison = false;
		bool undead = false;
		bool flying = false;
		bool friendly = false;
		bool public_reward = false;
		bool explosive_reward = false;
		bool invincible = false;
		bool damageable = true;
		bool can_do_bump_damage = true;
		bool auto_aggro = false;
		bool keep_corpse = false;
		bool only_normal_attacks = false;
		int8_t carnival_points = 0;
		int8_t hp_color = 0;
		int8_t hp_background_color = 0;
		uint8_t skill_count = 0;
		int16_t w_atk = 0;
		int16_t w_def = 0;
		int16_t m_atk = 0;
		int16_t m_def = 0;
		int16_t acc = 0;
		int16_t avo = 0;
		int16_t speed = 0;
		int16_t chase_speed = 0;
		int16_t summon_type = 0;
		uint16_t level = 0;
		int32_t self_destruction = 0;
		game_item_id buff = 0;
		game_mob_id link = 0;
		int32_t remove_after = 0;
		int32_t knockback = 0;
		game_damage fixed_damage = 0;
		game_skill_id damaged_by_skill = 0;
		game_mob_id damaged_by_mob = 0;
		uint32_t hp = 0;
		uint32_t mp = 0;
		game_experience exp = 0;
		uint32_t hp_recovery = 0;
		uint32_t mp_recovery = 0;
		double traction = 0.;
		mob_elemental_attribute ice_attr = mob_elemental_attribute::normal;
		mob_elemental_attribute fire_attr = mob_elemental_attribute::normal;
		mob_elemental_attribute poison_attr = mob_elemental_attribute::normal;
		mob_elemental_attribute lightning_attr = mob_elemental_attribute::normal;
		mob_elemental_attribute holy_attr = mob_elemental_attribute::normal;
		mob_elemental_attribute non_elem_attr = mob_elemental_attribute::normal;
		vector<game_mob_id> summon;

		auto has_hp_bar() const -> bool { return hp_color > 0; }
	};
}