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

using std::map;
using std::string;
using std::vector;

class Instance;
class PacketCreator;
class Player;

class Party {
public:
	Party(int32_t pid) : partyid(pid), instance(nullptr) { }
	void setLeader(int32_t playerid, bool firstload = false);
	void addMember(Player *player);
	void addMember(int32_t id);
	void deleteMember(Player *player);
	void deleteMember(int32_t id);
	void disband();
	void setMember(int32_t playerid, Player *player);
	void showHpBar(Player *player);
	void receiveHpBar(Player *player);
	void setInstance(Instance *inst) { instance = inst; }
	void warpAllMembers(int32_t mapid, const string &portalname = "");
	void sendPacket(PacketCreator &packet, int32_t mapid = -1);

	Player * getMember(int32_t id) { return (members.find(id) != members.end() ? members[id] : nullptr); }
	Player * getMemberByIndex(uint8_t index);
	Player * getLeader() { return members[leaderid]; }
	Instance * getInstance() const { return instance; }
	vector<int32_t> getAllPlayerIds();
	vector<Player *> getPartyMembers(int32_t mapid = -1);
	int32_t getLeaderId() const { return leaderid; }
	int32_t getId() const { return partyid; }
	int8_t getMembersCount() const { return members.size(); }
	int8_t getMemberCountOnMap(int32_t mapid);

	bool isLeader(int32_t playerid) const { return playerid == leaderid; }
	bool isWithinLevelRange(uint8_t lowbound, uint8_t highbound);
	bool checkFootholds(int8_t membercount, const vector<vector<int16_t> > &footholds); // Determines if the players are properly arranged (e.g. 3 people on 3 different barrels in Kerning PQ)
	bool verifyFootholds(const vector<vector<int16_t> > &footholds); // Determines if the players match your selected foothold groups
private:
	map<int32_t, Player *, std::greater<int32_t> > members;
	vector<int32_t> oldleader;
	int32_t leaderid;
	int32_t partyid;
	Instance *instance;
};
