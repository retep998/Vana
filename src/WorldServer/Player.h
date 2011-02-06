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

class Alliance;
class Guild;
class Party;

class Player {
public:
	Player();
	Player(int32_t id);
	void setOnline(bool isOnline) { online = isOnline; }
	void setIp(uint32_t newIp) { ip = newIp; }
	void setChannel(int16_t newChannel) { channel = newChannel; }
	void setOnlineTime(int64_t newTime) { onlineTime = newTime; }
	void setMap(int32_t mapId) { map = mapId; }
	void setJob(int16_t jobId) { job = jobId; }
	void setLevel(uint8_t level) { this->level = level; }
	void setName(const string &newName) { name = newName; }
	void setCashShop(bool state) { cashShop = state; }
	bool isInCashShop() const { return cashShop; }
	bool isOnline() const { return online; }
	uint8_t getLevel() const { return level; }
	int16_t getChannel() const { return channel; }
	int16_t getJob() const { return job; }
	int32_t getMap() const { return map; }
	int32_t getId() const { return id; }
	uint32_t getIp() const { return ip; }
	int64_t getOnlineTime() const { return onlineTime; }
	string getName() const { return name; }

	// Party
	void setParty(Party *party) { this->party = party; }
	Party * getParty() const { return party; }

	// Guild
	void setGuild(Guild *guild) { this->guild = guild; }
	void setGuildRank(uint8_t newRank) { guildRank = newRank; }
	void setGuildInviteId(int32_t guildId) { inviteGuild = guildId; }
	void setGuildInviteTime(int64_t time) { inviteTime = time; }
	bool isInvitedToGuild() const { return (inviteGuild != 0); }
	uint8_t getGuildRank() const { return guildRank; }
	int32_t getGuildInviteId() const { return inviteGuild; }
	int64_t getGuildInviteTime() const { return inviteTime; }
	Guild * getGuild() const { return guild; }

	// Alliance
	void setAllianceRank(uint8_t newRank) { allianceRank = newRank; }
	void setAlliance(Alliance *alliance) { this->alliance = alliance; }
	uint8_t getAllianceRank() const { return allianceRank; }
	Alliance * getAlliance() const { return alliance; }
private:
	void initialize();

	bool online;
	bool cashShop;
	uint8_t guildRank;
	uint8_t allianceRank;
	uint8_t level;
	int16_t channel;
	int16_t job;
	int32_t id;
	int32_t map;
	int32_t inviteGuild;
	uint32_t ip;
	int64_t onlineTime;
	int64_t inviteTime;
	string name;
	Party *party;
	Guild *guild;
	Alliance *alliance;
};
