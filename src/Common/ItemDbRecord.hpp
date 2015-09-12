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

#include "ItemDbInformation.hpp"
#include "Types.hpp"

namespace Vana {
	class Item;

	struct ItemDbRecord : ItemDbInformation {
		NO_DEFAULT_CONSTRUCTOR(ItemDbRecord);
	public:
		ItemDbRecord(inventory_slot_t slot, player_id_t charId, account_id_t userId, world_id_t worldId, const string_t &location, Item *item) :
			ItemDbInformation{slot, charId, userId, worldId, location},
			item{item}
		{
		}
		ItemDbRecord(const ItemDbInformation &info, Item *item) :
			ItemDbInformation{info.slot, info.charId, info.userId, info.worldId, info.location},
			item{item}
		{
		}

		Item *item;
	};
}