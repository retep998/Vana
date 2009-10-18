/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "ConfigFile.h"
#include "ConnectionManager.h"
#include "MiscUtilities.h"
#include "TimeUtilities.h"
#include <ctime>
#include <iostream>
#include <iomanip>

AbstractServer::AbstractServer() {
	to_listen = false;
}

void AbstractServer::initialize() {
	startTime = TimeUtilities::getTickCount();

	ConfigFile config("conf/inter_password.lua");
	inter_password = config.getString("inter_password");

	ConfigFile configExtIp("conf/external_ip.lua");
	external_ip = configExtIp.getIpMatrix("external_ip");

	if (inter_password == "changeme") {
		std::cerr << "ERROR: inter_password is not changed." << std::endl;
		std::cout << "Press enter to quit ...";
		getchar();
		exit(1);
	}

	loadConfig();
	loadData();
	if (to_listen) {
		listen();
	}
}

void AbstractServer::shutdown() {
	ConnectionManager::Instance()->stop();
}

void AbstractServer::displayLaunchTime() const {
		float loadingTime = (TimeUtilities::getTickCount() - getStartTime()) / (float) 1000;		
		std::cout << "Started in " << std::setprecision(3) << loadingTime << " seconds!" << std::endl << std::endl;
}