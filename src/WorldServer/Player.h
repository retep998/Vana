/*
Copyright (C) 2008-2011 Vana Development Team

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
	void setOnline(bool isOnline) { online = isOnline; }
	void setIp(ip_t newIp) { ip = newIp; }
	void setChannel(int16_t newChannel) { channel = newChannel; }
	void setOnlineTime(int64_t newTime) { onlineTime = newTime; }
	void setMap(int32_t mapId) { map = mapId; }
	void setJob(int16_t jobId) { job = jobId; }
	void setLevel(uint8_t level) { this->level = level; }
	void setName(const string &newName) { name = newName; }
	bool isOnline() const { return online; }
	uint8_t getLevel() const { return level; }
	int16_t getChannel() const { return channel; }
	int16_t getJob() const { return job; }
	int32_t getMap() const { return map; }
	int32_t getId() const { return id; }
	ip_t getIp() const { return ip; }
	int64_t getOnlineTime() const { return onlineTime; }
	string getName() const { return name; }

	// Party
	void setParty(Party *party) { this->party = party; }
	Party * getParty() const { return party; }
private:
	void initialize();

	bool online;
	bool cashShop; // No, we're not adding it at the moment
	uint8_t level;
	int16_t channel;
	int16_t job;
	int32_t id;
	int32_t map;
	ip_t ip;
	int64_t onlineTime;
	int64_t inviteTime;
	string name;
	Party *party;
};