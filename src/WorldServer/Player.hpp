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
#include "MapConstants.hpp"
#include "Types.hpp"
#include <string>

class Party;

class Player {
	NONCOPYABLE(Player);
public:
	Player();
	Player(int32_t id);
	auto setOnline(bool isOnline) -> void { m_online = isOnline; }
	auto setIp(const Ip &newIp) -> void { m_ip = newIp; }
	auto setChannel(int16_t newChannel) -> void { m_channel = newChannel; }
	auto setOnlineTime(int64_t newTime) -> void { m_onlineTime = newTime; }
	auto setMap(int32_t mapId) -> void { m_map = mapId; }
	auto setJob(int16_t jobId) -> void { m_job = jobId; }
	auto setLevel(uint8_t level) -> void { m_level = level; }
	auto setName(const string_t &newName) -> void { m_name = newName; }
	auto isOnline() const -> bool { return m_online; }
	auto getLevel() const -> uint8_t { return m_level; }
	auto getChannel() const -> int16_t { return m_channel; }
	auto getJob() const -> int16_t { return m_job; }
	auto getMap() const -> int32_t { return m_map; }
	auto getId() const -> int32_t { return m_id; }
	auto getOnlineTime() const -> int64_t { return m_onlineTime; }
	auto getName() const -> string_t { return m_name; }
	auto getIp() const -> const Ip & { return m_ip; }

	// Party
	auto setParty(Party *party) -> void { m_party = party; }
	auto getParty() const -> Party * { return m_party; }
private:
	bool m_online = false;
	bool m_cashShop = false;
	uint8_t m_level = 0;
	int16_t m_channel = -1;
	int16_t m_job = -1;
	int32_t m_id = 0;
	int32_t m_map = Maps::NoMap;
	int64_t m_onlineTime = 0;
	int64_t m_inviteTime = 0;
	Party *m_party = nullptr;
	string_t m_name;
	Ip m_ip;
};