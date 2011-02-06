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
#include "InitializeCommon.h"
#include "DatabaseMigration.h"
#include "Database.h"
#include "MapleVersion.h"
#include <cstdio>
#include <iostream>
#include <string>

using std::string;

void Initializing::checkMcdbVersion() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM mcdb_info LIMIT 1");
	mysqlpp::StoreQueryResult res = query.store();

	int32_t version = (int32_t) res[0]["version"];
	int32_t subversion = (int32_t) res[0]["subversion"];
	int32_t maple_version = (int32_t) res[0]["maple_version"];

	if (version != McdbVersion || subversion != McdbSubVersion) {
		// MCDB incompatible
		std::cout << "ERROR: MCDB version imcompatible. Expected: " << McdbVersion << "." << McdbSubVersion << " ";
		std::cout << "Have: " << version << "." << subversion << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(4);
	}

	if (maple_version != MAPLE_VERSION) {
		std::cout << "WARNING: Your copy of MCDB is based on an incongruent version of the WZ files. Vana: " << MAPLE_VERSION << " MCDB: " << maple_version << std::endl;
	}
}

void Initializing::checkSchemaVersion(bool update) {
	DatabaseMigration dbMigration(update);

	bool succeed = dbMigration.checkVersion();

	if (!succeed && !update) {
		// Wrong version and we're not allowed to update, so let's quit
		std::cout << "ERROR: Wrong version of database, please run Login Server to update." << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(4);
	}
	else if (!succeed) {
		// Failed, but we can update it
		std::cout << std::setw(outputWidth) << "Updating database...";

		dbMigration.update();

		std::cout << "DONE" << std::endl;
	}
}

void Initializing::setUsersOffline(int32_t onlineid) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = 0, characters.online = 0 WHERE users.online = " << onlineid;
	query.exec();
}
