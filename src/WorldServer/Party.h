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
#include <map>
#include <string>
#include <vector>

using std::string;
using std::map;
using std::vector;

class Player;

class Party {
public:
	Party(int32_t id);

	void setLeader(int32_t playerId);
	void addMember(Player *player);
	void deleteMember(int32_t playerId) { members.erase(playerId); }
	bool isLeader(int32_t playerId) const { return (playerId == m_leaderId); }
	int32_t getId() const { return m_id; }
	int32_t getLeader() const { return m_leaderId; }

	map<int32_t, Player *> members;
private:
	int32_t m_id;
	int32_t m_leaderId;
	vector<int32_t> m_oldLeaders;
};