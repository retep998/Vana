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

class Alliance;
class BbsThread;
class Guild;
class PacketCreator;
class PacketReader;
class Party;
class Player;
class WorldServerAcceptConnection;

namespace SyncPacket {
	namespace AlliancePacket {
		void changeAlliance(Alliance *alliance, int8_t type);
		void changeGuild(Alliance *alliance, Guild *guild);
		void changeLeader(Alliance *alliance, Player *oldLeader);
		void changePlayerRank(Alliance *alliance, Player *player);
		void changeCapacity(Alliance *alliance);
	}
	namespace BbsPacket {
		void sendThreadList(uint16_t channel, Guild *guild, int32_t playerid, int16_t page = 0);
		void sendThreadData(uint16_t channel, BbsThread *thread, int32_t playerid);
	}
	namespace GuildPacket {
		void updatePlayers(Guild *guild, bool remove);
		void updatePlayer(Guild *guild, Player *player);
		void updatePlayerRank(Player *player);
		void updatePlayerMesos(Player *player, int32_t amount);
		void updateEmblem(Guild *guild);
		void updateCapacity(Guild *guild);
		void loadGuild(int32_t guildid);
		void unloadGuild(int32_t guildid);
		void addPlayer(Player *player);
		void removePlayer(Player *player);
	}
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
		void sendPacketToChannelForHolding(uint16_t channel, int32_t playerid, PacketReader &buffer, bool fromCashOrMts);
		void sendPacketToCashServerForHolding(int32_t playerid, PacketReader &buffer);
		void sendHeldPacketRemoval(uint16_t channel, int32_t playerid);
		void playerChangeChannel(WorldServerAcceptConnection *player, int32_t playerid, uint32_t ip, uint16_t port);
		void newConnectable(uint16_t channel, int32_t playerid, uint32_t playerip);
		void newConnectableCashServer(int32_t playerid, uint32_t playerip);
		void sendCannotChangeServerToPlayer(uint16_t channel, int32_t playerid, int8_t reason);
		void sendPlayerDisconnectServer(WorldServerAcceptConnection *player, int32_t playerid);
		void sendParties(WorldServerAcceptConnection *player);
		void sendRemovePartyPlayer(int32_t playerid, int32_t partyid);
		void sendAddPartyPlayer(int32_t playerid, int32_t partyid);
		void sendSwitchPartyLeader(int32_t playerid, int32_t partyid);
		void sendCreateParty(int32_t playerid, int32_t partyid);
		void sendDisbandParty(int32_t partyid);
		void sendGuilds(WorldServerAcceptConnection *player);
		void sendAlliances(WorldServerAcceptConnection *player);
	}
	namespace BuddyPacket {
		void sendBuddyInvite(WorldServerAcceptConnection *channel, int32_t inviteeid, int32_t inviterid, const string &name);
		void sendBuddyOnlineOffline(WorldServerAcceptConnection *channel, vector<int32_t> players, int32_t playerid, int32_t channelid);
	}
}
