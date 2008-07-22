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
#include "AbstractServer.h"
#include "Config.h"
#include <ctime>
#include <iostream>
#include <iomanip>

AbstractServer::AbstractServer() {
	to_listen = false;
}

void AbstractServer::initialize() {
	clock_t startTime = clock(); // Measures time it takes to start

	Config config("conf/inter_password.lua");
	inter_password = config.getString("inter_password");

	loadConfig();
	loadData();
	if (to_listen)
		listen();

	float loadingTime = (clock() - startTime) / (float) 1000;
	std::cout << "Started in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl << std::endl;
}
