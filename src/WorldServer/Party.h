/*
Copyright (C) 2008-2012 Vana Development Team

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
#include <boost/tr1/functional.hpp>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::tr1::function;
using std::vector;

class PacketCreator;
class Player;

class Party {
public:
	Party(int32_t id, int32_t leaderId);

	void setLeader(Player *newLeader);
	void addMember(Player *player, bool first = false);
	void deleteMember(Player *player, bool kicked);
	void disband();
	bool isLeader(int32_t playerId) const { return (playerId == m_leaderId); }
	int8_t getMemberCount() const { return m_members.size(); }
	int32_t getId() const { return m_id; }
	int32_t getLeaderId() const { return m_leaderId; }

	void runFunction(function<void (Player *)> func);
private:
	int32_t m_id;
	int32_t m_leaderId;
	vector<int32_t> m_oldLeaders;
	map<int32_t, Player *> m_members;
};