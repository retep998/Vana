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
#include "PlayerObjects.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

class AbstractConnection;
class Channel;
class PacketCreator;
class PacketReader;
class WorldServerAcceptConnection;
struct Rates;

namespace SyncPacket {
	auto sendSyncData(WorldServerAcceptConnection *connection) -> void;

	namespace ConfigPacket {
		auto setRates(const Rates &rates) -> void;
		auto scrollingHeader(const string_t &message) -> void;
	}
	namespace PartyPacket {
		auto removePartyMember(int32_t partyId, int32_t playerId, bool kicked) -> void;
		auto addPartyMember(int32_t partyId, int32_t playerId) -> void;
		auto newPartyLeader(int32_t partyId, int32_t playerId) -> void;
		auto createParty(int32_t partyId, int32_t playerId) -> void;
		auto disbandParty(int32_t partyId) -> void;
	}
	namespace PlayerPacket {
		auto playerChangeChannel(AbstractConnection *connection, int32_t playerId, channel_id_t channelId, const Ip &ip, port_t port) -> void;
		auto newConnectable(channel_id_t channel, int32_t playerId, const Ip &ip, PacketReader &buffer) -> void;
		auto deleteConnectable(channel_id_t channel, int32_t playerId) -> void;
		auto updatePlayer(const PlayerData &data, update_bits_t flags) -> void;
		auto characterCreated(const PlayerData &data) -> void;
		auto characterDeleted(int32_t id) -> void;
	}
	namespace BuddyPacket {
		auto sendBuddyInvite(Channel *channel, int32_t inviteeId, int32_t inviterId, const string_t &name) -> void;
		auto sendBuddyOnlineOffline(Channel *channel, const vector_t<int32_t> &players, int32_t playerId, channel_id_t channelId) -> void;
	}
}