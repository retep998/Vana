/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "Types.h"
#include <string>

using std::string;

class Party;

class Player {
public:
	Player();
	Player(int32_t id);
	void setOnline(bool isOnline) { m_online = isOnline; }
	void setIp(ip_t newIp) { m_ip = newIp; }
	void setChannel(int16_t newChannel) { m_channel = newChannel; }
	void setOnlineTime(int64_t newTime) { m_onlineTime = newTime; }
	void setMap(int32_t mapId) { m_map = mapId; }
	void setJob(int16_t jobId) { m_job = jobId; }
	void setLevel(uint8_t level) { m_level = level; }
	void setName(const string &newName) { m_name = newName; }
	bool isOnline() const { return m_online; }
	uint8_t getLevel() const { return m_level; }
	int16_t getChannel() const { return m_channel; }
	int16_t getJob() const { return m_job; }
	int32_t getMap() const { return m_map; }
	int32_t getId() const { return m_id; }
	ip_t getIp() const { return m_ip; }
	int64_t getOnlineTime() const { return m_onlineTime; }
	string getName() const { return m_name; }

	// Party
	void setParty(Party *party) { m_party = party; }
	Party * getParty() const { return m_party; }
private:
	void initialize();

	bool m_online;
	bool m_cashShop; // No, we're not adding it at the moment
	uint8_t m_level;
	int16_t m_channel;
	int16_t m_job;
	int32_t m_id;
	int32_t m_map;
	ip_t m_ip;
	int64_t m_onlineTime;
	int64_t m_inviteTime;
	string m_name;
	Party *m_party;
};