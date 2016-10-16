/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "database_updater.hpp"
#include "common/abstract_server.hpp"
#include "common/exit_code.hpp"
#include "common/io/database.hpp"
#include "common/io/mysql_query_parser.hpp"
#include "common/util/string.hpp"
#include "common/util/tokenizer.hpp"
#ifdef WIN32
#include <filesystem>
#else
// Until GCC supports the TR2 filesystem header
#include <boost/filesystem.hpp>
#endif
#include <iostream>

namespace vana {
namespace io {

#ifdef WIN32
namespace fs = std::tr2::sys;
#else
namespace fs = boost::filesystem;
#endif

database_updater::database_updater(abstract_server *server, bool update) :
	m_server{server},
	m_update{update}
{
	load_database_info();
	auto files = load_sql_files();
	m_sql_version = files.first;
}

auto database_updater::check_version() -> version_check_result {
	if (!m_db_available) {
		return version_check_result::database_unavailable;
	}
	return m_sql_version <= m_file_version ?
		version_check_result::fully_updated :
		version_check_result::needs_update;
}

auto database_updater::update() -> void {
	update(m_sql_version);
}

auto database_updater::update(size_t version) -> void {
	if (version <= m_file_version) {
		throw std::out_of_range{"SQL version to update to is less than the highest query file"};
	}

	auto files = load_sql_files().second;
	for (auto iter = files.find(m_file_version + 1); iter != std::end(files); ++iter) {
		run_queries(iter->second);
	}

	update_info_table(version);
}

auto database_updater::load_database_info() -> void {
	// vana_info table for checking database version
	opt_int32_t version;
	bool retrieved_data = false;

	try {
		auto &db = m_update ?
			database::init_char_db() :
			database::get_char_db();
		m_db_available = true;
	}
	catch (soci::soci_error &) {
		return;
	}

	try {
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		if (database::table_exists(sql, db.get_schema(), db.make_table(vana::table::vana_info))) {
			sql.once
				<< "SELECT version "
				<< "FROM " << db.make_table(vana::table::vana_info),
				soci::into(version);

			retrieved_data = sql.got_data();
		}
	}
	catch (soci::soci_error &) { }

	if (!retrieved_data) {
		if (m_update) {
			create_info_table();
		}
		m_file_version = 0;
	}
	else {
		m_file_version = version.get(0);
	}
}

auto database_updater::load_sql_files() const -> pair<size_t, ord_map<int32_t, string>> {
	fs::path full_path = fs::system_complete(fs::path{"sql"});
	if (!fs::exists(full_path)) {
		m_server->log(vana::log::type::critical_error, "SQL files not found: " + full_path.generic_string());
		exit(exit_code::sql_directory_not_found);
		return make_pair(static_cast<size_t>(0), ord_map<int32_t, string>{});
	}

	fs::directory_iterator end;
	ord_map<int32_t, string> ret;
	size_t version = 0;
	for (fs::directory_iterator dir(full_path); dir != end; ++dir) {
		string filename = dir->path().filename().generic_string();
		string full_file = dir->path().generic_string();

		// Our Vana version files have a format of 0000_file.sql where 0000 is a number identifier
		// Scripts are executed in the order of lowest identifier (e.g. 0000) to highest (e.g. 9999)
		std::regex re{R"((\d\d\d\d)_.*\.sql)"};
		match matches;
		if (!std::regex_match(filename, matches, re)) {
			// Not a valid version SQL file
			continue;
		}

		size_t v = vana::util::str::lexical_cast<size_t, string>(matches[1]);

		if (version < v) {
			version = v;
		}

		if (m_update) {
			// Only record the SQL files if we're going to update the database
			ret[v] = full_file;
		}
	}

	return make_pair(version, ret);
}

auto database_updater::create_info_table() -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	sql.once << "CREATE TABLE IF NOT EXISTS " << db.make_table(vana::table::vana_info) << " (version INT UNSIGNED)";
	sql.once << "INSERT INTO " << db.make_table(vana::table::vana_info) << " VALUES (NULL)";
}

// Set version number in the info table
auto database_updater::update_info_table(size_t version) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	sql.once << "UPDATE " << db.make_table(vana::table::vana_info) << " SET version = :version", soci::use(version, "version");
}

auto database_updater::run_queries(const string &filename) -> void {
	auto &db = vana::io::database::get_char_db();
	auto &sql = db.get_session();
	vector<string> queries = mysql_query_parser::parse_queries(filename);

	// Run them
	for (const auto &query : queries) {
		try {
			sql.once << query;
		}
		catch (soci::soci_error &e) {
			m_server->log(vana::log::type::critical_error, [&](out_stream &str) {
				str <<
					"QUERY ERROR: " << e.what() << std::endl <<
					"File: " << filename << std::endl;
			});
			exit(exit_code::query_error);
			break;
		}
	}
}

}
}