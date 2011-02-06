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
#ifndef PARTYPACKET_H
#define PARTYPACKET_H

#include "Types.h"
#include <string>

using std::string;

class WorldServerAcceptConnection;
class Party;
class PacketCreator;

namespace PartyPacket {
	void createParty(WorldServerAcceptConnection *player, int32_t playerid);
	void disbandParty(WorldServerAcceptConnection *player, int32_t playerid);
	void partyError(WorldServerAcceptConnection *player, int32_t playerid, int8_t error);
	void giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is);
	void invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &inviter);
	void updateParty(WorldServerAcceptConnection *player, int8_t type, int32_t playerid, int32_t target = 0);
	void addParty(PacketCreator &packet, Party *party, int32_t tochan);
};

#endif
