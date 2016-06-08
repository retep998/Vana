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

#include "common/file_time.hpp"
#include "common/types.hpp"
#include <limits>

namespace vana {
	namespace constant {
		namespace item {
			namespace type {
				enum item_type : int16_t {
					armor_helm = 100,
					armor_face = 101,
					armor_eye = 102,
					armor_earring = 103,
					armor_top = 104,
					armor_overall = 105,
					armor_bottom = 106,
					armor_shoe = 107,
					armor_glove = 108,
					armor_shield = 109,
					armor_cape = 110,
					armor_ring = 111,
					armor_pendant = 112,
					medal = 114,
					weapon_1h_sword = 130,
					weapon_1h_axe = 131,
					weapon_1h_mace = 132,
					weapon_dagger = 133,
					weapon_wand = 137,
					weapon_staff = 138,
					weapon_2h_sword = 140,
					weapon_2h_axe = 141,
					weapon_2h_mace = 142,
					weapon_spear = 143,
					weapon_polearm = 144,
					weapon_bow = 145,
					weapon_crossbow = 146,
					weapon_claw = 147,
					weapon_knuckle = 148,
					weapon_gun = 149,
					mount = 190,
					item_arrow = 206,
					item_star = 207,
					item_bullet = 233,
					item_monster_card = 238,
					weather_cash = 512,
					cash_pet_food = 524
				};
			}
		}
	}
}