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
#include "BuddyList.h"
#include "Player.h"
#include "MySQLM.h"

BuddyList::BuddyList(Player *player) : player(player) {
	mysqlpp::Query query = db.query();
	query << "SELECT buddylist.buddy_charid as buddy_charid, characters.name as name FROM buddylist INNER JOIN characters ON buddylist.buddy_charid = characters.id WHERE buddylist.charid = " << mysqlpp::quote << player->getPlayerid();
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		add(res[i]);
	}
}

void BuddyList::add(int charid) {
	mysqlpp::Query query = db.query();
	query << "INSERT INTO buddylist (charid, buddy_charid) VALUES (" << mysqlpp::quote << player->getPlayerid() << ", " << mysqlpp::quote << charid << ")";
	mysqlpp::SimpleResult res = query.execute();
	
	query << "SELECT buddylist.buddy_charid as buddy_charid, characters.name as name FROM buddylist INNER JOIN characters ON buddylist.buddy_charid = characters.id WHERE buddylist.id = " << mysqlpp::quote << res.insert_id();
	mysqlpp::StoreQueryResult res2 = query.store();

	add(res2[0]);
}

bool BuddyList::add(const string &name) {
	mysqlpp::Query query = db.query();
	query << "SELECT id FROM characters WHERE name = " << mysqlpp::quote << name;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.size() == 0) { // Name does not exist
		return false;
	}

	add(res[0][0]);
	return true;
}

void BuddyList::remove(int charid) {
	mysqlpp::Query query = db.query();
	query << "DELETE FROM buddylist WHERE charid = " << mysqlpp::quote << player->getPlayerid() << " AND buddy_charid = " << mysqlpp::quote << charid;
	query.exec();

	buddies.erase(charid);
}

inline
void BuddyList::add(const mysqlpp::Row &row) {
	int charid = (int) row["buddy_charid"];
	row["name"].to_string(buddies[charid]); // Assigns string to buddies[charid] i.e. buddies[charid] = name
}