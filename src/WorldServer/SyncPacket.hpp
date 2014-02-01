/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Ip.hpp"
#include "InterHelper.hpp"
#include "PacketBuilder.hpp"
#include "PlayerObjects.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

class AbstractConnection;
class PacketBuilder;
class PacketReader;
struct Rates;

namespace SyncPacket {
	PACKET(sendSyncData, function_t<void(PacketBuilder &)> buildSyncData);

	namespace ConfigPacket {
		PACKET(setRates, const Rates &rates);
		PACKET(scrollingHeader, const string_t &message);
	}
	namespace PartyPacket {
		PACKET(removePartyMember, int32_t partyId, int32_t playerId, bool kicked);
		PACKET(addPartyMember, int32_t partyId, int32_t playerId);
		PACKET(newPartyLeader, int32_t partyId, int32_t playerId);
		PACKET(createParty, int32_t partyId, int32_t playerId);
		PACKET(disbandParty, int32_t partyId);
	}
	namespace PlayerPacket {
		PACKET(playerChangeChannel, int32_t playerId, channel_id_t channelId, const Ip &ip, port_t port);
		PACKET(newConnectable, int32_t playerId, const Ip &ip, PacketReader &buffer);
		PACKET(deleteConnectable, int32_t playerId);
		PACKET(updatePlayer, const PlayerData &data, update_bits_t flags);
		PACKET(characterCreated, const PlayerData &data);
		PACKET(characterDeleted, int32_t id);
	}
	namespace BuddyPacket {
		PACKET(sendBuddyInvite, int32_t inviteeId, int32_t inviterId, const string_t &name);
		PACKET(sendBuddyOnlineOffline, const vector_t<int32_t> &players, int32_t playerId, channel_id_t channelId);
	}
}