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
#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::tr1::unordered_map;

class Player;
namespace mysqlpp {
	class Row;
}

class PlayerBuddyList {
public:
	struct Buddy;
	typedef std::tr1::shared_ptr<Buddy> BuddyPtr;

	PlayerBuddyList(Player *player);
	uint8_t add(const string &name);
	void remove(int32_t charid);

	BuddyPtr getBuddy(uint8_t pos) { return buddies[buddies_order[pos]]; }
	uint8_t size() const { return (uint8_t) buddies.size(); }
private:
	void add(const mysqlpp::Row &row);

	struct OppositeStatus {
		static const uint8_t registered = 0;
		static const uint8_t unregistered = 2;
	};

	vector<int32_t> buddies_order;
	unordered_map<int32_t, BuddyPtr> buddies;
	Player *player;
};

struct PlayerBuddyList::Buddy {
	int32_t charid;
	string name;
	uint8_t oppositeStatus;
	int32_t channel;
};