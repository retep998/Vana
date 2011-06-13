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
#include <boost/utility.hpp>
#include <vector>

using std::vector;

class Item;
class Player;

class PlayerStorage : boost::noncopyable {
public:
	PlayerStorage(Player *player);
	~PlayerStorage();

	void setSlots(int8_t slots);
	void addItem(Item *item);
	void takeItem(int8_t slot);
	void setMesos(int32_t mesos) { m_mesos = mesos; }
	void changeMesos(int32_t mesos);

	int8_t getSlots() const { return m_slots; }
	int8_t getNumItems() const { return m_items.size(); }
	int8_t getNumItems(int8_t m_inv);
	int32_t getMesos() const { return m_mesos; }
	bool isFull() const { return ((int8_t) m_items.size() == m_slots); }
	Item * getItem(int8_t slot) const {
		if (slot < (int8_t) m_items.size()) {
			return m_items[slot];
		}
		return 0;
	}

	void load();
	void save();
private:
	Player *m_player;
	vector<Item *> m_items;
	int8_t m_slots;
	int32_t m_mesos;
};