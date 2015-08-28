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
#include "DbConfig.hpp"
#include <soci-mysql.h>

namespace Vana {

thread_local owned_ptr_t<Database> Database::m_chardb{nullptr};
thread_local owned_ptr_t<Database> Database::m_datadb{nullptr};

auto Database::initCharDb() -> Database & {
	if (m_chardb != nullptr) throw std::logic_error{"Must not call initCharDb after the database is already initialized"};

	auto config = ConfigFile::getDatabaseConfig();
	config->run();
	DbConfig conf = config->get<DbConfig>("chardb");
	m_chardb = make_owned_ptr<Database>(conf, false);
	auto &sql = m_chardb->getSession();

	if (!schemaExists(sql, conf.database)) {
		sql.once << "CREATE DATABASE " << conf.database;
	}

	m_chardb.release();
	return getCharDb();
}

Database::Database(const DbConfig &conf, bool includeDatabase) {
	m_session = make_owned_ptr<soci::session>(soci::mysql, buildConnectionString(conf, includeDatabase));
	m_session->reconnect();
	m_schema = conf.database;
	m_tablePrefix = conf.tablePrefix;
}

auto Database::getSession() -> soci::session & {
	return *m_session;
}

auto Database::getSchema() const -> string_t {
	return m_schema;
}

auto Database::getTablePrefix() const -> string_t {
	return m_tablePrefix;
}

auto Database::makeTable(const string_t &table) const -> string_t {
	return m_tablePrefix + table;
}

auto Database::tableExists(const string_t &table) -> bool {
	return tableExists(getSession(), m_schema, makeTable(table));
}

auto Database::schemaExists(soci::session &sql, const string_t &schema) -> bool {
	opt_string_t database;
	sql.once
		<< "SELECT SCHEMA_NAME "
		<< "FROM INFORMATION_SCHEMA.SCHEMATA "
		<< "WHERE SCHEMA_NAME = :schema "
		<< "LIMIT 1",
		soci::use(schema, "schema"),
		soci::into(database);
	return database.is_initialized();
}

auto Database::tableExists(soci::session &sql, const string_t &schema, const string_t &table) -> bool {
	opt_string_t databaseTable;
	sql.once
		<< "SELECT TABLE_NAME "
		<< "FROM INFORMATION_SCHEMA.TABLES "
		<< "WHERE TABLE_SCHEMA = :schema AND TABLE_NAME = :table "
		<< "LIMIT 1",
		soci::use(schema, "schema"),
		soci::use(table, "table"),
		soci::into(databaseTable);
	return databaseTable.is_initialized();
}

auto Database::connectCharDb() -> void {
	auto config = ConfigFile::getDatabaseConfig();
	config->run();
	DbConfig conf = config->get<DbConfig>("chardb");
	m_chardb = make_owned_ptr<Database>(conf, true);
}

auto Database::connectDataDb() -> void {
	auto config = ConfigFile::getDatabaseConfig();
	config->run();
	DbConfig conf = config->get<DbConfig>("datadb");
	m_datadb = make_owned_ptr<Database>(conf, true);
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

}