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
#ifndef PLAYERBUDDYLIST_H
#define PLAYERBUDDYLIST_H

#include "Types.h"
#include <unordered_map>
#include <string>

using std::string;
using std::tr1::unordered_map;

class Player;
namespace mysqlpp {
	class Row;
}

class PlayerBuddyList {
public:
	PlayerBuddyList(Player *player);
	void add(int32_t charid);
	bool add(const string &name);
	void remove(int32_t charid);
private:
	void add(const mysqlpp::Row &row);

	unordered_map<int32_t, string> buddies;
	Player *player;
};

#endif
