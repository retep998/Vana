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
		namespace mob_skill {
		enum : game_mob_skill_id {
			weapon_attack_up = 100,
			weapon_attack_up_aoe = 110,
			magic_attack_up = 101,
			magic_attack_up_aoe = 111,
			weapon_defense_up = 102,
			weapon_defense_up_aoe = 112,
			magic_defense_up = 103,
			magic_defense_up_aoe = 113,
			heal_aoe = 114,
			speed_up_aoe = 115,
			seal = 120,
			darkness = 121,
			weakness = 122,
			stun = 123,
			curse = 124,
			poison = 125,
			slow = 126,
			dispel = 127,
			seduce = 128,
			send_to_town = 129,
			poison_mist = 131,
			crazy_skull = 132,
			zombify = 133,
			weapon_immunity = 140,
			magic_immunity = 141,
			armor_skill = 142,
			weapon_damage_reflect = 143,
			magic_damage_reflect = 144,
			any_damage_reflect = 145,
			mc_weapon_attack_up = 150,
			mc_magic_attack_up = 151,
			mc_weapon_defense_up = 152,
			mc_magic_defense_up = 153,
			mc_accuracy_up = 154,
			mc_avoid_up = 155,
			mc_speed_up = 156,
			mc_seal = 157, // Not actually used in Monster Carnival
			summon = 200
		};
	}
	}
}