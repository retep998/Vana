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

#include "Types.h"
#include <string>

using std::string;

class WorldServerAcceptPlayer;
class Party;
class PacketCreator;

namespace PartyPacket{
	void createParty(WorldServerAcceptPlayer *player, int32_t playerid);
	void disbandParty(WorldServerAcceptPlayer *player, int32_t playerid);
	void partyError(WorldServerAcceptPlayer *player, int32_t playerid, int8_t error);
	void giveLeader(WorldServerAcceptPlayer *player, int32_t playerid, int32_t target, bool is);
	void invitePlayer(WorldServerAcceptPlayer *player, int32_t playerid, const string &inviter);
	void updateParty(WorldServerAcceptPlayer *player, char type, int32_t playerid, int32_t target = 0);
	void addParty(PacketCreator &packet, Party *party, int32_t tochan);
};

#endif
