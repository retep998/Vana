/*
Copyright (C) 2008 Vana Development Team

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
#ifndef PLAYERSTORAGE_H
#define PLAYERSTORAGE_H

#include <vector>
#include "PlayerInventory.h"

using std::vector;

class Player;
class PlayerStorage;

class PlayerStorage {
public:
	PlayerStorage(Player *player, char slots, int mesos);
	char getSlots() {
		return this->slots;
	}
	void setSlots(char slots);
	void addItem(Item *item);
	Item * getItem(char slot) {
		if (slot < (char) items.size())
			return items[slot];
		return 0;
	}
	char getNumItems() {
		return items.size();
	}
	char getNumItems(char inv);
	void takeItem(char slot) {
		items.erase(items.begin() + slot);
	}
	void setMesos(int mesos) {
		this->mesos = mesos;
	}
	void changeMesos(int mesos);
	int getMesos() {
		return mesos;
	}
	bool isFull() {
		return ((char) items.size() == slots);
	}
private:
	Player *player;
	char slots;
	vector <Item *> items;
	int mesos;
};

#endif
