/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "Types.h"
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

class Item;
class PacketReader;
class Player;

namespace Inventory {
	int16_t addItem(Player *player, Item *item, bool fromDrop = false);
	void addNewItem(Player *player, int32_t itemId, int16_t amount);
	void takeItem(Player *player, int32_t itemId, uint16_t howMany);
	void useItem(Player *player, int32_t itemId);
	void takeItemSlot(Player *player, int8_t inv, int16_t slot, int16_t amount, bool takeStar = false);
}