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
#ifndef PARTYPACKET_H
#define PARTYPACKET_H

#include <string>
using std::string;

class WorldServerAcceptPlayer;
class Party;
class Packet;

namespace PartyPacket{
	void createParty(WorldServerAcceptPlayer *player, int playerid);
	void disbandParty(WorldServerAcceptPlayer *player, int playerid);
	void partyError(WorldServerAcceptPlayer *player, int playerid, char error);
	void giveLeader(WorldServerAcceptPlayer *player, int playerid, int target, bool is);
	void invitePlayer(WorldServerAcceptPlayer *player, int playerid, const string &inviter);
	void updateParty(WorldServerAcceptPlayer *player, char type, int playerid, int target = 0);
	void addParty(Packet &packet, Party *party, int tochan);
};

#endif
