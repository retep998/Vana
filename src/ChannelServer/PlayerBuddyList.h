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
#include <boost/utility.hpp>
#include <list>
#include <string>
#include <vector>

using std::list;
using std::string;
using std::tr1::unordered_map;
using std::vector;

class PacketCreator;
class Player;
namespace mysqlpp {
	class Row;
}

struct Buddy {
	string name;
	string groupName;
	uint8_t oppositeStatus;
	int32_t channel;
	int32_t charId;
};
typedef std::tr1::shared_ptr<Buddy> BuddyPtr;

struct BuddyInvite {
	BuddyInvite() : send(true) { }
	bool send;
	string name;
	int32_t id;
};

class PlayerBuddyList : boost::noncopyable {
public:
	PlayerBuddyList(Player *player);
	uint8_t addBuddy(const string &name, const string &group, bool invite = true);
	void removeBuddy(int32_t charId);

	BuddyPtr getBuddy(int32_t charId) { return m_buddies[charId]; }
	uint8_t listSize() const { return m_buddies.size(); }
	vector<int32_t> getBuddyIds(); // For sending the online packet to the players.
	void addBuddyInvite(const BuddyInvite &invite) { m_pendingBuddies.push_back(invite); }

	void addBuddies(PacketCreator &packet);
	void checkForPendingBuddy();
	void removePendingBuddy(int32_t id, bool accepted);
private:
	void addBuddy(const mysqlpp::Row &row);
	void load();

	bool m_sentRequest;
	list<BuddyInvite> m_pendingBuddies;
	unordered_map<int32_t, BuddyPtr> m_buddies;
	Player *m_player;
};