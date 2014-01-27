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
#include "Types.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class PacketCreator;
class Party;
class Player;
namespace soci {
	class row;
}

class PlayerDataProvider {
	SINGLETON_CUSTOM_CONSTRUCTOR(PlayerDataProvider);
public:
	auto loadData() -> void;
	auto getChannelConnectPacket(PacketCreator &packet) -> void;
	auto getPlayerDataPacket(PacketCreator &packet, int32_t playerId) -> void;

	// Player info
	auto initialPlayerConnect(int32_t id, uint16_t channel, const Ip &ip) -> void;
	auto playerConnect(Player *player, bool online = true) -> void;
	auto playerDisconnect(int32_t id, int16_t channel = -1) -> void;
	auto removeChannelPlayers(uint16_t channel) -> void;
	auto loadPlayer(int32_t playerId) -> void;
	auto getPlayer(const string_t &name, bool includeOffline = false) -> Player *;
	auto getPlayer(int32_t id, bool includeOffline = false) -> Player *;
	auto getPlayerQuantity() -> int32_t;

	// Channel changes
	auto addPendingPlayer(int32_t id, uint16_t channelId) -> void;
	auto removePendingPlayer(int32_t id) -> void;
	auto removePendingPlayerEarly(int32_t id) -> int16_t;
	auto getPendingPlayerChannel(int32_t id) -> uint16_t;

	// Parties
	auto getPartyId() -> int32_t;
	auto createParty(int32_t playerId) -> void;
	auto addPartyMember(int32_t playerId) -> void;
	auto removePartyMember(int32_t playerId, int32_t target) -> void;
	auto removePartyMember(int32_t playerId) -> void;
	auto setPartyLeader(int32_t playerId, int32_t leaderId) -> void;
	auto disbandParty(int32_t id) -> void;
	auto getParty(int32_t id) -> Party *;
private:
	auto loadGuilds(int16_t worldId) -> void;
	auto loadAlliances(int16_t worldId) -> void;
	auto loadPlayers(int16_t worldId) -> void;
	auto loadPlayer(const soci::row &row) -> void;
	auto generatePlayerDataPacket(PacketCreator &packet, Player *player) -> void;

	LoopingId<int32_t> m_partyIds;
	hash_map_t<int32_t, uint16_t> m_channelSwitches;
	hash_map_t<int32_t, ref_ptr_t<Party>> m_parties;
	hash_map_t<int32_t, ref_ptr_t<Player>> m_players;
};