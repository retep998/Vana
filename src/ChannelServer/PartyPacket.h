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
#include <string>

using std::string;

class Party;
class Player;

namespace PartyPacket {
	namespace Errors {
		enum : int8_t {
			PlayerHasParty = 0x10,
			PartyFull = 0x11,
			DifferingChannel = 0x12
		};
	}

	void error(Player *player, int8_t error);
	void createParty(Player *packetTarget, Party *party);
	void joinParty(Player *packetTarget, Party *party, const string &player);
	void leaveParty(Player *packetTarget, Party *party, int32_t playerId, const string &name, bool kicked);
	void invitePlayer(Player *packetTarget, Party *party, const string &inviter);
	void disbandParty(Player *packetTarget, Party *party);
	void setLeader(Player *packetTarget, Party *party, int32_t newLeader);
	void silentUpdate(Player *packetTarget, Party *party);
}