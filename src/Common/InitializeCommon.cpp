/*
Copyright (C) 2008 Vana Development Team

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
#include "InitializeCommon.h"
#include "MySQLM.h"
#include "Config.h"
#include <iostream>
#include <string>

using std::string;

void Initializing::initializeMySQL() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing MySQL... ";
	Config config("conf/mysql.lua");
	bool failed = false;
	// Character Database
	if (!(chardb.set_option(new mysqlpp::ReconnectOption(true)) && chardb.connect(config.getString("chardb_database").c_str(), config.getString("chardb_host").c_str(), config.getString("chardb_username").c_str(), config.getString("chardb_password").c_str(), config.getInt("chardb_port")))) {
		std::cout << "FAILED: " << chardb.error() << std::endl;
		exit(1);
	}
	// Data Database
	if (!(datadb.set_option(new mysqlpp::ReconnectOption(true)) && datadb.connect(config.getString("datadb_database").c_str(), config.getString("datadb_host").c_str(), config.getString("datadb_username").c_str(), config.getString("datadb_password").c_str(), config.getInt("datadb_port")))) {
		std::cout << "FAILED: " << datadb.error() << std::endl;
		exit(1);
	}
	std::cout << "DONE" << std::endl;
}

void Initializing::checkSchemaVersion(bool update) {
	// Vana_info table for checking database version (currently checking and updating unimplemented)
	mysqlpp::Query query = chardb.query("SELECT * FROM Vana_info LIMIT 1");
	mysqlpp::StoreQueryResult res = query.store();

	bool succeed = false;
	int version;
	if (res.size() == 0) {
		if (update) {
			// Table doesn't exist or there's no record in the table, so lets create it
			query << "CREATE TABLE IF NOT EXISTS Vana_info (version INT UNSIGNED)";
			query.exec();

			// Insert a default record of NULL
			query << "INSERT INTO Vana_info VALUES (NULL)";
			query.exec();
		}
		
		version = -1;
	}
	else {
		version = res[0][0].is_null() ? -1 : (int) res[0][0];
	}

	// TODO: Compare version (and run SQL files if update is true)
	succeed = true;

	if (!succeed && !update) {
		// Wrong version and we're not allowed to update, so lets quit
		std::cout << "Wrong version of database, please run Login Server to update.";
		exit(4);
	}
	else if (!succeed) {
		// Failed but we can update it
		// TODO: Update the schema
		std::cout << "Wrong version of database, updating...";
		succeed = true;
	}
}

void Initializing::setUsersOffline(int onlineid) {
	mysqlpp::Query query = chardb.query();
	query << "UPDATE users INNER JOIN characters ON users.id = characters.userid SET users.online = 0, characters.online = 0 WHERE users.online = " << mysqlpp::quote << onlineid;
	query.exec();
}
