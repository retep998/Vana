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

thread_local soci::session * Database::m_chardb = nullptr;
thread_local soci::session * Database::m_datadb = nullptr;
string_t Database::m_charTablePrefix;
string_t Database::m_dataTablePrefix;

auto Database::connectCharDb() -> void {
	ConfigFile config("conf/database.lua");
	config.run();
	DbConfig conf = config.get<DbConfig>("chardb");
	m_chardb = new soci::session(soci::mysql, buildConnectionString(conf));
	m_chardb->reconnect();
	m_charTablePrefix = conf.tablePrefix;
}

auto Database::connectDataDb() -> void {
	ConfigFile config("conf/database.lua");
	config.run();
	DbConfig conf = config.get<DbConfig>("datadb");
	m_datadb = new soci::session(soci::mysql, buildConnectionString(conf));
	m_datadb->reconnect();
	m_dataTablePrefix = conf.tablePrefix;
}

auto Database::buildConnectionString(const DbConfig &conf) -> string_t {
	out_stream_t str;

	str << "dbname=" << conf.database
		<< " user=" << conf.username
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