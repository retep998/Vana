/*
Copyright (C) 2008-2016 Vana Development Team

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
	class Item;
	class PacketReader;

	namespace ChannelServer {
		class Player;

		namespace InventoryHandler {
			auto moveItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto dropItem(ref_ptr_t<Player> player, PacketReader &reader, Item *item, inventory_slot_t slot, inventory_t inv) -> void;
			auto useItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto cancelItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useSkillbook(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useChair(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useItemEffect(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleChair(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useSummonBag(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useReturnScroll(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useScroll(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useBuffItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useCashItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleRockFunctions(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleRockTeleport(ref_ptr_t<Player> player, item_id_t itemId, PacketReader &reader) -> bool;
			auto handleHammerTime(ref_ptr_t<Player> player) -> void;
			auto handleRewardItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleScriptItem(ref_ptr_t<Player> player, PacketReader &reader) -> void;
		}
	}
}