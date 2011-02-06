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
#include "WorldServer.h"
#include "ConnectionManager.h"
#include "InitializeCommon.h"
#include "InitializeWorld.h"
#include "IpUtilities.h"
#include "WorldServerAcceptPacket.h"

WorldServer * WorldServer::singleton = 0;

void WorldServer::listen() {
	ConnectionManager::Instance()->accept(inter_port, new WorldServerAcceptConnectionFactory());
}

void WorldServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::loadData();

	loginPlayer = new LoginServerConnection;
	ConnectionManager::Instance()->connect(login_ip, login_inter_port, loginPlayer);
	loginPlayer->sendAuth(inter_password, external_ip);
}

void WorldServer::loadConfig() {
	ConfigFile config("conf/worldserver.lua");
	login_ip = IpUtilities::stringToIp(config.getString("login_ip"));
	login_inter_port = config.getShort("login_inter_port");

	inter_port = -1; // Will get from login server later
	scrollingHeader = ""; // Will get from login server later
}

void WorldServer::setScrollingHeader(const string &message) {
	scrollingHeader = message;
	WorldServerAcceptPacket::scrollingHeader(message);
}
