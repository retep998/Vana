/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "ItemConstants.hpp"
#include "Types.hpp"

class Item;
class PacketReader;
class Player;

namespace Inventory {
	auto addItem(Player *player, Item *item, bool fromDrop = false) -> slot_qty_t;
	auto addNewItem(Player *player, item_id_t itemId, slot_qty_t amount, Items::StatVariance variancePolicy = Items::StatVariance::None) -> void;
	auto takeItem(Player *player, item_id_t itemId, slot_qty_t howMany) -> void;
	auto useItem(Player *player, item_id_t itemId) -> void;
	auto takeItemSlot(Player *player, inventory_t inv, inventory_slot_t slot, slot_qty_t amount, bool takeStar = false) -> void;
}