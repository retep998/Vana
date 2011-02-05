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

#include "GameObjects.h"
#include "Guild.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>

using std::tr1::unordered_map;
using std::string;

class Guild;

class Alliance {
public:
	Alliance(int32_t id, const string &name, const string &notice, const GuildRanks &ranks, int32_t capacity, int32_t leader);

	void setNotice(const string &note) { m_notice = note; }
	void setLeaderId(int32_t id) { m_leader = id; }
	void setCapacity(int32_t cap) { m_capacity = cap; }
	void setTitle(int32_t rank, const string &title) { m_titles[rank - 1] = title; }
	void setRanks(const GuildRanks &ranks) { m_titles = ranks; }
	void addGuild(Guild *guild);
	void removeGuild(Guild *guild) { m_guilds.erase(guild->getId()); }
	void save();

	int32_t getId() const { return m_id; }
	string getName() const { return m_name; }
	string getNotice() const { return m_notice; }
	int32_t getCapacity() const { return m_capacity; }
	int32_t getLeaderId() const { return m_leader; }
	uint8_t getLowestRank();
	string getTitle(uint8_t rank) const { return m_titles[rank - 1]; }
	unordered_map<int32_t, Guild *> getGuilds() { return m_guilds; }
	Guild * getGuild(int32_t guildid) { return m_guilds.find(guildid) != m_guilds.end() ? m_guilds[guildid] : nullptr; }
	uint8_t getSize() const { return m_guilds.size(); }
private:
	int32_t m_id;
	int32_t m_leader;
	string m_name;
	string m_notice;
	int32_t m_capacity;
	GuildRanks m_titles;
	unordered_map<int32_t, Guild *> m_guilds;
};
