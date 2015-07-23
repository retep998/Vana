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

#include "Types.hpp"

class Item;
class PacketReader;
class Player;

namespace InventoryHandler {
	auto itemMove(Player *player, PacketReader &reader) -> void;
	auto dropItem(Player *player, PacketReader &reader, Item *item, inventory_slot_t slot, inventory_t inv) -> void;
	auto moveItem(Player *player, PacketReader &reader) -> void;
	auto useItem(Player *player, PacketReader &reader) -> void;
	auto cancelItem(Player *player, PacketReader &reader) -> void;
	auto useSkillbook(Player *player, PacketReader &reader) -> void;
	auto useChair(Player *player, PacketReader &reader) -> void;
	auto useItemEffect(Player *player, PacketReader &reader) -> void;
	auto handleChair(Player *player, PacketReader &reader) -> void;
	auto useSummonBag(Player *player, PacketReader &reader) -> void;
	auto useReturnScroll(Player *player, PacketReader &reader) -> void;
	auto useScroll(Player *player, PacketReader &reader) -> void;
	auto useBuffItem(Player *player, PacketReader &reader) -> void;
	auto useCashItem(Player *player, PacketReader &reader) -> void;
	auto handleRockFunctions(Player *player, PacketReader &reader) -> void;
	auto handleRockTeleport(Player *player, item_id_t itemId, PacketReader &reader) -> bool;
	auto handleHammerTime(Player *player) -> void;
	auto handleRewardItem(Player *player, PacketReader &reader) -> void;
	auto handleScriptItem(Player *player, PacketReader &reader) -> void;
}