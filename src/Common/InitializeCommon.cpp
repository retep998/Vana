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
#include "ExitCodes.h"
#include "MapleVersion.h"
#include <cstdio>
#include <iostream>

using std::cout;
using std::endl;

void Initializing::checkMcdbVersion() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM mcdb_info LIMIT 1");
	mysqlpp::StoreQueryResult res = query.store();

	if (res.num_rows() == 0) {
		cout << "ERROR: mcdb_info is empty." << endl;
		cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::McdbError);
	}

	int32_t version = (int32_t) res[0]["version"];
	int32_t subversion = (int32_t) res[0]["subversion"];
	int32_t mapleVersion = (int32_t) res[0]["maple_version"];
	bool testServer = (bool) res[0]["test_server"];
	string mapleLocale = (string) res[0]["maple_locale"];

	if (version != McdbVersion || subversion != McdbSubVersion) {
		cout << "ERROR: MCDB version imcompatible." << endl;
		cout << "Vana: " << McdbVersion << "." << McdbSubVersion << endl;
		cout << "MCDB: " << version << "." << subversion << endl;
		cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::McdbIncompatible);
	}
	if (mapleLocale != MapleVersion::LocaleString || testServer != MapleVersion::TestServer) {
		cout << "ERROR: Your MCDB is designed for different locale." << endl;
		cout << "Vana: " << makeLocale(MapleVersion::LocaleString, MapleVersion::TestServer) << endl;
		cout << "MCDB: " << makeLocale(mapleLocale, testServer) << endl;
		cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::McdbIncompatible);
	}
	if (mapleVersion != MapleVersion::Version) {
		cout << "WARNING: Your copy of MCDB is based on an incongruent version of the WZ files." << endl;
		cout << "Vana: " << MapleVersion::Version << endl;
		cout << "MCDB: " << mapleVersion << endl;
	}
}

string Initializing::makeLocale(string locale, bool testServer) {
	string loc = locale;
	if (testServer) {
		loc += " (test server)";
	}
	return loc;
}

void Initializing::checkSchemaVersion(bool update) {
	DatabaseMigration dbMigration(update);

	bool succeed = dbMigration.checkVersion();

	if (!succeed && !update) {
		// Wrong version and we're not allowed to update, so let's quit
		cout << "ERROR: Wrong version of database, please run Login Server to update." << endl;
		cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::InfoDatabaseError);
	}
	else if (!succeed) {
		// Failed, but we can update it
		cout << std::setw(outputWidth) <<  "Updating database...";

		dbMigration.update();

		cout << "DONE" << endl;
	}
}

void Initializing::setUsersOffline(int32_t onlineid) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = 0, characters.online = 0 WHERE users.online = " << onlineid;
	query.exec();
}
