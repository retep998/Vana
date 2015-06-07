/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Types.hpp"
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class Instance;
class PacketBuilder;
class Player;

class Party {
	NONCOPYABLE(Party);
	NO_DEFAULT_CONSTRUCTOR(Party);
public:
	Party(party_id_t partyId);

	auto setLeader(player_id_t playerId, bool showPacket = false) -> void;
	auto setMember(player_id_t playerId, Player *player) -> void;
	auto setInstance(Instance *instance) -> void { m_instance = instance; }
	auto isLeader(player_id_t playerId) const -> bool { return playerId == m_leaderId; }
	auto getMembersCount() const -> int8_t { return static_cast<int8_t>(m_members.size()); }
	auto getId() const -> party_id_t { return m_partyId; }
	auto getLeaderId() const -> player_id_t { return m_leaderId; }
	auto getMember(player_id_t id) -> Player * { return m_members.find(id) != std::end(m_members) ? m_members[id] : nullptr; }
	auto getLeader() -> Player * { return m_members[m_leaderId]; }
	auto getInstance() const -> Instance * { return m_instance; }

	// More complicated specific functions
	auto addMember(Player *player, bool first = false) -> void;
	auto addMember(player_id_t id, const string_t &name, bool first = false) -> void;
	auto deleteMember(Player *player, bool kicked) -> void;
	auto deleteMember(player_id_t id, const string_t &name, bool kicked) -> void;
	auto disband() -> void;
	auto showHpBar(Player *player) -> void;
	auto receiveHpBar(Player *player) -> void;
	auto silentUpdate() -> void;
	auto runFunction(function_t<void(Player *)> func) -> void;

	auto warpAllMembers(map_id_t mapId, const string_t &portalName = "") -> void;
	auto isWithinLevelRange(player_level_t lowBound, player_level_t highBound) -> bool;
	auto checkFootholds(int8_t memberCount, const vector_t<vector_t<foothold_id_t>> &footholds) -> Result;
	auto verifyFootholds(const vector_t<vector_t<foothold_id_t>> &footholds) -> Result;
	auto getMemberCountOnMap(map_id_t mapId) -> int8_t;
	auto getMemberByIndex(uint8_t oneBasedIndex) -> Player *;
	auto getZeroBasedIndexByMember(Player *player) -> int8_t;
	auto getAllPlayerIds() -> vector_t<player_id_t>;
	auto getPartyMembers(map_id_t mapId = -1) -> vector_t<Player *>;

	auto getMembers() const -> const ord_map_t<player_id_t, Player *, std::greater<player_id_t>> & { return m_members; }
private:
	player_id_t m_leaderId = 0;
	party_id_t m_partyId = 0;
	Instance *m_instance = nullptr;
	ord_map_t<player_id_t, Player *, std::greater<player_id_t>> m_members;
};