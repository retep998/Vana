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

#include "GameObjects.h"
#include "PlayerObjects.h"
#include "Types.h"
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

	// Online players
	auto addPlayer(Player *player) -> void;
	auto newPlayer(PacketReader &packet) -> void;
	auto changeChannel(PacketReader &packet) -> void;
	auto newConnectable(PacketReader &packet) -> void;
	auto deleteConnectable(int32_t id) -> void;
	auto removePlayer(Player *player) -> void;
	auto getPlayer(int32_t id) -> Player *;
	auto getPlayer(const string_t &name) -> Player *;
	auto run(function_t<void(Player *)> func) -> void;
	auto sendPacket(PacketCreator &packet, int32_t minGmLevel = 0) -> void;

	// Player data
	auto getPlayerData(int32_t id) -> PlayerData *;
	auto updatePlayer(PacketReader &packet) -> void;

	// Parties
	auto getParty(int32_t id) -> Party *;
	auto newParty(int32_t id, int32_t leaderId) -> void;
	auto disbandParty(int32_t id) -> void;
	auto switchPartyLeader(int32_t id, int32_t leaderId) -> void;
	auto removePartyMember(int32_t id, int32_t playerId, bool kicked) -> void;
	auto addPartyMember(int32_t id, int32_t playerId) -> void;
private:
	auto parsePlayer(PacketReader &packet) -> void;

	hash_map_t<int32_t, ref_ptr_t<PlayerData>> m_playerData;
	hash_map_t<int32_t, ref_ptr_t<Party>> m_parties;
	hash_map_t<int32_t, Player *> m_players;
	case_insensitive_hash_map_t<Player *> m_playersByName;
};