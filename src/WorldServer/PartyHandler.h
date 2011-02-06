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

#ifndef PARTYHANDLER_H
#define PARTYHANDLER_H

#include "Players.h"
#include "Types.h"
#include <map>
#include <vector>
#include <string>

using std::map;
using std::vector;
using std::string;

class Party;
class WorldServerAcceptConnection;

#define PARTY_LEAVE 0x01
#define PARTY_JOIN 0x02
#define PARTY_SET_LEADER 0x03
#define PARTY_SILENT_UPDATE 0x04
#define PARTY_LOG_IN_OUT 0x05
#define PARTY_EXPEL 0x06

#define PARTY_SYNC_CHANNEL_START 0x01
#define PARTY_SYNC_DISBAND 0x02
#define PARTY_SYNC_CREATE 0x03
#define PARTY_SYNC_SWITCH_LEADER 0x04
#define PARTY_SYNC_REMOVE_MEMBER 0x05
#define PARTY_SYNC_ADD_MEMBER 0x06

namespace PartyHandler {
	void createParty(WorldServerAcceptConnection *player, int32_t playerid);
	void leaveParty(WorldServerAcceptConnection *player, int32_t playerid);
	void invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &invitee);
	void joinParty(WorldServerAcceptConnection *player, int32_t playerid, int32_t partyid);
	void giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is);
	void expelPlayer(WorldServerAcceptConnection *player, int32_t playerid, int32_t target);
	void silentUpdate(int32_t playerid);
	void logInLogOut(int32_t playerid);
};

class Party {
public:
	void setLeader(int32_t playerid) { this->leaderid = playerid; }
	void addMember(Player *player) { this->members[player->id] = player; }
	void deleteMember(int32_t target) { this->members.erase(target); }

	int32_t getLeader() const { return this->leaderid; }
	bool isLeader(int32_t playerid) const { return playerid == leaderid; }
	map<int32_t, Player *> members;
	vector<int32_t> oldLeader;
private:
	int32_t leaderid;
};

#endif
