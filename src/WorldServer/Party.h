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
	void setVoters(int8_t voters) { m_voters = voters; }
	void setGuildContract(int8_t guildContract) { m_guildContract = guildContract; }
	void setGuildName(const string &name) { m_guildName = name; }
	void clearGuild();
	bool isLeader(int32_t playerId) const { return (playerId == m_leaderId); }
	int8_t getGuildContract() const { return m_guildContract; }
	int8_t getVoters() const { return m_voters; }
	int32_t getId() const { return m_id; }
	int32_t getLeader() const { return m_leaderId; }
	string getGuildName() const { return m_guildName; }

	map<int32_t, Player *> members;
private:
	int8_t m_guildContract;
	int8_t m_voters;
	int32_t m_id;
	int32_t m_leaderId;
	string m_guildName;
	vector<int32_t> m_oldLeaders;
};
