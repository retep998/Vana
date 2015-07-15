/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "InitializeCommon.hpp"
#include "Database.hpp"
#include "DatabaseUpdater.hpp"
#include "ExitCodes.hpp"
#include "MapleVersion.hpp"
#include "TimeUtilities.hpp"
#include <cstdio>
#include <iomanip>
#include <iostream>

auto Initializing::checkMcdbVersion() -> Result {
	soci::session &sql = Database::getDataDb();
	soci::row row;
	sql.once
		<< "SELECT * "
		<< "FROM " << Database::makeDataTable("mcdb_info"),
		soci::into(row);

	if (!sql.got_data()) {
		std::cerr << "ERROR: mcdb_info is empty." << std::endl;
		ExitCodes::exit(ExitCodes::McdbError);
		return Result::Failure;
	}

	int32_t version = row.get<int32_t>("version");
	int32_t subversion = row.get<int32_t>("subversion");
	version_t mapleVersion = row.get<version_t>("maple_version");
	bool testServer = row.get<bool>("test_server");
	string_t mapleLocale = row.get<string_t>("maple_locale");

	if (version != McdbVersion || subversion != McdbSubVersion) {
		std::cerr << "ERROR: MCDB version incompatible." << std::endl;
		std::cerr << "Vana: " << McdbVersion << "." << McdbSubVersion << std::endl;
		std::cerr << "MCDB: " << version << "." << subversion << std::endl;
		ExitCodes::exit(ExitCodes::McdbIncompatible);
		return Result::Failure;
	}

	auto makeLocale = [](const string_t &locale, bool testServer) {
		string_t loc = locale;
		if (testServer) {
			loc += " (test server)";
		}
		return loc;
	};

	if (mapleLocale != MapleVersion::LocaleString || testServer != MapleVersion::TestServer) {
		std::cerr << "ERROR: Your MCDB is designed for different locale." << std::endl;
		std::cerr << "Vana: " << makeLocale(MapleVersion::LocaleString, MapleVersion::TestServer) << std::endl;
		std::cerr << "MCDB: " << makeLocale(mapleLocale, testServer) << std::endl;
		ExitCodes::exit(ExitCodes::McdbLocaleIncompatible);
		return Result::Failure;
	}

	if (mapleVersion != MapleVersion::Version) {
		std::cerr << "WARNING: Your copy of MCDB is based on an incongruent version of the WZ files." << std::endl;
		std::cerr << "Vana: " << MapleVersion::Version << std::endl;
		std::cerr << "MCDB: " << mapleVersion << std::endl;
	}

	return Result::Successful;
}

auto Initializing::checkSchemaVersion(bool update) -> Result {
	DatabaseUpdater db{update};

	VersionCheckResult check = db.checkVersion();

	if (check == VersionCheckResult::DatabaseUnavailable) {
		std::cerr << "ERROR: Vana database is currently inaccessible." << std::endl;
		ExitCodes::exit(ExitCodes::InfoDatabaseError);
		return Result::Failure;
	}

	if (check == VersionCheckResult::NeedsUpdate) {
		if (!update) {
			// Wrong version and we're not allowed to update, so let's quit
			std::cerr << "ERROR: Wrong version of database, please run LoginServer to update." << std::endl;
			ExitCodes::exit(ExitCodes::InfoDatabaseError);
			return Result::Failure;
		}
		else {
			// Failed, but we can update it
			std::cout << std::setw(OutputWidth) << "Updating database...";

			db.update();

			std::cout << "DONE" << std::endl;
		}
	}

	return Result::Successful;
}

auto Initializing::setUsersOffline(int32_t onlineId) -> void {
	std::cout << "Resetting online status of players..." << std::endl;
	time_point_t startTime = TimeUtilities::getNow();

	Database::getCharDb().once
		<< "UPDATE " << Database::makeCharTable("user_accounts") << " u "
		<< "INNER JOIN " << Database::makeCharTable("characters") << " c ON u.user_id = c.user_id "
		<< "SET "
		<< "	u.online = 0,"
		<< "	c.online = 0 "
		<< "WHERE u.online = :online",
		soci::use(onlineId, "online");

	auto loadingTime = TimeUtilities::getDistance<milliseconds_t>(TimeUtilities::getNow(), startTime);
	std::cout << "Reset all accounts and players in " << std::setprecision(3) << loadingTime / 1000.f << " seconds!" << std::endl << std::endl;
}