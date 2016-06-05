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

#include "Common/MobAttackType.hpp"
#include "Common/Types.hpp"

namespace vana {
	struct mob_attack_info {
		int8_t id = 0;
		game_mob_skill_level level = 0;
		game_mob_skill_id disease = 0;
		uint8_t mp_consume = 0;
		uint16_t mp_burn = 0;
		bool deadly_attack = false;
		mob_attack_type attack_type = mob_attack_type::normal;
	};
}