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
		PACKET(removePartyMember, party_id_t partyId, player_id_t playerId, bool kicked);
		PACKET(addPartyMember, party_id_t partyId, player_id_t playerId);
		PACKET(newPartyLeader, party_id_t partyId, player_id_t playerId);
		PACKET(createParty, party_id_t partyId, player_id_t playerId);
		PACKET(disbandParty, party_id_t partyId);
	}
	namespace PlayerPacket {
		PACKET(playerChangeChannel, player_id_t playerId, channel_id_t channelId, const Ip &ip, port_t port);
		PACKET(newConnectable, player_id_t playerId, const Ip &ip, PacketReader &buffer);
		PACKET(deleteConnectable, player_id_t playerId);
		PACKET(updatePlayer, const PlayerData &data, update_bits_t flags);
		PACKET(characterCreated, const PlayerData &data);
		PACKET(characterDeleted, player_id_t id);
	}
	namespace BuddyPacket {
		PACKET(sendBuddyInvite, player_id_t inviteeId, player_id_t inviterId, const string_t &name);
		PACKET(sendAcceptBuddyInvite, player_id_t inviteeId, player_id_t inviterId);
		PACKET(sendBuddyRemoval, player_id_t listOwnerId, player_id_t removalId);
		PACKET(sendReaddBuddy, player_id_t listOwnerId, player_id_t buddyId);
	}
}