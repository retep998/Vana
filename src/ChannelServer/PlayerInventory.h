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

#include "Item.h"
#include "ItemConstants.h"
#include "Types.h"
#include <boost/array.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::unordered_map;
using std::vector;

class Player;
class PacketCreator;

class PlayerInventory {
public:
	PlayerInventory(Player *player, const boost::array<uint8_t, Inventories::InventoryCount> &maxslots, int32_t mesos);
	~PlayerInventory();

	void load();
	void save();

	void connectData(PacketCreator &packet);
	void addEquippedPacket(PacketCreator &packet);
	void rockPacket(PacketCreator &packet);
	void wishListPacket(PacketCreator &packet);

	void setMesos(int32_t mesos, bool is = false);
	bool modifyMesos(int32_t mod, bool is = false);
	void addMaxSlots(int8_t inventory, int8_t rows);
	void addItem(int8_t inv, int16_t slot, Item *item, bool isLoading = false);
	void deleteItem(int8_t inv, int16_t slot, bool updateAmount = true);
	void setItem(int8_t inv, int16_t slot, Item *item);
	void changeItemAmount(int32_t itemid, int16_t amount) { m_itemamounts[itemid] += amount; }
	void setAutoHpPot(int32_t id) { m_autoHpPotId = id; }
	void setAutoMpPot(int32_t id) { m_autoMpPotId = id; }

	uint8_t getMaxSlots(int8_t inv) const { return m_maxslots[inv - 1]; }
	int32_t getMesos() const { return m_mesos; }
	int32_t getAutoHpPot() const { return m_autoHpPotId; }
	int32_t getAutoMpPot() const { return m_autoMpPotId; }

	int16_t getItemAmountBySlot(int8_t inv, int16_t slot);
	uint16_t getItemAmount(int32_t itemid);
	int32_t getEquippedId(int16_t slot, bool cash = false);
	Item * getItem(int8_t inv, int16_t slot);
	bool isEquippedItem(int32_t itemid);

	bool hasOpenSlotsFor(int32_t itemid, int16_t amount, bool canStack = false);
	int16_t getOpenSlotsNum(int8_t inv);

	int32_t doShadowStars();

	bool isHammering() const { return m_hammer != -1; }
	int16_t getHammerSlot() const { return m_hammer; }
	void setHammerSlot(int16_t hammer) { m_hammer = hammer; }

	void addRockMap(int32_t mapid, int8_t type);
	void delRockMap(int32_t mapid, int8_t type);
	bool ensureRockDestination(int32_t mapid);

	void addWishListItem(int32_t itemid);

	void checkExpiredItems();

	boost::array<boost::array<int32_t, 2>, Inventories::EquippedSlots> getEquippedData() { return m_equipped; }
private:
	typedef unordered_map<int16_t, Item *> ItemInventory;

	boost::array<uint8_t, Inventories::InventoryCount> m_maxslots;
	boost::array<boost::array<int32_t, 2>, Inventories::EquippedSlots> m_equipped; // One set of slots for non-cash items, the other for cash items.
	boost::array<ItemInventory, Inventories::InventoryCount> m_items;

	vector<int32_t> m_viplocations;
	vector<int32_t> m_rocklocations;
	vector<int32_t> m_wishlist;

	unordered_map<int32_t, uint16_t> m_itemamounts;
	int16_t m_hammer;
	int32_t m_mesos;
	int32_t m_autoHpPotId; // For checking the Auto HP/MP pot ID
	int32_t m_autoMpPotId;
	Player *m_player;

	void addEquipped(int16_t slot, int32_t itemid);
};
