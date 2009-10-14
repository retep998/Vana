/*
Copyright (C) 2008-2009 Vana Development Team

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

#ifndef GUILD_H
#define GUILD_H

#include "GuildBbs.h"
#include "Players.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using boost::scoped_ptr;
using std::string;
using std::tr1::unordered_map;

class GuildBbs;
struct Player;

class Guild {
public:
	Guild(string name, string notice, int32_t id, int32_t leaderid, int32_t capacity, int16_t logo, uint8_t logocolor, int16_t logobg, uint8_t logobgcolor, int32_t gp, string title1, string title2, string title3, string title4, string title5, int32_t allianceid);

	void addPlayer(Player *player);
	void save();
	void setAllianceId(int32_t id) { allianceid = id; }
	void setCapacity(int32_t cap) { capacity = cap; }
	void setGuildPoints(int32_t points) { this->points = points; }
	void setInvite(int32_t guildid);
	void setLogo(int16_t val) { logo = val; }
	void setLogoColor(uint8_t val) { logocolor = val; }
	void setLogoBg(int16_t val) { logobg = val; }
	void setLogoBgColor(uint8_t val) { logobgcolor = val; }
	void setNotice(string note) { notice = note; }
	void setTitle(uint8_t rank, string val) { title[rank-1] = val; }
	void removeInvite();
	void removePlayer(Player *player);

	int32_t getAllianceId() const { return allianceid; }
	GuildBbs *getBbs() const { return bbs.get(); }
	int32_t getCapacity() const { return capacity; }
	int32_t getGuildPoints() const { return points; }
	int32_t getId() const { return id; }
	bool getInvited() const { return invited; }
	int32_t getInvitedId() const { return invite_id; }
	int32_t getLeader() const { return leaderid; }
	int16_t getLogo() const { return logo; }
	uint8_t getLogoColor() const { return logocolor; }
	int16_t getLogoBg() const { return logobg; }
	uint8_t getLogoBgColor() const { return logobgcolor; }
	uint8_t getLowestRank();
	string getName() const { return name; }
	string getNotice() const { return notice; }
	string getTitle(uint8_t rank) const { return title[rank]; }

	unordered_map<int32_t, Player *> m_players;
private:
	string name;
	string notice;
	int32_t id;
	int32_t capacity;
	int32_t points;
	int16_t logo;
	uint8_t logocolor;
	int16_t logobg;
	uint8_t logobgcolor;
	int32_t leaderid;
	int32_t allianceid;
	boost::array<string, 5> title;
	
	bool invited;
	int32_t invite_id;
	time_t invite_time;

	scoped_ptr<GuildBbs> bbs;
};

#endif
