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
#include "Guild.h"
#include "GameConstants.h"
#include "GuildBbs.h"
#include "Database.h"
#include "Player.h"

Guild::Guild(const string &name, const string &notice, int32_t id, int32_t leaderid, int32_t capacity, int32_t gp, const GuildLogo &logo, const GuildRanks &ranks, Alliance *alliance) :
m_id(id),
m_name(name),
m_notice(notice),
m_capacity(capacity),
m_leaderId(leaderid),
m_logo(logo),
m_points(gp),
m_alliance(alliance),
m_titles(ranks),
m_invited(false)
{
	m_bbs.reset(new GuildBbs(this));
};

void Guild::addPlayer(Player *player) {
	if (player == nullptr)
		return;

	m_players[player->getId()] = player;
}

void Guild::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "UPDATE guilds SET " <<
		"notice = " << mysqlpp::quote << m_notice << ", " <<
		"capacity = " << m_capacity << ", " <<
		"logo = " << m_logo.logo << ", " <<
		"logocolor = " << static_cast<int16_t>(m_logo.color) << ", " <<
		"logobg = " << m_logo.background << ", " <<
		"logobgcolor = " << static_cast<int16_t>(m_logo.backgroundColor) << ", " <<
		"points = " << m_points << ", ";

	for (int32_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++) {
		query << "rank" << i << "title = " << mysqlpp::quote << m_titles[i - 1] << ", ";
	}

	query << "leaderid = " << m_leaderId << " " <<
		"WHERE id = " << m_id << " LIMIT 1";
	query.exec();
}

void Guild::setInvite(int32_t guildid) {
	m_invited = true;
	m_inviteId = guildid;
	m_inviteTime = time(0);
}

void Guild::removeInvite() {
	m_invited = false;
	m_inviteId = 0;
	m_inviteTime = 0;
}

void Guild::removePlayer(Player *player) {
	if (player == nullptr)
		return;
	if (m_players.find(player->getId()) == m_players.end())
		return;

	m_players.erase(player->getId());
}

uint8_t Guild::getLowestRank() {
	uint8_t retval = GuildsAndAlliances::RankQuantity;
	for (int32_t i = GuildsAndAlliances::RankQuantity; i > 0; i--) {
		if (!m_titles[i - 1].empty()) {
			retval = static_cast<uint8_t>(i); // Cast until I learn more...
			break;
		}
	}
	return retval;
}
