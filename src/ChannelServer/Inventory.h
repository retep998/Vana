/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef INVENTORY_H
#define INVENTORY_H

#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class PacketReader;
struct Item;

namespace Inventory {
	void useShop(Player *player, PacketReader &packet);
	void useStorage(Player *player, PacketReader &packet);
	void itemMove(Player *player, PacketReader &packet);
	int16_t addItem(Player *player, Item *item, bool is = false);
	void addNewItem(Player *player, int32_t itemid, int16_t amount);
	void takeItem(Player *player, int32_t itemid, uint16_t howmany);
	void useItem(Player *player, PacketReader &packet);
	void useItem(Player *player, int32_t itemid);
	void cancelItem(Player *player, PacketReader &packet);
	void useSkillbook(Player *player, PacketReader &packet);
	void takeItemSlot(Player *player, int8_t inv, int16_t slot, int16_t amount, bool takeStar = false);
	void useChair(Player *player, PacketReader &packet);
	void useItemEffect(Player *player, PacketReader &packet);
	void handleChair(Player *player, PacketReader &packet);
	void useSummonBag(Player *player, PacketReader &packet);
	void useReturnScroll(Player *player, PacketReader &packet);
	void useScroll(Player *player, PacketReader &packet);
	void useCashItem(Player *player, PacketReader &packet);
	void handleRockFunctions(Player *player, PacketReader &packet);
	bool handleRockTeleport(Player *player, int8_t type, int32_t itemid, PacketReader &packet);
};

#endif
