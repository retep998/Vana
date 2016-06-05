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
	enum class key_map_action : int32_t {
		equipment_menu = 0,
		item_menu = 1,
		ability_menu = 2,
		skill_menu = 3,
		buddy_list = 4,
		world_map = 5,
		messenger = 6,
		minimap = 7,
		quest_menu = 8,
		set_key = 9,
		all_chat = 10,
		whisper_chat = 11,
		party_chat = 12,
		buddy_chat = 13,
		shortcut = 14,
		quick_slot = 15,
		expand_chat = 16,
		guild_list = 17,
		guild_chat = 18,
		party_list = 19,
		helper = 20,
		spouse_chat = 21,
		monster_book = 22,
		cash_shop = 23,
		alliance_chat = 24,
		party_search = 25,
		family_list = 26,

		pick_up = 50,
		sit = 51,
		attack = 52,
		jump = 53,
		npc_chat = 54,

		cockeyed = 100,
		happy = 101,
		sarcastic = 102,
		crying = 103,
		outraged = 104,
		shocked = 105,
		annoyed = 106,
	};
}