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
#include <list>
#include <string>
#include <vector>

using std::list;
using std::string;
using std::vector;
using std::tr1::unordered_map;

class PacketCreator;
class Player;

namespace mysqlpp {
	class Row;
};

class PlayerBuddyList {
public:
	struct Buddy {
		string m_name;
		string m_group_name;
		uint8_t m_oppositeStatus;
		int32_t m_channel;
		int32_t m_charid;
	};
	struct BuddyInvite {
		BuddyInvite() : m_send(true) { }
		bool m_send;
		string m_name;
		int32_t m_id;
	};

	typedef std::tr1::shared_ptr<Buddy> BuddyPtr;

	PlayerBuddyList(Player *player);
	uint8_t addBuddy(const string &name, const string &group, bool invite = true);
	void removeBuddy(int32_t charid);

	BuddyPtr getBuddy(int32_t charid) { return m_buddies[charid]; }
	uint8_t listSize() const { return (uint8_t)m_buddies.size(); }
	vector<int32_t> getBuddyIds(); // For sending the online packet to the players.
	void addBuddyInvite(PlayerBuddyList::BuddyInvite invite) { m_pending_buddies.push_back(invite); }

	void addBuddies(PacketCreator &packet);
	void checkForPendingBuddy();
	void removePendingBuddy(int32_t id, bool accepted);
private:
	void addBuddy(const mysqlpp::Row &row);
	void load();

	unordered_map<int32_t, BuddyPtr> m_buddies;
	list<BuddyInvite> m_pending_buddies;
	Player *m_player;

	bool sentRequest;
};
