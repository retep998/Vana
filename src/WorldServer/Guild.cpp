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
#include "Guild.h"
#include "GuildBbs.h"
#include "Database.h"
#include "Players.h"

Guild::Guild(string name, string notice, int32_t id, int32_t leaderid, int32_t capacity, int16_t logo, uint8_t logocolor, int16_t logobg, uint8_t logobgcolor, int32_t gp, string title1, string title2, string title3, string title4, string title5, int32_t allianceid) : id(id),
name(name),
notice(notice),
capacity(capacity),
leaderid(leaderid),
logo(logo),
logocolor(logocolor),
logobg(logobg),
logobgcolor(logobgcolor),
points(gp),
allianceid(allianceid),
invited(false) {
	title[0] = title1;
	title[1] = title2;
	title[2] = title3;
	title[3] = title4;
	title[4] = title5;
	bbs.reset(new GuildBbs(this));
};

void Guild::addPlayer(Player *player) {
	if (player == 0) 
		return;

	m_players[player->id] = player;
}

void Guild::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "UPDATE guilds SET " <<
		"notice = " << mysqlpp::quote << notice << ", " <<
		"capacity = " << capacity << ", " <<
		"logo = " << logo << ", " <<
		"logocolor = " << static_cast<int16_t>(logocolor) << ", " <<
		"logobg = " << logobg << ", " <<
		"logobgcolor = " << static_cast<int16_t>(logobgcolor) << ", " <<
		"gp = " << points << ", " <<
		"rank1title = " << mysqlpp::quote << title[0] << ", " <<
		"rank2title = " << mysqlpp::quote << title[1] << ", " <<
		"rank3title = " << mysqlpp::quote << title[2] << ", " <<
		"rank4title = " << mysqlpp::quote << title[3] << ", " <<
		"rank5title = " << mysqlpp::quote << title[4] << ", " <<
		"leaderid = " << leaderid << " " <<
		"WHERE id = " << id << " LIMIT 1";
	query.exec();
}

void Guild::setInvite(int32_t guildid) {
	invited = true;
	invite_id = guildid;
	invite_time = time(0);
}

void Guild::removeInvite() {
	invited = false;
	invite_id = 0;
	invite_time = 0;
}

void Guild::removePlayer(Player *player) {
	if (player == 0) 
		return;
	if (m_players.find(player->id) == m_players.end())
		return;

	m_players.erase(player->id);
}

uint8_t Guild::getLowestRank() {
	if (!title[3].empty()) {
		if (!title[4].empty()) {
			return 5; // ! title[4] = rank 5, not 4 !
		}
		else {
			return 4;
		}
	}
	else {
		return 3;
	}
}
