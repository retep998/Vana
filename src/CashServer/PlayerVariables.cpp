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
#include "PlayerVariables.h"
#include "Database.h"
#include "Player.h"

PlayerVariables::PlayerVariables(Player *p) : player(p) {
	load();
}

void PlayerVariables::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM character_variables WHERE charid = " << player->getId();
	query.exec();

	if (m_variables.size() > 0) {
		bool firstrun = true;
		for (unordered_map<string, string>::iterator iter = m_variables.begin(); iter != m_variables.end(); iter++) {
			if (firstrun) {
				query << "INSERT INTO character_variables VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << player->getId() << ","
					<< mysqlpp::quote << iter->first << ","
					<< mysqlpp::quote << iter->second << ")";
		}
		query.exec();
	}
}

void PlayerVariables::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM character_variables WHERE charid = " << player->getId();
	mysqlpp::StoreQueryResult res = query.store();
	for (size_t i = 0; i < res.size(); i++) {
		m_variables[(string) res[i]["key"]] = string(res[i]["value"]);
	}
}
