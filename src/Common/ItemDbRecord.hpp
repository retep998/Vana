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

#include "Common/ItemDbInformation.hpp"
#include "Common/Types.hpp"

namespace vana {
	class item;

	struct item_db_record : item_db_info {
		NO_DEFAULT_CONSTRUCTOR(item_db_record);
	public:
		item_db_record(game_inventory_slot slot, game_player_id char_id, game_account_id user_id, game_world_id world_id, const string &location, item *item) :
			item_db_info{slot, char_id, user_id, world_id, location},
			item{item}
		{
		}

		item_db_record(const item_db_info &info, item *item) :
			item_db_info{info.slot, info.char_id, info.user_id, info.world_id, info.location},
			item{item}
		{
		}

		item *item;
	};
}