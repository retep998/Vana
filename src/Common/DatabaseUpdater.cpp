/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "DatabaseUpdater.hpp"
#include "Database.hpp"
#include "ExitCodes.hpp"
#include "MySqlQueryParser.hpp"
#include "StringUtilities.hpp"
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
	m_update(update)
{
	loadDatabaseInfo();
	loadSqlFiles();
}

auto DatabaseUpdater::checkVersion() -> VersionCheckResult {
	return m_sqlVersion <= m_fileVersion ? VersionCheckResult::FullyUpdated : VersionCheckResult::NeedsUpdate;
}

auto DatabaseUpdater::update() -> void {
	update(m_sqlVersion);
}

auto DatabaseUpdater::update(size_t version) -> void {
	if (version <= m_fileVersion) {
		throw std::out_of_range("SQL version to update to is less than the highest query file");
	}

	for (auto iter = m_sqlFiles.find(m_fileVersion + 1); iter != std::end(m_sqlFiles); ++iter) {
		runQueries(iter->second);
	}

	updateInfoTable(version);
}

auto DatabaseUpdater::loadDatabaseInfo() -> void {
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
		m_fileVersion = 0;
	}
	else {
		m_fileVersion = version.getOrDefault(0);
	}
}

auto DatabaseUpdater::loadSqlFiles() -> void {
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
		string_t filename = dir->path().filename();
		string_t fileString = (fullPath / dir->path()).file_string();
#else
		string_t filename = dir->path().filename().generic_string();
		string_t fileString = dir->path().generic_string();
#endif
		if (filename.find(".sql") == string_t::npos) {
			// Not an SQL file
			continue;
		}

		string_t version = filename;
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
auto DatabaseUpdater::createInfoTable() -> void {
	soci::session &sql = Database::getCharDb();
	sql.once << "CREATE TABLE IF NOT EXISTS vana_info (version INT UNSIGNED)";
	sql.once << "INSERT INTO vana_info VALUES (NULL)";
}

// Set version number in the info table
auto DatabaseUpdater::updateInfoTable(size_t version) -> void {
	Database::getCharDb().once << "UPDATE vana_info SET version = :version", soci::use(version, "version");
}

auto DatabaseUpdater::runQueries(const string_t &filename) -> void {
	soci::session &sql = Database::getCharDb();
	vector_t<string_t> queries = MySqlQueryParser::parseQueries(filename);

	// Run them
	for (const auto &query : queries) {
		try {
			sql.once << query;
		}
		catch (soci::soci_error &e) {
			std::cerr << "\nQUERY ERROR: " << e.what() << std::endl;
			std::cerr << "File: " << filename << std::endl;
			ExitCodes::exit(ExitCodes::QueryError);
		}
	}
}