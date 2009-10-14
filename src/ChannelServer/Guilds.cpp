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

#include "Guilds.h"
#include "Database.h"

Guilds * Guilds::singleton = 0;

void Guilds::addGuild(int32_t gid, Guild * gi) {
	m_guilds[gid] = gi;
	m_guilds_names[gi->name] = gi;
}

Guild * Guilds::getGuild(int32_t gid) {
	return (m_guilds.find(gid) == m_guilds.end() ? 0 : m_guilds[gid]);
}

Guild * Guilds::getGuild(string name) {
	return (m_guilds_names.find(name) == m_guilds_names.end() ? 0 : m_guilds_names[name]);
}

void Guilds::addGuild(int32_t id, string name, int16_t logo, uint8_t logocolor, int16_t logobg, uint8_t logobgcolor, int32_t capacity, int32_t allianceid) {
	Guild * gi = new Guild(id, name, logo, logocolor, logobg, logobgcolor, capacity, allianceid);
	m_guilds[id] = gi;
}

void Guilds::loadGuild(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "SELECT name, logo, logocolor, logobg, logobgcolor, capacity, alliance FROM guilds WHERE id = " << id << " LIMIT 1";
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW guildRow = res.fetch_raw_row();
	addGuild(id, 
		(string) guildRow[0], 
		atoi(guildRow[1]), 
		atoi(guildRow[2]), 
		atoi(guildRow[3]), 
		atoi(guildRow[4]), 
		atoi(guildRow[5]),
		atoi(guildRow[6]));
}

void Guilds::unloadGuild(int32_t id) {
	if (m_guilds.find(id) == m_guilds.end())
		return;
	m_guilds_names.erase(m_guilds[id]->name);
	delete m_guilds[id];
	m_guilds.erase(id);
}

bool Guilds::hasEmblem(int32_t id) {
	if (m_guilds.find(id) == m_guilds.end())
		return false;
	Guild *g = m_guilds[id];
	if (g->logo != 0 || g->logocolor != 0 || g->logobg != 0 || g->logobgcolor != 0)
		return true;
	else
		return false;
}
