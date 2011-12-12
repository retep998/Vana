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
#include <vector>

using std::string;
using std::vector;

class PacketCreator;
class PacketReader;
class Party;
class Player;
class WorldServerAcceptConnection;

namespace SyncPacket {
	void sendSyncData(WorldServerAcceptConnection *player);

	namespace PartyPacket {
		void removePartyMember(int32_t partyId, int32_t playerId, bool kicked);
		void addPartyMember(int32_t partyId, int32_t playerId);
		void newPartyLeader(int32_t partyId, int32_t playerId);
		void createParty(int32_t partyId, int32_t playerId);
		void disbandParty(int32_t partyId);
	}
	namespace PlayerPacket {
		void playerChangeChannel(WorldServerAcceptConnection *connection, int32_t playerId, ip_t ip, port_t port);
		void newConnectable(uint16_t channel, int32_t playerId, ip_t ip, PacketReader &buffer);
		void deleteConnectable(uint16_t channel, int32_t playerId);
		void updatePlayerJob(int32_t playerId, int16_t job);
		void updatePlayerMap(int32_t playerId, int32_t map);
		void updatePlayerLevel(int32_t playerId, int16_t level);
	}
	namespace BuddyPacket {
		void sendBuddyInvite(WorldServerAcceptConnection *connection, int32_t inviteeId, int32_t inviterid, const string &name);
		void sendBuddyOnlineOffline(WorldServerAcceptConnection *connection, const vector<int32_t> &players, int32_t playerId, int32_t channelId);
	}
}