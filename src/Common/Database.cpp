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
#include "Database.hpp"
#include "ConfigFile.hpp"
#include "Configuration.hpp"
#include <soci-mysql.h>

thread_local owned_ptr_t<soci::session> Database::m_chardb{nullptr};
thread_local owned_ptr_t<soci::session> Database::m_datadb{nullptr};
string_t Database::m_charDatabase;
string_t Database::m_charTablePrefix;
string_t Database::m_dataTablePrefix;

auto Database::initCharDb() -> soci::session & {
	if (m_chardb != nullptr) throw std::logic_error{"Must not call initCharDb after the database is already initialized"};

	ConfigFile config{"conf/database.lua"};
	config.run();
	DbConfig conf = config.get<DbConfig>("chardb");
	m_chardb = make_owned_ptr<soci::session>(soci::mysql, buildConnectionString(conf, false));
	m_chardb->reconnect();
	opt_string_t database;
	m_chardb->once
		<< "SELECT SCHEMA_NAME "
		<< "FROM INFORMATION_SCHEMA.SCHEMATA "
		<< "WHERE SCHEMA_NAME = :schema "
		<< "LIMIT 1",
		soci::use(conf.database, "schema"),
		soci::into(database);

	if (!database.is_initialized()) {
		m_chardb->once <<
			"CREATE DATABASE " << conf.database;
	}

	m_chardb.release();
	return getCharDb();
}

auto Database::getCharSchema() -> string_t {
	return m_charDatabase;
}

auto Database::connectCharDb() -> void {
	ConfigFile config{"conf/database.lua"};
	config.run();
	DbConfig conf = config.get<DbConfig>("chardb");
	m_chardb = make_owned_ptr<soci::session>(soci::mysql, buildConnectionString(conf, true));
	m_chardb->reconnect();
	m_charTablePrefix = conf.tablePrefix;
	m_charDatabase = conf.database;
}

auto Database::connectDataDb() -> void {
	ConfigFile config{"conf/database.lua"};
	config.run();
	DbConfig conf = config.get<DbConfig>("datadb");
	m_datadb = make_owned_ptr<soci::session>(soci::mysql, buildConnectionString(conf, true));
	m_datadb->reconnect();
	m_dataTablePrefix = conf.tablePrefix;
}

auto Database::buildConnectionString(const DbConfig &conf, bool includeDatabase) -> string_t {
	out_stream_t str;

	if (includeDatabase) {
		str << "dbname=" << conf.database << " ";
	}

	str
		<< "user=" << conf.username
		<< " password='" << conf.password << "'"
		<< " host=" << conf.host
		<< " port=" << conf.port;

	return str.str();
}

auto Database::makeCharTable(const string_t &table) -> string_t {
	return m_charTablePrefix + table;
}

auto Database::makeDataTable(const string_t &table) -> string_t {
	return m_dataTablePrefix + table;
}