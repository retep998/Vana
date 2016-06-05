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

#include "common_temp/Rect.hpp"
#include "common_temp/Types.hpp"
#include <vector>

namespace vana {
	struct mob_skill_level_info {
		int8_t summon_effect = 0;
		uint8_t mp = 0;
		uint8_t hp = 0;
		uint8_t count = 0;
		game_mob_skill_level level = 0;
		int16_t cooldown = 0;
		int16_t prop = 0;
		int16_t limit = 0;
		int32_t x = 0;
		int32_t y = 0;
		seconds time = seconds{0};
		rect dimensions;
		vector<game_mob_id> summons;
	};
}