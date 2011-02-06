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
#include <boost/tr1/unordered_map.hpp>
#include <string>

using std::string;
using std::tr1::unordered_map;

class Item;
class Player;

class Pet : public MovableLife {
public:
	Pet(Player *player, Item *item);
	Pet(Player *player, Item *item, int8_t index, const string &name, int8_t level, int16_t closeness, int8_t fullness, int8_t inventorySlot);

	void setIndex(int8_t index) { this->index = index; }
	void setInventorySlot(int8_t slot) { this->inventorySlot = slot; }

	void setName(const string &name);
	void modifyFullness(int8_t offset, bool sendPacket = true);
	void addCloseness(int16_t amount);
	void setCashId(int64_t id) { cashId = id; }

	int8_t getIndex() const { return this->index; }
	int8_t getLevel() const { return this->level; }
	int8_t getInventorySlot() const { return this->inventorySlot; }
	int8_t getFullness() const { return this->fullness; }
	int16_t getCloseness() const { return this->closeness; }
	int32_t getId() const { return this->id; }
	int32_t getItemId() const { return this->itemid; }
	int64_t getCashId() const { return this->cashId; }
	bool isSummoned() const { return this->index != -1; }
	string getName() { return this->name; }
	Pos getPos() const { return Pos(m_pos.x, m_pos.y - 1); }

	void startTimer();
private:
	int8_t index;
	int8_t level;
	int8_t fullness;
	int8_t inventorySlot;
	int16_t closeness;
	int32_t id;
	int32_t itemid;
	int64_t cashId;
	string name;
	Player *player;

	void levelUp();
};
