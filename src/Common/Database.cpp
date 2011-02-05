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

Database::tsConn Database::chardb;
Database::tsConn Database::datadb;

void Database::connectCharDB() {
	ConfigFile config("conf/mysql.lua");
	// Character Database
	chardb.reset(new mysqlpp::Connection);
	chardb->set_option(new mysqlpp::ReconnectOption(true));
	chardb->connect(config.getString("chardb_database").c_str(), 
					config.getString("chardb_host").c_str(), 
					config.getString("chardb_username").c_str(), 
					config.getString("chardb_password").c_str(), 
					config.getInt("chardb_port")
					);
}

void Database::connectDataDB() {
	ConfigFile config("conf/mysql.lua");
	// Data Database
	datadb.reset(new mysqlpp::Connection);
	datadb->set_option(new mysqlpp::ReconnectOption(true));
	datadb->connect(config.getString("datadb_database").c_str(), 
					config.getString("datadb_host").c_str(), 
					config.getString("datadb_username").c_str(), 
					config.getString("datadb_password").c_str(), 
					config.getInt("datadb_port")
					);
}
