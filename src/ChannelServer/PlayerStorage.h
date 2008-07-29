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
	void setSlots(char slots) {
		this->slots = slots;
	}
	void addItem(Item *item) {
		items.push_back(item);
	}
	Item * getItem(short slot) {
		return items[slot - 1];
	}
	vector <Item *> getItems() {
		return items;
	}
	void takeItem(short slot) {
		items.erase(items.begin() + (slot - 1));
	}
	void setMesos(int mesos) {
		this->mesos = mesos;
	}
	void changeMesos(int mesos);
	int getMesos() {
		return mesos;
	}
private:
	Player *player;
	char slots;
	vector <Item *> items;
	int mesos;
};

#endif
