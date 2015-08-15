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
#include "AbstractServer.hpp"
#include "Database.hpp"
#include "DatabaseUpdater.hpp"
#include "ExitCodes.hpp"
#include "MapleVersion.hpp"
#include "TimeUtilities.hpp"
#include <cstdio>
#include <iomanip>
#include <iostream>

auto Initializing::checkMcdbVersion(AbstractServer *server) -> Result {
	soci::row row;

	try {
		auto &db = Database::getDataDb();
		auto &sql = db.getSession();
		if (!db.tableExists("mcdb_info")) {
			server->log(LogType::CriticalError, "ERROR: mcdb_info does not exist.");
			ExitCodes::exit(ExitCodes::McdbError);
			return Result::Failure;
		}

		sql.once
			<< "SELECT * "
			<< "FROM " << db.makeTable("mcdb_info"),
			soci::into(row);

		if (!sql.got_data()) {
			server->log(LogType::CriticalError, "ERROR: mcdb_info is empty.");
			ExitCodes::exit(ExitCodes::McdbError);
			return Result::Failure;
		}
	}
	catch (soci::soci_error &e) {
		server->log(LogType::CriticalError, string_t{"ERROR: "} + e.what());
		ExitCodes::exit(ExitCodes::McdbError);
		return Result::Failure;
	}

	int32_t version = row.get<int32_t>("version");
	int32_t subversion = row.get<int32_t>("subversion");
	version_t mapleVersion = row.get<version_t>("maple_version");
	bool testServer = row.get<bool>("test_server");
	string_t mapleLocale = row.get<string_t>("maple_locale");

	if (version != McdbVersion || subversion != McdbSubVersion) {
		server->log(LogType::CriticalError, [&](out_stream_t &str) {
			str
				<< "ERROR: MCDB version incompatible." << std::endl
				<< "Vana: " << McdbVersion << "." << McdbSubVersion << std::endl
				<< "MCDB: " << version << "." << subversion;
		});
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
		server->log(LogType::CriticalError, [&](out_stream_t &str) {
			str
			<< "ERROR: Your MCDB is designed for different locale." << std::endl
			<< "Vana: " << makeLocale(MapleVersion::LocaleString, MapleVersion::TestServer) << std::endl
			<< "MCDB: " << makeLocale(mapleLocale, testServer);
		});
		ExitCodes::exit(ExitCodes::McdbLocaleIncompatible);
		return Result::Failure;
	}

	if (mapleVersion != MapleVersion::Version) {
		server->log(LogType::Warning, [&](out_stream_t &str) {
			str
				<< "WARNING: Your copy of MCDB is based on an incongruent version of the WZ files." << std::endl
				<< "Vana: " << MapleVersion::Version << std::endl
				<< "MCDB: " << mapleVersion;
		});
	}

	return Result::Successful;
}

auto Initializing::checkSchemaVersion(AbstractServer *server, bool update) -> Result {
	DatabaseUpdater db{server, update};

	VersionCheckResult check = db.checkVersion();

	if (check == VersionCheckResult::DatabaseUnavailable) {
		server->log(LogType::CriticalError, "ERROR: Vana database is currently inaccessible.");
		ExitCodes::exit(ExitCodes::InfoDatabaseError);
		return Result::Failure;
	}

	if (check == VersionCheckResult::NeedsUpdate) {
		if (!update) {
			// Wrong version and we're not allowed to update, so let's quit
			server->log(LogType::CriticalError, "ERROR: Wrong version of database, please run LoginServer to update.");
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

auto Initializing::setUsersOffline(AbstractServer *server, int32_t onlineId) -> void {
	std::cout << "Resetting online status of players..." << std::endl;
	time_point_t startTime = TimeUtilities::getNow();

	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	sql.once
		<< "UPDATE " << db.makeTable("user_accounts") << " u "
		<< "INNER JOIN " << db.makeTable("characters") << " c ON u.user_id = c.user_id "
		<< "SET "
		<< "	u.online = 0,"
		<< "	c.online = 0 "
		<< "WHERE u.online = :online",
		soci::use(onlineId, "online");

	auto loadingTime = TimeUtilities::getDistance<milliseconds_t>(TimeUtilities::getNow(), startTime);
	std::cout << "Reset all accounts and players in " << std::setprecision(3) << loadingTime / 1000.f << " seconds!" << std::endl << std::endl;
}