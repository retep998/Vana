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
#include <vector>

class PacketCreator;
class Player;

class Party {
	NONCOPYABLE(Party);
	NO_DEFAULT_CONSTRUCTOR(Party);
public:
	Party(int32_t id, int32_t leaderId);

	auto setLeader(Player *newLeader) -> void;
	auto addMember(Player *player, bool first = false) -> void;
	auto deleteMember(Player *player, bool kicked) -> void;
	auto disband() -> void;
	auto isLeader(int32_t playerId) const -> bool { return playerId == m_leaderId; }
	auto getMemberCount() const -> int8_t { return m_members.size(); }
	auto getId() const -> int32_t { return m_id; }
	auto getLeaderId() const -> int32_t { return m_leaderId; }

	auto runFunction(function_t<void(Player *)> func) -> void;
private:
	int32_t m_id = 0;
	int32_t m_leaderId = 0;
	vector_t<int32_t> m_oldLeaders;
	ord_map_t<int32_t, Player *> m_members;
};