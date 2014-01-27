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

#include "Item.hpp"
#include "ItemConstants.hpp"
#include "Types.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

class PacketCreator;
class Player;

class PlayerInventory {
	NONCOPYABLE(PlayerInventory);
	NO_DEFAULT_CONSTRUCTOR(PlayerInventory);
public:
	PlayerInventory(Player *player, const array_t<uint8_t, Inventories::InventoryCount> &maxSlots, int32_t mesos);
	~PlayerInventory();

	auto load() -> void;
	auto save() -> void;

	auto connectData(PacketCreator &packet) -> void;
	auto addEquippedPacket(PacketCreator &packet) -> void;
	auto rockPacket(PacketCreator &packet) -> void;
	auto wishListPacket(PacketCreator &packet) -> void;

	auto setMesos(int32_t mesos, bool sendPacket = false) -> void;
	auto modifyMesos(int32_t mod, bool sendPacket = false) -> bool;
	auto addMaxSlots(int8_t inventory, int8_t rows) -> void;
	auto addItem(int8_t inv, int16_t slot, Item *item, bool sendPacketLoading = false) -> void;
	auto deleteItem(int8_t inv, int16_t slot, bool updateAmount = true) -> void;
	auto setItem(int8_t inv, int16_t slot, Item *item) -> void;
	auto changeItemAmount(int32_t itemId, int16_t amount) -> void { m_itemAmounts[itemId] += amount; }
	auto setAutoHpPot(int32_t id) -> void { m_autoHpPotId = id; }
	auto setAutoMpPot(int32_t id) -> void { m_autoMpPotId = id; }
	auto swapItems(int8_t inventory, int16_t slot1, int16_t slot2) -> void;
	auto destroyEquippedItem(int32_t itemId) -> void;

	auto getMaxSlots(int8_t inv) const -> uint8_t { return m_maxSlots[inv - 1]; }
	auto getMesos() const -> int32_t { return m_mesos; }
	auto getAutoHpPot() const -> int32_t { return m_autoHpPotId; }
	auto getAutoMpPot() const -> int32_t { return m_autoMpPotId; }

	auto getItemAmountBySlot(int8_t inv, int16_t slot) -> int16_t;
	auto getItemAmount(int32_t itemId) -> uint16_t;
	auto getEquippedId(int16_t slot, bool cash = false) -> int32_t;
	auto getItem(int8_t inv, int16_t slot) -> Item *;
	auto isEquippedItem(int32_t itemId) -> bool;

	auto hasOpenSlotsFor(int32_t itemId, int16_t amount, bool canStack = false) -> bool;
	auto getOpenSlotsNum(int8_t inv) -> int16_t;
	auto doShadowStars() -> int32_t;

	auto isHammering() const -> bool { return m_hammer != -1; }
	auto getHammerSlot() const -> int16_t { return m_hammer; }
	auto setHammerSlot(int16_t hammer) -> void { m_hammer = hammer; }

	auto addRockMap(int32_t mapId, int8_t type) -> void;
	auto delRockMap(int32_t mapId, int8_t type) -> void;
	auto ensureRockDestination(int32_t mapId) -> bool;

	auto addWishListItem(int32_t itemId) -> void;
	auto checkExpiredItems() -> void;
private:
	auto addEquipped(int16_t slot, int32_t itemId) -> void;

	int16_t m_hammer = -1;
	int32_t m_mesos = 0;
	int32_t m_autoHpPotId = 0;
	int32_t m_autoMpPotId = 0;
	Player *m_player = nullptr;
	array_t<uint8_t, Inventories::InventoryCount> m_maxSlots;
	array_t<array_t<int32_t, 2>, Inventories::EquippedSlots> m_equipped; // Separate sets of slots for regular items and cash items
	array_t<hash_map_t<int16_t, Item *>, Inventories::InventoryCount> m_items;
	vector_t<int32_t> m_vipLocations;
	vector_t<int32_t> m_rockLocations;
	vector_t<int32_t> m_wishlist;
	hash_map_t<int32_t, uint16_t> m_itemAmounts;
};