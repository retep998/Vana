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

#include "Common/ItemConstants.hpp"
#include "Common/Types.hpp"

namespace Vana {
	class Item;
	class PacketReader;

	namespace ChannelServer {
		class Player;

		namespace Inventory {
			auto addItem(ref_ptr_t<Player> player, Item *item, bool fromDrop = false) -> slot_qty_t;
			auto addNewItem(ref_ptr_t<Player> player, item_id_t itemId, slot_qty_t amount, Items::StatVariance variancePolicy = Items::StatVariance::None) -> void;
			auto takeItem(ref_ptr_t<Player> player, item_id_t itemId, slot_qty_t howMany) -> void;
			auto useItem(ref_ptr_t<Player> player, item_id_t itemId) -> void;
			auto takeItemSlot(ref_ptr_t<Player> player, inventory_t inv, inventory_slot_t slot, slot_qty_t amount, bool takeStar = false, bool overrideGmBenefits = false) -> void;
		}
	}
}