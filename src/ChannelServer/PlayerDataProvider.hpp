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
#include "PlayerObjects.hpp"
#include "Types.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class PacketCreator;
class PacketReader;
class Party;
class Player;

class PlayerDataProvider {
	SINGLETON(PlayerDataProvider);
public:
	auto parseChannelConnectPacket(PacketReader &packet) -> void;
	
	auto handlePlayerSync(PacketReader &packet) -> void;
	auto handlePartySync(PacketReader &packet) -> void;
	auto handleBuddySync(PacketReader &packet) -> void;

	// Online players
	auto addPlayer(Player *player) -> void;
	auto removePlayer(Player *player) -> void;
	auto updatePlayerLevel(Player *player) -> void;
	auto updatePlayerMap(Player *player) -> void;
	auto updatePlayerJob(Player *player) -> void;
	auto getPlayer(int32_t id) -> Player *;
	auto getPlayer(const string_t &name) -> Player *;
	auto run(function_t<void(Player *)> func) -> void;
	auto sendPacket(PacketCreator &packet, int32_t minGmLevel = 0) -> void;
	auto sendPacketToList(const vector_t<int32_t> &playerIds, PacketCreator &packet) -> void;
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
private:
	auto addPlayerData(const PlayerData &data) -> void;
	auto handleCharacterCreated(PacketReader &packet) -> void;
	auto handleCharacterDeleted(PacketReader &packet) -> void;
	auto handleChangeChannel(PacketReader &packet) -> void;
	auto handleNewConnectable(PacketReader &packet) -> void;
	auto handleDeleteConnectable(int32_t id) -> void;
	auto handleUpdatePlayer(PacketReader &packet) -> void;

	auto handleCreateParty(int32_t id, int32_t leaderId) -> void;
	auto handleDisbandParty(int32_t id) -> void;
	auto handlePartyTransfer(int32_t id, int32_t leaderId) -> void;
	auto handlePartyRemove(int32_t id, int32_t playerId, bool kicked) -> void;
	auto handlePartyAdd(int32_t id, int32_t playerId) -> void;

	auto buddyInvite(PacketReader &packet) -> void;
	auto buddyOnlineOffline(PacketReader &packet) -> void;

	hash_set_t<int32_t> m_gmList;
	hash_map_t<int32_t, PlayerData> m_playerData;
	hash_map_t<int32_t, vector_t<Player *>> m_followers;
	case_insensitive_hash_map_t<PlayerData *> m_playerDataByName;
	hash_map_t<int32_t, ref_ptr_t<Party>> m_parties;
	hash_map_t<int32_t, Player *> m_players;
	case_insensitive_hash_map_t<Player *> m_playersByName;
};