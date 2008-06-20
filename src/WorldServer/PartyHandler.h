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

#ifndef PARTYHANDLER_H
#define PARTYHANDLER_H

#include <hash_map>
#include <vector>
#include "Players.h"

using stdext::hash_map;
using std::vector;
using std::string;

class Party;
class WorldServerAcceptPlayer;

#define PARTY_LEAVE 0x01
#define PARTY_JOIN 0x02
#define PARTY_SET_LEADER 0x03
#define PARTY_SILENT_UPDATE 0x04
#define PARTY_LOG_IN_OUT 0x05
#define PARTY_EXPEL 0x06

namespace PartyHandler{
	extern int partyCount;
	extern hash_map <int, Party *> parties;
	void createParty(WorldServerAcceptPlayer *player, int playerid);
	void leaveParty(WorldServerAcceptPlayer *player, int playerid);
	void invitePlayer(WorldServerAcceptPlayer *player, int playerid, const string &invitee);
	void joinParty(WorldServerAcceptPlayer *player, int playerid, int partyid);
	void giveLeader(WorldServerAcceptPlayer *player, int playerid, int target, bool is);
	void expelPlayer(WorldServerAcceptPlayer *player, int playerid, int target);
	void silentUpdate(int playerid);
	void logInLogOut(int playerid);
};

class Party{
public:
	void setId(int partyid) {
		this->partyid = partyid;
	}
	int getId() {
		return this->partyid;
	}
	void setLeader(int playerid) {
		this->leaderid = playerid;
	}
	int getLeader() {
		return this->leaderid;
	}
	bool isLeader(int playerid) {
		return playerid == leaderid;
	}
	void addMember(Player *player) {
		this->members[player->id] = player;
	}
	void deleteMember(Player *player) {
		this->members.erase(player->id);
	}
	hash_map<int, Player *> members;
	vector<int> oldLeader;
private:
	int partyid;
	int leaderid;
};

#endif
