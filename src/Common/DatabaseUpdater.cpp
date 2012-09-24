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
#include "DatabaseUpdater.h"
#include "Database.h"
#include "ExitCodes.h"
#include "MySqlQueryParser.h"
#include "StringUtilities.h"
#include "tokenizer.hpp"
#ifdef WIN32
#include <filesystem>
#else
// Until GCC supports the TR2 filesystem header
#include <boost/filesystem.hpp>
#endif
#include <iostream>

#ifdef WIN32
namespace fs = std::tr2::sys;
#else
namespace fs = boost::filesystem;
#endif

DatabaseUpdater::DatabaseUpdater(bool update) :
	m_sqlVersion(0),
	m_update(update)
{
	loadDatabaseInfo();
	loadSqlFiles();
}

// Returns true if the database is up-to-date.
bool DatabaseUpdater::checkVersion() {
	return m_sqlVersion <= m_version;
}

// Updates the database to the latest version
void DatabaseUpdater::update() {
	update(m_sqlVersion);
}

// Updates the database to the specified version
void DatabaseUpdater::update(size_t version) {
	if (version <= m_version) {
		// TODO: Throw exception
		return;
	}

	for (SqlFiles::iterator iter = m_sqlFiles.find(m_version + 1); iter != m_sqlFiles.end(); ++iter) {
		runQueries(iter->second);
	}

	updateInfoTable(version);
}

void DatabaseUpdater::loadDatabaseInfo() {
	// vana_info table for checking database version
	opt_int32_t version;
	bool retrievedData = false;

	try {
		soci::session &sql = Database::getCharDb();
		sql.once << "SELECT version FROM vana_info", soci::into(version);
		retrievedData = sql.got_data();
	}
	catch (soci::soci_error) { }

	if (!retrievedData) {
		if (m_update) {
			createInfoTable();
		}
		m_version = 0;
	}
	else {
		m_version = (!version.is_initialized() ? 0 : version.get());
	}
}

void DatabaseUpdater::loadSqlFiles() {
	fs::path fullPath = fs::system_complete(fs::path("sql"));
	if (!fs::exists(fullPath)) {
#ifdef WIN32
		std::cerr << "SQL files not found: " << fullPath.file_string() << std::endl;
#else
		std::cerr << "SQL files not found: " << fullPath.generic_string() << std::endl;
#endif
		ExitCodes::exit(ExitCodes::SqlDirectoryNotFound);
	}

	fs::directory_iterator end;
	for (fs::directory_iterator dir(fullPath); dir != end; ++dir) {
#ifdef WIN32
		const string &filename = dir->path().filename();
		const string &fileString = dir->path().file_string();
#else
		const string &filename = dir->path().filename().generic_string();
		const string &fileString = dir->path().generic_string();
#endif
		if (filename.find(".sql") == string::npos) {
			// Not an SQL file
			continue;
		}

		string version = filename;
		version.erase(version.find_first_of("_"));
		size_t v = StringUtilities::lexical_cast<size_t>(version);

		if (m_sqlVersion < v) {
			m_sqlVersion = v;
		}

		if (m_update) {
			// Only record the SQL files if we're going to update the database
			m_sqlFiles[v] = fileString;
		}
	}
}

// Create the info table
void DatabaseUpdater::createInfoTable() {
	soci::session &sql = Database::getCharDb();
	sql.once << "CREATE TABLE IF NOT EXISTS vana_info (version INT UNSIGNED)";
	sql.once << "INSERT INTO vana_info VALUES (NULL)";
}

// Set version number in the info table
void DatabaseUpdater::updateInfoTable(size_t version) {
	Database::getCharDb().once << "UPDATE vana_info SET version = :version", soci::use(version, "version");
}

void DatabaseUpdater::runQueries(const string &filename) {
	soci::session &sql = Database::getCharDb();
	const std::vector<string> &queries = MySqlQueryParser::parseQueries(filename);

	// Run them
	for (auto i = queries.begin(); i != queries.end(); ++i) {
		try {
			sql.once << *i;
		}
		catch (soci::soci_error &e) {
			std::cerr << "\nERROR: " << e.what() << std::endl;
			std::cerr << "File: " << filename << std::endl;
			// TODO: Handle the error
		}
	}
}