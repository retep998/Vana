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

#include "common/types.hpp"

namespace vana {
	namespace constant {
		namespace status_effect {
			namespace mob {
				const uint8_t max_venom_count = 3;

				enum mob_status : uint32_t {
					watk = 0x01,
					wdef = 0x02,
					matk = 0x04,
					mdef = 0x08,
					acc = 0x10,

					avoid = 0x20,
					speed = 0x40,
					stun = 0x80,
					freeze = 0x100,
					poison = 0x200,

					seal = 0x400,
					no_clue1 = 0x800,
					weapon_attack_up = 0x1000,
					weapon_defense_up = 0x2000,
					magic_attack_up = 0x4000,

					magic_defense_up = 0x8000,
					doom = 0x10000,
					shadow_web = 0x20000,
					weapon_immunity = 0x40000,
					magic_immunity = 0x80000,

					no_clue2 = 0x100000,
					no_clue3 = 0x200000,
					ninja_ambush = 0x400000,
					no_clue4 = 0x800000,
					venomous_weapon = 0x1000000,

					no_clue5 = 0x2000000,
					no_clue6 = 0x4000000,
					empty = 0x8000000, // All mobs have this when they spawn
					hypnotize = 0x10000000,
					weapon_damage_reflect = 0x20000000,

					magic_damage_reflect = 0x40000000,
					no_clue7 = 0x80000000 // Last bit you can use with 4 bytes
				};
			}

			namespace player {
				enum player_status : int16_t {
					curse = 0x01,
					weakness = 0x02,
					darkness = 0x04,
					seal = 0x08,
					poison = 0x10,
					stun = 0x20,
					slow = 0x40,
					seduce = 0x80,
					zombify = 0x100,
					crazy_skull = 0x200
				};
			}
		}
	}
}