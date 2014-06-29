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

#include "MovableLife.hpp"
#include "Point.hpp"
#include "Types.hpp"
#include <soci.h>
#include <string>

class Item;
class Player;

class Pet : public MovableLife {
	NONCOPYABLE(Pet);
	NO_DEFAULT_CONSTRUCTOR(Pet);
public:
	Pet(Player *player, Item *item);
	Pet(Player *player, Item *item, const soci::row &row);

	auto summon(int8_t index) -> void { m_index = index; }
	auto desummon() -> void { m_index.reset(); }
	auto setInventorySlot(int8_t slot) -> void { m_inventorySlot = slot; }
	auto setName(const string_t &name) -> void;
	auto modifyFullness(int8_t offset, bool sendPacket = true) -> void;
	auto addCloseness(int16_t amount) -> void;

	auto getIndex() const -> opt_int8_t { return m_index; }
	auto getLevel() const -> int8_t { return m_level; }
	auto getInventorySlot() const -> int8_t { return m_inventorySlot; }
	auto getFullness() const -> int8_t { return m_fullness; }
	auto getCloseness() const -> int16_t { return m_closeness; }
	auto getItemId() const -> item_id_t { return m_itemId; }
	auto getId() const -> pet_id_t { return m_id; }
	auto isSummoned() const -> bool { return m_index.is_initialized(); }
	auto getName() -> string_t { return m_name; }
	auto getPos() const -> Point override { return Point{m_pos.x, m_pos.y - 1}; }
	auto hasNameTag() const -> bool;
	auto hasQuoteItem() const -> bool;

	auto startTimer() -> void;
private:
	auto initializePet(const soci::row &row) -> void;
	auto levelUp() -> void;

	opt_int8_t m_index;
	int8_t m_level = 1;
	int8_t m_fullness = Stats::MaxFullness;
	int8_t m_inventorySlot = 0;
	int16_t m_closeness = 0;
	item_id_t m_itemId = 0;
	pet_id_t m_id = 0;
	Item *m_item = nullptr;
	Player *m_player = nullptr;
	string_t m_name;
};