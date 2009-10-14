/*
Copyright (C) 2009 Vana Development Team

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
#ifndef ALLIANCE_H
#define ALLIANCE_H

#include "Guild.h"
#include "Types.h"
#include <boost/tr1/unordered_map.hpp>
#include <string>

using std::tr1::unordered_map;
using std::string;

class Guild;

class Alliance {
public:
	Alliance(int32_t id, string name, string notice, string title1, string title2, string title3, string title4, string title5, int32_t capacity, int32_t leader) : id(id),
	name(name),
	notice(notice),
	capacity(capacity),
	leader(leader) {
		title[0] = title1;
		title[1] = title2;
		title[2] = title3;
		title[3] = title4;
		title[4] = title5;
	};

	void setNotice(string note) { notice = note; }
	void setLeaderId(int32_t id) { leader = id; }
	void setCapacity(int32_t cap) { capacity = cap; }
	void setTitle(int32_t rank, string title) { this->title[rank] = title; }
	void addGuild(Guild *guild);
	void removeGuild(Guild *guild) { m_guilds.erase(guild->getId()); }
	void save();

	int32_t getId() const { return id; }
	string getName() const { return name; }
	string getNotice() const { return notice; }
	int32_t getCapacity() const { return capacity; }
	int32_t getLeaderId() const { return leader; }
	uint8_t getLowestRank();
	string getTitle(uint8_t rank) const { return title[rank]; }
	unordered_map<int32_t, Guild *> getGuilds() { return m_guilds; }
	Guild *getGuild(int32_t guildid) { return m_guilds.find(guildid) != m_guilds.end() ? m_guilds[guildid] : 0; }
	uint8_t getSize() const { return m_guilds.size(); }

private:
	int32_t id;
	int32_t leader;
	string name;
	string notice;
	boost::array<string, 5> title;
	int32_t capacity;
	unordered_map<int32_t, Guild *> m_guilds;
};

#endif
