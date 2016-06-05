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

#include "Common/Types.hpp"
#include <vector>

namespace vana {
	struct equip_info {
		bool trade_block_on_equip = false;
		int8_t slots = 0;
		int8_t attack_speed = 0;
		int8_t healing = 0;
		uint8_t taming_mob = 0;
		uint8_t ice_damage = 0;
		uint8_t fire_damage = 0;
		uint8_t lightning_damage = 0;
		uint8_t poison_damage = 0;
		uint8_t elemental_default = 0;
		game_stat str_bonus = 0;
		game_stat dex_bonus = 0;
		game_stat int_bonus = 0;
		game_stat luk_bonus = 0;
		game_health hp_bonus = 0;
		game_health mp_bonus = 0;
		game_stat watk_bonus = 0;
		game_stat matk_bonus = 0;
		game_stat wdef_bonus = 0;
		game_stat mdef_bonus = 0;
		game_stat acc_bonus = 0;
		game_stat avo_bonus = 0;
		game_stat hands_bonus = 0;
		game_stat jump_bonus = 0;
		game_stat speed_bonus = 0;
		game_stat required_str = 0;
		game_stat required_dex = 0;
		game_stat required_int = 0;
		game_stat required_luk = 0;
		game_fame required_fame = 0;
		uint64_t valid_slots = 0;
		double traction = 0.;
		vector<int8_t> valid_jobs;
	};
}