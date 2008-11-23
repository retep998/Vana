/*
Copyright (C) 2008 Vana Development Team

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

#define GETINVENTORY(itemid) static_cast<uint8_t>(itemid/1000000)
#define ISARROW(x) (x/10000 == 206)
#define ISSTAR(x) (x/10000 == 207)
#define ISBULLET(x) (x/10000 == 233) // Pirate's bullets and capsules
#define ISRECHARGEABLE(x) (ISSTAR(x) || ISBULLET(x))
#define ISEQUIP(x) (GETINVENTORY(x) == 1)
#define ISPET(x) (x >= 5000000 && x <= 5000053)
#define GETWEAPONTYPE(x) (x/10000)
#define ISOVERALL(itemid) (itemid/10000 == 105)
#define IS2HWEAPON(itemid) (itemid/100000 == 14)

#include "Types.h"
#include <unordered_map>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

class Player;
class PacketReader;
struct Item;

enum {
	WEAPON_1H_SWORD = 130,
	WEAPON_1H_AXE = 131,
	WEAPON_1H_MACE = 132,
	WEAPON_DAGGER = 133,
	WEAPON_WAND = 137,
	WEAPON_STAFF = 138,
	WEAPON_2H_SWORD = 140,
	WEAPON_2H_AXE = 141,
	WEAPON_2H_MACE = 142,
	WEAPON_SPEAR = 143,
	WEAPON_POLEARM = 144,
	WEAPON_BOW = 145,
	WEAPON_CROSSBOW = 146,
	WEAPON_CLAW = 147,
	WEAPON_KNUCKLE = 148,
	WEAPON_GUN = 149
};

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
	void stopChair(Player *player, PacketReader &packet);
	void useSummonBag(Player *player, PacketReader &packet);
	void useReturnScroll(Player *player, PacketReader &packet);
	void useScroll(Player *player, PacketReader &packet);
	void useCashItem(Player *player, PacketReader &packet);
};

#endif
