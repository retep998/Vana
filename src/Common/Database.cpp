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
#include "Database.h"
#include "ConfigFile.h"
#include "Configuration.h"

Database::tsConn Database::m_chardb;
Database::tsConn Database::m_datadb;

void Database::connectCharDb() {
	ConfigFile config("conf/mysql.lua");
	DbConfig conf = config.getDbConfig("chardb");
	m_chardb.reset(new mysqlpp::Connection);
	m_chardb->set_option(new mysqlpp::ReconnectOption(true));
	m_chardb->connect(conf.database.c_str(), conf.host.c_str(), conf.username.c_str(), conf.password.c_str(), conf.port);
}

void Database::connectDataDb() {
	ConfigFile config("conf/mysql.lua");
	DbConfig conf = config.getDbConfig("datadb");
	m_datadb.reset(new mysqlpp::Connection);
	m_datadb->set_option(new mysqlpp::ReconnectOption(true));
	m_datadb->connect(conf.database.c_str(), conf.host.c_str(), conf.username.c_str(), conf.password.c_str(), conf.port);
}