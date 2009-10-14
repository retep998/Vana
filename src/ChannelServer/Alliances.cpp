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

#include "Alliances.h"
#include "Database.h"

Alliances * Alliances::singleton = 0;

void Alliances::addAlliance(int32_t id, Alliance * alliance) {
	m_alliances[id] = alliance;
	m_alliances_names[alliance->name] = alliance;
}

Alliance * Alliances::getAlliance(int32_t id) {
	return (m_alliances.find(id) == m_alliances.end() ? 0 : m_alliances[id]);
}

Alliance * Alliances::getAlliance(string name) {
	return (m_alliances_names.find(name) == m_alliances_names.end() ? 0 : m_alliances_names[name]);
}

void Alliances::addAlliance(int32_t id, string name, int32_t capacity) {
	Alliance * alliance = new Alliance(id, name, capacity);

	m_alliances[id] = alliance;
}

void Alliances::loadAlliance(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "SELECT name, capacity FROM alliances WHERE id = " << id << " LIMIT 1";
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW allianceRow = res.fetch_raw_row();
	addAlliance(id, (string) allianceRow[0], atoi(allianceRow[1]));
}

void Alliances::unloadAlliance(int32_t id) {
	if (m_alliances.find(id) == m_alliances.end())
		return;
	m_alliances_names.erase(m_alliances[id]->name);
	delete m_alliances[id];
	m_alliances.erase(id);
}
