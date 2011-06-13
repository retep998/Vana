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

#include "MovableLife.h"
#include "Pos.h"
#include "Types.h"
#include <string>

using std::string;

class Item;
class Player;

class Pet : public MovableLife {
public:
	Pet(Player *player, Item *item);
	Pet(Player *player, Item *item, int8_t index, const string &name, int8_t level, int16_t closeness, int8_t fullness, int8_t inventorySlot);

	void setIndex(int8_t index) { m_index = index; }
	void setInventorySlot(int8_t slot) { m_inventorySlot = slot; }

	void setName(const string &name);
	void modifyFullness(int8_t offset, bool sendPacket = true);
	void addCloseness(int16_t amount);

	int8_t getIndex() const { return m_index; }
	int8_t getLevel() const { return m_level; }
	int8_t getInventorySlot() const { return m_inventorySlot; }
	int8_t getFullness() const { return m_fullness; }
	int16_t getCloseness() const { return m_closeness; }
	int32_t getItemId() const { return m_itemId; }
	int64_t getId() const { return m_id; }
	bool isSummoned() const { return m_index != -1; }
	string getName() { return m_name; }
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }
	bool hasNameTag() const;
	bool hasQuoteItem() const;

	void startTimer();
private:
	int8_t m_index;
	int8_t m_level;
	int8_t m_fullness;
	int8_t m_inventorySlot;
	int16_t m_closeness;
	int32_t m_itemId;
	int64_t m_id;
	string m_name;
	Player *m_player;

	void levelUp();
};