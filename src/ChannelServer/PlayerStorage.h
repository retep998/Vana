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

#include "Types.h"
#include <vector>

using std::vector;

class Item;
class Player;

class PlayerStorage {
public:
	PlayerStorage(Player *player);
	~PlayerStorage();

	void setSlots(int8_t slots);
	void addItem(Item *item);
	void takeItem(int8_t slot);
	void setMesos(int32_t mesos) { this->mesos = mesos; }
	void changeMesos(int32_t mesos);

	int8_t getSlots() const { return this->slots; }
	int8_t getNumItems() const { return items.size(); }
	int8_t getNumItems(int8_t inv);
	int32_t getMesos() const { return mesos; }
	bool isFull() const { return ((int8_t) items.size() == slots); }
	Item * getItem(int8_t slot) const {
		if (slot < (int8_t) items.size())
			return items[slot];
		return nullptr;
	}

	void load();
	void save();

	void changeNxCredit(int32_t val);
	void changeNxPrepaid(int32_t val);
	void changeMaplePoints(int32_t val);
	int32_t getNxCredit();
	int32_t getNxPrepaid();
	int32_t getMaplePoints();
private:
	Player *player;
	vector<Item *> items;
	int8_t slots;
	int32_t mesos;
};
