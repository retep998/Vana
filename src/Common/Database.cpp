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
#include "Database.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "ExitCodes.h"
#include "soci-mysql.h"
#include <exception>

Database::tsConn Database::m_chardb;
Database::tsConn Database::m_datadb;

void Database::connectCharDb() {
	ConfigFile config("conf/mysql.lua");
	DbConfig conf = config.getClass<DbConfig>("chardb");
	try {
		m_chardb.reset(new soci::session(soci::mysql, buildConnectionString(conf)));
	}
	catch (std::exception &ex) {
		std::cerr << "Could not connect to the Vana database: " << std::endl << ex.what() << std::endl;
		std::cerr << "Press enter to quit." << std::endl;
		getchar();
		exit(ExitCodes::InfoDatabaseError);
	}
}

void Database::connectDataDb() {
	ConfigFile config("conf/mysql.lua");
	DbConfig conf = config.getClass<DbConfig>("datadb");
	try {
		m_datadb.reset(new soci::session(soci::mysql, buildConnectionString(conf)));
	}
	catch (std::exception &ex) {
		std::cerr << "Could not connect to the MCDB database: " << std::endl << ex.what() << std::endl;
		std::cerr << "Press enter to quit." << std::endl;
		getchar();
		exit(ExitCodes::InfoDatabaseError);
	}
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