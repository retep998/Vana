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

#include "Types.h"
#include <vector>

class Item;
class Player;

class PlayerStorage {
	NONCOPYABLE(PlayerStorage);
	NO_DEFAULT_CONSTRUCTOR(PlayerStorage);
public:
	PlayerStorage(Player *player);
	~PlayerStorage();

	auto setSlots(uint8_t slots) -> void;
	auto addItem(Item *item) -> void;
	auto takeItem(uint8_t slot) -> void;
	auto setMesos(int32_t mesos) -> void { m_mesos = mesos; }
	auto changeMesos(int32_t mesos) -> void;

	auto getSlots() const -> uint8_t { return m_slots; }
	auto getNumItems() const -> uint8_t { return m_items.size(); }
	auto getNumItems(uint8_t m_inv) -> uint8_t;
	auto getMesos() const -> int32_t { return m_mesos; }
	auto isFull() const -> bool { return m_items.size() == m_slots; }
	auto getItem(uint8_t slot) const -> Item * {
		if (slot < m_items.size()) {
			return m_items[slot];
		}
		return nullptr;
	}

	auto load() -> void;
	auto save() -> void;
private:
	uint8_t m_slots = 0;
	int32_t m_mesos = 0;
	int32_t m_charSlots = 0;
	vector_t<Item *> m_items;
	Player *m_player = nullptr;
};