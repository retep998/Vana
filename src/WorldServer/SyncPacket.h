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

class PacketCreator;
class PacketReader;
class Party;
class Player;
class WorldServerAcceptConnection;

namespace SyncPacket {
	namespace PartyPacket {
		void createParty(uint16_t channel, int32_t playerid);
		void disbandParty(uint16_t channel, int32_t playerid);
		void partyError(uint16_t channel, int32_t playerid, int8_t error);
		void giveLeader(uint16_t channel, int32_t playerid, int32_t target, bool is);
		void invitePlayer(uint16_t channel, int32_t playerid, const string &inviter);
		void updateParty(uint16_t channel, int8_t type, int32_t playerid, int32_t target = 0);
		void addParty(PacketCreator &packet, Party *party, int32_t tochan);
	}
	namespace PlayerPacket {
		void sendPacketToChannelForHolding(uint16_t channel, int32_t playerid, PacketReader &buffer);
		void sendHeldPacketRemoval(uint16_t channel, int32_t playerid);
		void playerChangeChannel(WorldServerAcceptConnection *player, int32_t playerid, uint32_t ip, uint16_t port);
		void newConnectable(uint16_t channel, int32_t playerid, uint32_t playerIp);
		void sendParties(WorldServerAcceptConnection *player);
		void sendRemovePartyPlayer(int32_t playerid, int32_t partyid);
		void sendAddPartyPlayer(int32_t playerid, int32_t partyid);
		void sendSwitchPartyLeader(int32_t playerid, int32_t partyid);
		void sendCreateParty(int32_t playerid, int32_t partyid);
		void sendDisbandParty(int32_t partyid);
		void sendGuilds(WorldServerAcceptConnection *player);
		void sendAlliances(WorldServerAcceptConnection *player);
	}
}