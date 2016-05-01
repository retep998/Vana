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

#include "Common/Item.hpp"
#include "Common/ItemConstants.hpp"
#include "Common/Types.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	class Item;
	class PacketBuilder;

	namespace ChannelServer {
		class Player;

		class PlayerInventory {
			NONCOPYABLE(PlayerInventory);
			NO_DEFAULT_CONSTRUCTOR(PlayerInventory);
		public:
			PlayerInventory(Player *player, const array_t<inventory_slot_count_t, Inventories::InventoryCount> &maxSlots, mesos_t mesos);
			~PlayerInventory();

			auto load() -> void;
			auto save() -> void;

			auto connectPacket(PacketBuilder &builder) -> void;
			auto addEquippedPacket(PacketBuilder &builder) -> void;
			auto rockPacket(PacketBuilder &builder) -> void;
			auto wishlistInfoPacket(PacketBuilder &builder) -> void;

			auto setMesos(mesos_t mesos, bool sendPacket = false) -> void;
			auto modifyMesos(mesos_t mod, bool sendPacket = false) -> bool;
			auto addMaxSlots(inventory_t inventory, inventory_slot_count_t rows) -> void;
			auto addItem(inventory_t inv, inventory_slot_t slot, Item *item, bool sendPacketLoading = false) -> void;
			auto deleteItem(inventory_t inv, inventory_slot_t slot, bool updateAmount = true) -> void;
			auto setItem(inventory_t inv, inventory_slot_t slot, Item *item) -> void;
			auto changeItemAmount(item_id_t itemId, slot_qty_t amount) -> void { m_itemAmounts[itemId] += amount; }
			auto setAutoHpPot(item_id_t id) -> void { m_autoHpPotId = id; }
			auto setAutoMpPot(item_id_t id) -> void { m_autoMpPotId = id; }
			auto swapItems(inventory_t inventory, inventory_slot_t slot1, inventory_slot_t slot2) -> void;
			auto destroyEquippedItem(item_id_t itemId) -> void;

			auto getMaxSlots(inventory_t inv) const -> inventory_slot_count_t { return m_maxSlots[inv - 1]; }
			auto getMesos() const -> mesos_t { return m_mesos; }
			auto getAutoHpPot() const -> item_id_t { return m_autoHpPotId; }
			auto getAutoMpPot() const -> item_id_t { return m_autoMpPotId; }

			auto getItemAmountBySlot(inventory_t inv, inventory_slot_t slot) -> slot_qty_t;
			auto getItemAmount(item_id_t itemId) -> slot_qty_t;
			auto getEquippedId(inventory_slot_t slot, bool cash = false) -> item_id_t;
			auto getItem(inventory_t inv, inventory_slot_t slot) -> Item *;
			auto isEquippedItem(item_id_t itemId) -> bool;

			auto hasOpenSlotsFor(item_id_t itemId, slot_qty_t amount, bool canStack = false) -> bool;
			auto getOpenSlotsNum(inventory_t inv) -> inventory_slot_count_t;
			auto doShadowStars() -> item_id_t;

			auto isHammering() const -> bool { return m_hammer != -1; }
			auto getHammerSlot() const -> inventory_slot_t { return m_hammer; }
			auto setHammerSlot(inventory_slot_t hammer) -> void { m_hammer = hammer; }

			auto addRockMap(map_id_t mapId, int8_t type) -> void;
			auto delRockMap(map_id_t mapId, int8_t type) -> void;
			auto ensureRockDestination(map_id_t mapId) -> bool;

			auto addWishListItem(item_id_t itemId) -> void;
			auto checkExpiredItems() -> void;
		private:
			auto addEquipped(inventory_slot_t slot, item_id_t itemId) -> void;

			inventory_slot_t m_hammer = -1;
			mesos_t m_mesos = 0;
			item_id_t m_autoHpPotId = 0;
			item_id_t m_autoMpPotId = 0;
			Player *m_player = nullptr;
			array_t<inventory_slot_count_t, Inventories::InventoryCount> m_maxSlots;
			array_t<array_t<item_id_t, 2>, Inventories::EquippedSlots> m_equipped; // Separate sets of slots for regular items and cash items
			array_t<hash_map_t<inventory_slot_t, Item *>, Inventories::InventoryCount> m_items;
			vector_t<map_id_t> m_vipLocations;
			vector_t<map_id_t> m_rockLocations;
			vector_t<item_id_t> m_wishlist;
			hash_map_t<item_id_t, slot_qty_t> m_itemAmounts;
		};
	}
}