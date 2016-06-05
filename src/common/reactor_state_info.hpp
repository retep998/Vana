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

#include "common/rect.hpp"
#include "common/types.hpp"
#include <vector>

namespace vana {
	struct reactor_state_info {
		int8_t next_state = 0;
		int16_t type = 0;
		game_slot_qty item_quantity = 0;
		game_item_id item_id = 0;
		int32_t timeout = 0;
		rect dimensions;
		vector<game_skill_id> trigger_skills;
	};
}