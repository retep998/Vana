/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Database.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "soci-mysql.h"

thread_local soci::session * Database::m_chardb = nullptr;
thread_local soci::session * Database::m_datadb = nullptr;

void Database::connectCharDb() {
	ConfigFile config("conf/database.lua");
	DbConfig conf = config.getClass<DbConfig>("chardb");
	m_chardb = new soci::session(soci::mysql, buildConnectionString(conf));
	m_chardb->reconnect();
}

void Database::connectDataDb() {
	ConfigFile config("conf/database.lua");
	DbConfig conf = config.getClass<DbConfig>("datadb");
	m_datadb = new soci::session(soci::mysql, buildConnectionString(conf));
	m_datadb->reconnect();
}

string Database::buildConnectionString(const DbConfig &conf) {
	std::ostringstream str;

	str << "dbname=" << conf.database
		<< " user=" << conf.username
		<< " password='" << conf.password << "'"
		<< " host=" << conf.host
		<< " port=" << conf.port;

	return str.str();
}