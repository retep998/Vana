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
#include "GuildBbs.h"
#include "Types.h"
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>

using boost::scoped_ptr;
using std::string;
using std::tr1::unordered_map;

class Alliance;
class GuildBbs;
class Player;

class Guild {
public:
	Guild(const string &name, const string &notice, int32_t id, int32_t leaderid, int32_t capacity, int32_t gp, const GuildLogo &logo, const GuildRanks &ranks, Alliance *alliance);

	void addPlayer(Player *player);
	void save();
	void setCapacity(int32_t cap) { m_capacity = cap; }
	void setGuildPoints(int32_t points) { m_points = points; }
	void setInvite(int32_t guildid);
	void setLogo(const GuildLogo &newLogo) { m_logo = newLogo; }
	void setNotice(const string &note) { m_notice = note; }
	void setRanks(const GuildRanks &ranks) { m_titles = ranks; }
	void setTitle(uint8_t rank, const string &val) { m_titles[rank - 1] = val; }
	void removeInvite();
	void removePlayer(Player *player);
	void setAlliance(Alliance *alliance) { m_alliance = alliance; }

	bool getInvited() const { return m_invited; }
	uint8_t getLowestRank();
	int32_t getCapacity() const { return m_capacity; }
	int32_t getGuildPoints() const { return m_points; }
	int32_t getId() const { return m_id; }
	int32_t getInvitedId() const { return m_inviteId; }
	int32_t getLeader() const { return m_leaderId; }
	string getName() const { return m_name; }
	string getNotice() const { return m_notice; }
	string getTitle(uint8_t rank) const { return m_titles[rank - 1]; }
	const GuildLogo & getLogo() const { return m_logo; }
	GuildBbs * getBbs() const { return m_bbs.get(); }
	Alliance * getAlliance() const { return m_alliance; }

	unordered_map<int32_t, Player *> m_players;
private:
	bool m_invited;
	int32_t m_id;
	int32_t m_capacity;
	int32_t m_points;
	int32_t m_leaderId;
	int32_t m_inviteId;
	time_t m_inviteTime;
	string m_name;
	string m_notice;
	GuildLogo m_logo;
	GuildRanks m_titles;
	Alliance *m_alliance;
	scoped_ptr<GuildBbs> m_bbs;
};
