/*
Copyright (C) 2008-2011 Vana Development Team

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
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class PacketReader;
class Item;

namespace Inventory {
	int16_t addItem(Player *player, Item *item, bool is = false);
	void addNewItem(Player *player, int32_t itemid, int16_t amount);
	void takeItem(Player *player, int32_t itemid, uint16_t howmany);
	void useItem(Player *player, int32_t itemid);
	void takeItemSlot(Player *player, int8_t inv, int16_t slot, int16_t amount, bool takeStar = false);
	bool showShop(Player *player, int32_t shopid);
};
