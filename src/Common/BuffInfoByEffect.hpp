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

#include "Common/BuffInfo.hpp"
#include "Common/Types.hpp"

namespace vana {
	struct buff_info_by_effect {
		buff_info physical_attack;
		buff_info physical_defense;
		buff_info magic_attack;
		buff_info magic_defense;
		buff_info accuracy;
		buff_info avoid;
		buff_info craft;
		buff_info speed;
		buff_info jump;
		buff_info magic_guard;
		buff_info dark_sight;
		buff_info booster;
		buff_info power_guard;
		buff_info hyper_body_hp;
		buff_info hyper_body_mp;
		buff_info invincible;
		buff_info soul_arrow;
		buff_info stun;
		buff_info poison;
		buff_info seal;
		buff_info darkness;
		buff_info combo;
		buff_info charge;
		buff_info timed_hurt;
		buff_info holy_symbol;
		buff_info meso_up;
		buff_info shadow_partner;
		buff_info pickpocket;
		buff_info meso_guard;
		buff_info weakness;
		buff_info curse;
		buff_info slow;
		buff_info morph;
		buff_info timed_heal;
		buff_info maple_warrior;
		buff_info power_stance;
		buff_info sharp_eyes;
		buff_info mana_reflection;
		buff_info seduce;
		buff_info shadow_stars;
		buff_info infinity;
		buff_info holy_shield;
		buff_info hamstring;
		buff_info blind;
		buff_info concentrate;
		buff_info zombify;
		buff_info echo;
		buff_info crazy_skull;
		buff_info ignore_weapon_immunity;
		buff_info ignore_magic_immunity;
		buff_info spark;
		buff_info dawn_warrior_final_attack;
		buff_info wind_walker_final_attack;
		buff_info elemental_reset;
		buff_info wind_walk;
		buff_info energy_charge;
		buff_info dash_speed;
		buff_info dash_jump;
		buff_info mount;
		buff_info speed_infusion;
		buff_info homing_beacon;
	};
}