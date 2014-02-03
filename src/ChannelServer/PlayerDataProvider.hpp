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
#include "PlayerObjects.hpp"
#include "shared_array.hpp"
#include "Types.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class PacketBuilder;
class PacketReader;
class Party;
class Player;

struct ConnectingPlayer {
	ConnectingPlayer() : connectIp(0) { }

	Ip connectIp;
	time_point_t connectTime;
	int32_t mapId = -1;
	string_t portal;
	uint16_t packetSize;
	MiscUtilities::shared_array<unsigned char> heldPacket;
};

class PlayerDataProvider {
	SINGLETON(PlayerDataProvider);
public:
	auto parseChannelConnectPacket(PacketReader &reader) -> void;
	
	auto handlePlayerSync(PacketReader &reader) -> void;
	auto handlePartySync(PacketReader &reader) -> void;
	auto handleBuddySync(PacketReader &reader) -> void;

	// Online players
	auto addPlayer(Player *player) -> void;
	auto removePlayer(Player *player) -> void;
	auto updatePlayerLevel(Player *player) -> void;
	auto updatePlayerMap(Player *player) -> void;
	auto updatePlayerJob(Player *player) -> void;
	auto getPlayer(int32_t id) -> Player *;
	auto getPlayer(const string_t &name) -> Player *;
	auto run(function_t<void(Player *)> func) -> void;
	auto send(int32_t playerId, const PacketBuilder &packet) -> void;
	auto send(const vector_t<int32_t> &playerIds, const PacketBuilder &packet) -> void;
	auto send(const PacketBuilder &packet) -> void;
	auto addFollower(Player *follower, Player *target) -> void;
	auto stopFollowing(Player *follower) -> void;

	// Player data
	auto getPlayerData(int32_t id) const -> const PlayerData * const;
	auto getPlayerDataByName(const string_t &name) const -> const PlayerData * const;

	// Parties
	auto getParty(int32_t id) -> Party *;

	// Chat
	auto handleGroupChat(int8_t chatType, int32_t playerId, const vector_t<int32_t> &receivers, const string_t &chat) -> void;
	auto handleGmChat(Player *player, const string_t &chat) -> void;

	// Connections
	auto checkPlayer(int32_t id, const Ip &ip, bool &hasPacket) const -> Result;
	auto getPacket(int32_t id) const -> PacketReader;
	auto playerEstablished(int32_t id) -> void;
private:
	auto sendSync(const PacketBuilder &packet) const -> void;
	auto addPlayerData(const PlayerData &data) -> void;
	auto handleCharacterCreated(PacketReader &reader) -> void;
	auto handleCharacterDeleted(PacketReader &reader) -> void;
	auto handleChangeChannel(PacketReader &reader) -> void;
	auto handleNewConnectable(PacketReader &reader) -> void;
	auto handleDeleteConnectable(int32_t id) -> void;
	auto handleUpdatePlayer(PacketReader &reader) -> void;

	auto handleCreateParty(int32_t id, int32_t leaderId) -> void;
	auto handleDisbandParty(int32_t id) -> void;
	auto handlePartyTransfer(int32_t id, int32_t leaderId) -> void;
	auto handlePartyRemove(int32_t id, int32_t playerId, bool kicked) -> void;
	auto handlePartyAdd(int32_t id, int32_t playerId) -> void;

	auto buddyInvite(PacketReader &reader) -> void;
	auto buddyOnlineOffline(PacketReader &reader) -> void;

	auto newPlayer(int32_t id, const Ip &ip, PacketReader &reader) -> void;

	const static uint32_t MaxConnectionMilliseconds = 5000;

	hash_set_t<int32_t> m_gmList;
	hash_map_t<int32_t, PlayerData> m_playerData;
	hash_map_t<int32_t, vector_t<Player *>> m_followers;
	case_insensitive_hash_map_t<PlayerData *> m_playerDataByName;
	hash_map_t<int32_t, ref_ptr_t<Party>> m_parties;
	hash_map_t<int32_t, Player *> m_players;
	case_insensitive_hash_map_t<Player *> m_playersByName;
	hash_map_t<int32_t, ConnectingPlayer> m_connections;
};