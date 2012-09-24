/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "Database.h"
#include "DatabaseUpdater.h"
#include "ExitCodes.h"
#include "MapleVersion.h"
#include "TimeUtilities.h"
#include <cstdio>
#include <iomanip>
#include <iostream>

using std::endl;

void Initializing::checkMcdbVersion() {
	soci::session &sql = Database::getDataDb();
	soci::row row;
	sql.once << "SELECT * FROM mcdb_info", soci::into(row);

	if (!sql.got_data()) {
		std::cerr << "ERROR: mcdb_info is empty." << endl;
		ExitCodes::exit(ExitCodes::McdbError);
	}

	int32_t version = row.get<int32_t>("version");
	int32_t subversion = row.get<int32_t>("subversion");
	int16_t mapleVersion = row.get<int16_t>("maple_version");
	bool testServer = row.get<bool>("test_server");
	const string &mapleLocale = row.get<string>("maple_locale");

	if (version != McdbVersion || subversion != McdbSubVersion) {
		std::cerr << "ERROR: MCDB version incompatible." << endl;
		std::cerr << "Vana: " << McdbVersion << "." << McdbSubVersion << endl;
		std::cerr << "MCDB: " << version << "." << subversion << endl;
		ExitCodes::exit(ExitCodes::McdbIncompatible);
	}

	if (mapleLocale != MapleVersion::LocaleString || testServer != MapleVersion::TestServer) {
		std::cerr << "ERROR: Your MCDB is designed for different locale." << endl;
		std::cerr << "Vana: " << makeLocale(MapleVersion::LocaleString, MapleVersion::TestServer) << endl;
		std::cerr << "MCDB: " << makeLocale(mapleLocale, testServer) << endl;
		ExitCodes::exit(ExitCodes::McdbLocaleIncompatible);
	}
	if (mapleVersion != MapleVersion::Version) {
		std::cerr << "WARNING: Your copy of MCDB is based on an incongruent version of the WZ files." << endl;
		std::cerr << "Vana: " << MapleVersion::Version << endl;
		std::cerr << "MCDB: " << mapleVersion << endl;
	}
}

string Initializing::makeLocale(const string &locale, bool testServer) {
	string loc = locale;
	if (testServer) {
		loc += " (test server)";
	}
	return loc;
}

void Initializing::checkSchemaVersion(bool update) {
	DatabaseUpdater db(update);

	bool succeed = db.checkVersion();

	if (!succeed && !update) {
		// Wrong version and we're not allowed to update, so let's quit
		std::cerr << "ERROR: Wrong version of database, please run LoginServer to update." << endl;
		ExitCodes::exit(ExitCodes::InfoDatabaseError);
	}
	else if (!succeed) {
		// Failed, but we can update it
		std::cout << std::setw(OutputWidth) << "Updating database...";

		db.update();

		std::cout << "DONE" << endl;
	}
}

void Initializing::setUsersOffline(int32_t onlineId) {
	std::cout << "Resetting online status of players..." << std::endl;
	clock_t startTime = TimeUtilities::getTickCount();

	Database::getCharDb().once
		<< "UPDATE user_accounts u "
		<< "INNER JOIN characters c ON u.user_id = c.user_id "
		<< "SET "
		<< "	u.online = 0,"
		<< "	c.online = 0 "
		<< "WHERE u.online = :online",
		soci::use(onlineId, "online");

	float loadingTime = (TimeUtilities::getTickCount() - startTime) / 1000.0f;
	std::cout << "Reset all accounts and players in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl << std::endl;
}