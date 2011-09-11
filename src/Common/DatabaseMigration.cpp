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
#include "DatabaseMigration.h"
#include "Database.h"
#include "DatabaseMigrationRunner.h"
#include "ExitCodes.h"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp> 
#include <iostream>

namespace fs = boost::filesystem;

DatabaseMigration::DatabaseMigration(bool update) :
	m_sqlVersion(0),
	m_update(update)
{
	loadDatabaseInfo();
	loadSqlFiles();
}

// Returns true if the database is up-to-date.
bool DatabaseMigration::checkVersion() {
	return m_sqlVersion <= m_version;
}

// Updates the database to the latest version
void DatabaseMigration::update() {
	update(m_sqlVersion);
}

// Updates the database to the specified version
void DatabaseMigration::update(size_t version) {
	if (version <= m_version) {
		// TODO: Throw exception
		return;
	}

	for (SqlFiles::iterator iter = m_sqlFiles.find(m_version + 1); iter != m_sqlFiles.end(); ++iter) {
		Runner runner(iter->second);
		runner.run();
	}

	updateInfoTable(version);
}

void DatabaseMigration::loadDatabaseInfo() {
	// vana_info table for checking database version
	mysqlpp::Connection &conn = Database::getCharDb();
	mysqlpp::StoreQueryResult res;
	{
		mysqlpp::NoExceptions ne(conn);
		mysqlpp::Query query = Database::getCharDb().query("SELECT * FROM vana_info LIMIT 1");
		res = query.store();
	}

	if (res.size() == 0) {
		if (m_update) {
			createInfoTable();
		}
		m_version = 0;
	}
	else {
		m_version = res[0][0].is_null() ? 0 : (int32_t) res[0][0];
	}
}

void DatabaseMigration::loadSqlFiles() {
	fs::path fullPath = fs::system_complete(fs::path("sql", fs::native));
	if (!fs::exists(fullPath)) {
		std::cerr << "SQL files not found: " << fullPath.native_file_string() << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::InfoDatabaseError);
	}

	fs::directory_iterator end;
	for (fs::directory_iterator dir(fullPath); dir != end; ++dir) {
		string &filename = dir->filename();
		string &filestring = dir->path().native_file_string();
		if (filename.find(".sql") == string::npos) {
			// Not an SQL file
			continue;
		}

		string version = filename;
		version.erase(version.find_first_of("_"));
		size_t v = boost::lexical_cast<size_t>(version);

		if (m_sqlVersion < v) {
			m_sqlVersion = v;
		}

		if (m_update) {
			// Only record the SQL files if we're going to update the database
			m_sqlFiles[v] = filestring;
		}
	}
}

// Create the info table
void DatabaseMigration::createInfoTable() {
	mysqlpp::Query query = Database::getCharDb().query();

	query << "CREATE TABLE IF NOT EXISTS vana_info (version INT UNSIGNED)";
	query.exec();

	// Insert a default record of NULL
	query << "INSERT INTO vana_info VALUES (NULL)";
	query.exec();
}

// Set version number in the info table
void DatabaseMigration::updateInfoTable(size_t version) {
	mysqlpp::Query query = Database::getCharDb().query();

	query << "UPDATE vana_info SET version = " << version;
	query.exec();
}