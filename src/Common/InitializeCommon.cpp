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

void Initializing::initializeMySQL(){
	Config config("conf/mysql.lua");
	printf("Initializing MySQL... ");
	if(db.set_option(new mysqlpp::ReconnectOption(true)) && db.connect(config.getString("database"), config.getString("host"), config.getString("username"), config.getString("password"), config.getInt("port")))
		printf("DONE\n");
	else{
		printf("FAILED: %s\n", db.error());
		exit(1);
	}
}

