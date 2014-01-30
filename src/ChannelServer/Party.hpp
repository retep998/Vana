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

#include "Types.hpp"
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class Instance;
class PacketCreator;
class Player;

class Party {
	NONCOPYABLE(Party);
	NO_DEFAULT_CONSTRUCTOR(Party);
public:
	Party(int32_t partyId);

	auto setLeader(int32_t playerId, bool showPacket = false) -> void;
	auto setMember(int32_t playerId, Player *player) -> void;
	auto setInstance(Instance *instance) -> void { m_instance = instance; }
	auto isLeader(int32_t playerId) const -> bool { return playerId == m_leaderId; }
	auto getMembersCount() const -> int8_t { return m_members.size(); }
	auto getId() const -> int32_t { return m_partyId; }
	auto getLeaderId() const -> int32_t { return m_leaderId; }
	auto getMember(int32_t id) -> Player * { return m_members.find(id) != std::end(m_members) ? m_members[id] : nullptr; }
	auto getLeader() -> Player * { return m_members[m_leaderId]; }
	auto getInstance() const -> Instance * { return m_instance; }

	// More complicated specific functions
	auto addMember(Player *player, bool first = false) -> void;
	auto addMember(int32_t id, const string_t &name, bool first = false) -> void;
	auto deleteMember(Player *player, bool kicked) -> void;
	auto deleteMember(int32_t id, const string_t &name, bool kicked) -> void;
	auto disband() -> void;
	auto showHpBar(Player *player) -> void;
	auto receiveHpBar(Player *player) -> void;
	auto silentUpdate() -> void;
	auto runFunction(function_t<void(Player *)> func) -> void;

	auto warpAllMembers(int32_t mapId, const string_t &portalName = "") -> void;
	auto isWithinLevelRange(uint8_t lowBound, uint8_t highBound) -> bool;
	auto checkFootholds(int8_t membercount, const vector_t<vector_t<int16_t>> &footholds) -> bool;
	auto verifyFootholds(const vector_t<vector_t<int16_t>> &footholds) -> bool;
	auto getMemberCountOnMap(int32_t mapId) -> int8_t;
	auto getMemberByIndex(uint8_t index) -> Player *;
	auto getAllPlayerIds() -> vector_t<int32_t>;
	auto getPartyMembers(int32_t mapId = -1) -> vector_t<Player *>;

	auto updatePacket(int32_t mapId, PacketCreator &packet) -> void;
	auto getMembers() const -> const ord_map_t<int32_t, Player *, std::greater<int32_t>> & { return m_members; }
private:
	int32_t m_leaderId = 0;
	int32_t m_partyId = 0;
	Instance *m_instance = nullptr;
	ord_map_t<int32_t, Player *, std::greater<int32_t>> m_members;
};