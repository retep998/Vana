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
#include <boost/tr1/functional.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::tr1::function;
using std::tr1::unordered_map;
using std::vector;

class Instance;
class PacketCreator;
class Player;

class Party {
public:
	Party(int32_t partyId);

	void setLeader(int32_t playerId);
	void setMember(int32_t playerId, Player *player);
	void setInstance(Instance *instance) { m_instance = instance; }
	bool isLeader(int32_t playerId) const { return (playerId == m_leaderId); }
	int8_t getMembersCount() const { return m_members.size(); }
	int32_t getId() const { return m_partyId; }
	int32_t getLeaderId() const { return m_leaderId; }
	Player * getMember(int32_t id) { return (m_members.find(id) != m_members.end() ? m_members[id] : nullptr); }
	Player * getLeader() { return m_members[m_leaderId]; }
	Instance * getInstance() const { return m_instance; }

	// More complicated specific functions
	void addMember(Player *player, bool first = false);
	void addMember(int32_t id, bool first = false);
	void deleteMember(Player *player, bool kicked);
	void deleteMember(int32_t id, bool kicked);
	void disband();
	void showHpBar(Player *player);
	void receiveHpBar(Player *player);
	void silentUpdate();
	void runFunction(function<void (Player *)> func);

	void warpAllMembers(int32_t mapId, const string &portalname = "");
	bool isWithinLevelRange(uint8_t lowbound, uint8_t highbound);
	bool checkFootholds(int8_t membercount, const vector<vector<int16_t>> &footholds); // Determines if the players are properly arranged (e.g. 3 people on 3 different barrels in Kerning PQ)
	bool verifyFootholds(const vector<vector<int16_t>> &footholds); // Determines if the players match your selected foothold groups
	int8_t getMemberCountOnMap(int32_t mapId);
	Player * getMemberByIndex(uint8_t index);
	vector<int32_t> getAllPlayerIds();
	vector<Player *> getPartyMembers(int32_t mapId = -1);

	void updatePacket(PacketCreator &packet);
private:
	typedef map<int32_t, Player *, std::greater<int32_t>> PlayerMap;
	typedef unordered_map<size_t, bool> TakenFootholds;

	int32_t m_leaderId;
	int32_t m_partyId;
	Instance *m_instance;
	PlayerMap m_members;
};