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

#include "GameObjects.hpp"
#include "Ip.hpp"
#include "LoopingId.hpp"
#include "PlayerObjects.hpp"
#include "Types.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class AbstractConnection;
class PacketBuilder;
class PacketBuilder;
class Player;
class WorldServerAcceptConnection;
namespace soci {
	class row;
}

class PlayerDataProvider {
	SINGLETON_CUSTOM_CONSTRUCTOR(PlayerDataProvider);
public:
	auto loadData() -> void;
	auto getChannelConnectPacket(PacketBuilder &packet) -> void;
	auto channelDisconnect(channel_id_t channel) -> void;
	auto send(int32_t playerId, const PacketBuilder &builder) -> void;
	auto send(const vector_t<int32_t> &playerIds, const PacketBuilder &builder) -> void;
	auto send(const PacketBuilder &builder) -> void;

	// Handling
	auto handlePlayerSync(AbstractConnection *connection, PacketReader &reader) -> void;
	auto handlePartySync(AbstractConnection *connection, PacketReader &reader) -> void;
	auto handleBuddySync(AbstractConnection *connection, PacketReader &reader) -> void;
private:
	auto loadPlayers(world_id_t worldId) -> void;
	auto loadPlayer(int32_t playerId) -> void;
	auto addPlayer(const PlayerData &data) -> void;
	auto sendSync(const PacketBuilder &builder) const -> void;

	// Players
	auto removePendingPlayer(int32_t id) -> channel_id_t;
	auto handlePlayerConnect(channel_id_t channel, PacketReader &reader) -> void;
	auto handlePlayerDisconnect(channel_id_t channel, PacketReader &reader) -> void;
	auto handleChangeChannelRequest(AbstractConnection *connection, PacketReader &reader) -> void;
	auto handleChangeChannel(AbstractConnection *connection, PacketReader &reader) -> void;
	auto handlePlayerUpdate(PacketReader &reader) -> void;
	auto handleCharacterCreated(PacketReader &reader) -> void;
	auto handleCharacterDeleted(PacketReader &reader) -> void;

	// Parties
	auto handleCreateParty(int32_t playerId) -> void;
	auto handlePartyAdd(int32_t playerId, int32_t partyId) -> void;
	auto handlePartyRemove(int32_t playerId, int32_t targetId) -> void;
	auto handlePartyLeave(int32_t playerId) -> void;
	auto handlePartyTransfer(int32_t playerId, int32_t newLeaderId) -> void;

	// Buddies
	auto buddyInvite(PacketReader &reader) -> void;
	auto buddyOnline(PacketReader &reader) -> void;

	LoopingId<int32_t> m_partyIds;
	hash_map_t<int32_t, channel_id_t> m_channelSwitches;
	hash_map_t<int32_t, PartyData> m_parties;
	hash_map_t<int32_t, PlayerData> m_players;
	case_insensitive_hash_map_t<PlayerData *> m_playersByName;
};