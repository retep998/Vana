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
#include "DatabaseUpdater.hpp"
#include "AbstractServer.hpp"
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

namespace Vana {

#ifdef WIN32
namespace fs = std::tr2::sys;
#else
namespace fs = boost::filesystem;
#endif

DatabaseUpdater::DatabaseUpdater(AbstractServer *server, bool update) :
	m_server{server},
	m_update{update}
{
	loadDatabaseInfo();
	auto files = loadSqlFiles();
	m_sqlVersion = files.first;
}

auto DatabaseUpdater::checkVersion() -> VersionCheckResult {
	if (!m_dbAvailable) {
		return VersionCheckResult::DatabaseUnavailable;
	}
	return m_sqlVersion <= m_fileVersion ?
		VersionCheckResult::FullyUpdated :
		VersionCheckResult::NeedsUpdate;
}

auto DatabaseUpdater::update() -> void {
	update(m_sqlVersion);
}

auto DatabaseUpdater::update(size_t version) -> void {
	if (version <= m_fileVersion) {
		throw std::out_of_range{"SQL version to update to is less than the highest query file"};
	}

	auto files = loadSqlFiles().second;
	for (auto iter = files.find(m_fileVersion + 1); iter != std::end(files); ++iter) {
		runQueries(iter->second);
	}

	updateInfoTable(version);
}

auto DatabaseUpdater::loadDatabaseInfo() -> void {
	// vana_info table for checking database version
	opt_int32_t version;
	bool retrievedData = false;

	try {
		auto &db = m_update ?
			Database::initCharDb() :
			Database::getCharDb();
		m_dbAvailable = true;
	}
	catch (soci::soci_error &) {
		return;
	}

	try {
		auto &db = Database::getCharDb();
		auto &sql = db.getSession();
		if (Database::tableExists(sql, db.getSchema(), db.makeTable("vana_info"))) {
			sql.once
				<< "SELECT version "
				<< "FROM " << db.makeTable("vana_info"),
				soci::into(version);

			retrievedData = sql.got_data();
		}
	}
	catch (soci::soci_error &) { }

	if (!retrievedData) {
		if (m_update) {
			createInfoTable();
		}
		m_fileVersion = 0;
	}
	else {
		m_fileVersion = version.get(0);
	}
}

auto DatabaseUpdater::loadSqlFiles() const -> pair_t<size_t, ord_map_t<int32_t, string_t>> {
	fs::path fullPath = fs::system_complete(fs::path{"sql"});
	if (!fs::exists(fullPath)) {
		m_server->log(LogType::CriticalError, "SQL files not found: " + fullPath.generic_string());
		ExitCodes::exit(ExitCodes::SqlDirectoryNotFound);
		return make_pair(static_cast<size_t>(0), ord_map_t<int32_t, string_t>{});
	}

	fs::directory_iterator end;
	ord_map_t<int32_t, string_t> ret;
	size_t version = 0;
	for (fs::directory_iterator dir(fullPath); dir != end; ++dir) {
		string_t filename = dir->path().filename().generic_string();
		string_t fullFile = dir->path().generic_string();

		// Our Vana version files have a format of 0000_file.sql where 0000 is a number identifier
		// Scripts are executed in the order of lowest identifier (e.g. 0000) to highest (e.g. 9999)
		std::regex re{R"((\d\d\d\d)_.*\.sql)"};
		match_t matches;
		if (!std::regex_match(filename, matches, re)) {
			// Not a valid version SQL file
			continue;
		}

		size_t v = StringUtilities::lexical_cast<size_t, string_t>(matches[1]);

		if (version < v) {
			version = v;
		}

		if (m_update) {
			// Only record the SQL files if we're going to update the database
			ret[v] = fullFile;
		}
	}

	return make_pair(version, ret);
}

auto DatabaseUpdater::createInfoTable() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	sql.once << "CREATE TABLE IF NOT EXISTS " << db.makeTable("vana_info") << " (version INT UNSIGNED)";
	sql.once << "INSERT INTO " << db.makeTable("vana_info") << " VALUES (NULL)";
}

// Set version number in the info table
auto DatabaseUpdater::updateInfoTable(size_t version) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	sql.once << "UPDATE " << db.makeTable("vana_info") << " SET version = :version", soci::use(version, "version");
}

auto DatabaseUpdater::runQueries(const string_t &filename) -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	vector_t<string_t> queries = MySqlQueryParser::parseQueries(filename);

	// Run them
	for (const auto &query : queries) {
		try {
			sql.once << query;
		}
		catch (soci::soci_error &e) {
			m_server->log(LogType::CriticalError, [&](out_stream_t &str) {
				str <<
					"QUERY ERROR: " << e.what() << std::endl <<
					"File: " << filename << std::endl;
			});
			ExitCodes::exit(ExitCodes::QueryError);
			break;
		}
	}
}

}