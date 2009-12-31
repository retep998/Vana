/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "PlayerBuddyList.h"
#include "BuddyListPacket.h"
#include "Database.h"
#include "Player.h"
#include <algorithm>

PlayerBuddyList::PlayerBuddyList(Player *player) : player(player) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT buddylist.id, buddylist.buddy_charid, buddylist.name AS name_cache, characters.name FROM buddylist LEFT JOIN characters ON buddylist.buddy_charid = characters.id WHERE buddylist.charid = " << player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		add(res[i]);
	}
}

uint8_t PlayerBuddyList::add(const string &name) {
	if (buddies.size() >= player->getBuddyListSize()) { // Buddy list full
		return 0x0b;
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT c.id, c.name, u.gm, c.buddylist_size AS buddylist_limit,(SELECT COUNT(b.id) FROM buddylist b WHERE b.charid = c.id ) AS buddylist_size FROM characters c INNER JOIN users u ON c.userid = u.id WHERE c.name = " << mysqlpp::quote << name;
	mysqlpp::StoreQueryResult res = query.store();

	if (res.size() == 0) { // Name does not exist
		return 0x0f;
	}

	if ((int32_t) res[0][2] > 0 && !player->isGm()) { // GM cannot be in buddy list unless the player is a GM
		return 0x0e;
	}

	if ((uint32_t) res[0][4] >= (uint32_t) res[0][3]) { // Opposite-end buddy list full
		return 0x0c;
	}

	int32_t charid = res[0][0];

	if (buddies.find(charid) != buddies.end()) { // Already in buddy list
		return 0x0d;
	}

	query << "INSERT INTO buddylist (charid, buddy_charid, name) VALUES (" << player->getId() << ", " << charid << ", " << mysqlpp::quote << res[0][1] << ")";
	mysqlpp::SimpleResult res2 = query.execute();
	
	query << "SELECT buddylist.id, buddylist.buddy_charid, buddylist.name AS name_cache, characters.name FROM buddylist LEFT JOIN characters ON buddylist.buddy_charid = characters.id WHERE buddylist.id = " << res2.insert_id();
	res = query.store();

	add(res[0]);

	BuddyListPacket::update(player, BuddyListPacket::add);

	return 0;
}

void PlayerBuddyList::remove(int32_t charid) {
	if (buddies.find(charid) == buddies.end()) {
		// Hacking
		return;
	}

	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM buddylist WHERE charid = " << player->getId() << " AND buddy_charid = " << charid;
	query.exec();

	buddies.erase(charid);
	buddies_order.erase(std::remove(buddies_order.begin(), buddies_order.end(), charid));

	BuddyListPacket::update(player, BuddyListPacket::remove);
}

void PlayerBuddyList::add(const mysqlpp::Row &row) {
	int32_t charid = (int32_t) row["buddy_charid"];

	if (!row["name"].is_null() && row["name"] != row["name_cache"]) {
		// Outdated name cache, i.e. character renamed
		mysqlpp::Query query = Database::getCharDB().query();
		query << "UPDATE buddylist SET name = " << mysqlpp::quote << row["name"] << " WHERE id = " << row["id"];
		query.exec();
	}

	BuddyPtr buddy(new Buddy);
	buddy->charid = charid;
	
	// Note that the cache is for displaying the character name when the
	// character in question is deleted.
	if (row["name"].is_null()) {
		row["name_cache"].to_string(buddy->name); // Buddy's character deleted
	}
	else {
		row["name"].to_string(buddy->name);
	}

	buddy->oppositeStatus = OppositeStatus::unregistered; // FIXME
	buddy->channel = -1; // FIXME

	buddies[charid] = buddy;
	buddies_order.push_back(charid);
}
