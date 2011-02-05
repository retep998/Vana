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
#include "Alliance.h"
#include "Database.h"
#include "GameConstants.h"
#include "Guild.h"

Alliance::Alliance(int32_t id, const string &name, const string &notice, const GuildRanks &ranks, int32_t capacity, int32_t leader) :
m_id(id),
m_name(name),
m_notice(notice),
m_capacity(capacity),
m_titles(ranks),
m_leader(leader)
{
}

void Alliance::addGuild(Guild *guild) {
	m_guilds[guild->getId()] = guild;
}

void Alliance::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "UPDATE alliances SET " <<
		"notice = " << mysqlpp::quote << m_notice << ",";

	for (int32_t i = 1; i <= GuildsAndAlliances::RankQuantity; i++) {
		query << "rank" << i << "title = " << mysqlpp::quote << m_titles[i - 1] << ", ";
	}

	query << "capacity = " << m_capacity << ", " <<
		"leader = " << m_leader << " " <<
		"WHERE id = " << m_id;
	query.exec();
}

uint8_t Alliance::getLowestRank() {
	uint8_t retval = GuildsAndAlliances::RankQuantity;
	for (int32_t i = GuildsAndAlliances::RankQuantity; i > 0; i--) {
		if (!m_titles[i - 1].empty()) {
			retval = static_cast<uint8_t>(i); // Cast until I learn more...
			break;
		}
	}
	return retval;
}
