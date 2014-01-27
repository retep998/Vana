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

#include "Types.hpp"

class Item;
class Player;
class PacketReader;

namespace InventoryHandler {
	auto itemMove(Player *player, PacketReader &packet) -> void;
	auto dropItem(Player *player, PacketReader &packet, Item *item, int16_t slot, int8_t inv) -> void;
	auto moveItem(Player *player, PacketReader &packet) -> void;
	auto useItem(Player *player, PacketReader &packet) -> void;
	auto cancelItem(Player *player, PacketReader &packet) -> void;
	auto useSkillbook(Player *player, PacketReader &packet) -> void;
	auto useChair(Player *player, PacketReader &packet) -> void;
	auto useItemEffect(Player *player, PacketReader &packet) -> void;
	auto handleChair(Player *player, PacketReader &packet) -> void;
	auto useSummonBag(Player *player, PacketReader &packet) -> void;
	auto useReturnScroll(Player *player, PacketReader &packet) -> void;
	auto useScroll(Player *player, PacketReader &packet) -> void;
	auto useCashItem(Player *player, PacketReader &packet) -> void;
	auto handleRockFunctions(Player *player, PacketReader &packet) -> void;
	auto handleRockTeleport(Player *player, int32_t itemId, PacketReader &packet) -> bool;
	auto handleHammerTime(Player *player) -> void;
	auto handleRewardItem(Player *player, PacketReader &packet) -> void;
	auto handleScriptItem(Player *player, PacketReader &packet) -> void;
}