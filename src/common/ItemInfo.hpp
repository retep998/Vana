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

#include "common/Types.hpp"
#include <string>

namespace vana {
	struct item_info {
		bool time_limited = false;
		bool no_sale = false;
		bool karma_scissors = false;
		bool expire_on_logout = false;
		bool block_pickup = false;
		bool no_trade = false;
		bool quest = false;
		bool cash = false;
		game_player_level min_level = 0;
		game_player_level max_level = 0;
		game_skill_level maker_level = 0;
		game_slot_qty max_slot = 0;
		game_mesos price = 0;
		int32_t max_obtainable = 0;
		game_experience exp = 0;
		game_npc_id npc = 0;
		game_mesos mesos = 0;
		string name;
	};
}