/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/Types.hpp"

namespace Vana {
	struct ItemDbInformation {
		NO_DEFAULT_CONSTRUCTOR(ItemDbInformation);
	public:
		ItemDbInformation(inventory_slot_t slot, player_id_t charId, account_id_t userId, world_id_t worldId, const string_t &location) :
			slot{slot},
			charId{charId},
			userId{userId},
			worldId{worldId},
			location{location}
		{
		}

		inventory_slot_t slot;
		player_id_t charId;
		account_id_t userId;
		world_id_t worldId;
		string_t location;
	};
}