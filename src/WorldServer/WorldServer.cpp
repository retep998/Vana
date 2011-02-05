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
#include "Types.h"
#include "WorldServerAcceptPacket.h"
#include <boost/lexical_cast.hpp>

WorldServer * WorldServer::singleton = nullptr;

WorldServer::WorldServer() :
m_worldId(-1),
m_cashServerConnection(nullptr),
m_cashConnected(false) 
{
	setServerType(ServerTypes::World);
}

void WorldServer::listen() {
	ConnectionManager::Instance()->accept(m_port, new WorldServerAcceptConnectionFactory());
}

void WorldServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::loadData();

	m_loginConnection = new LoginServerConnection;
	ConnectionManager::Instance()->connect(m_loginIp, m_loginPort, getLoginConnection());
	string interPassword = getInterPassword();
	IpMatrix externalIp = getExternalIp();
	getLoginConnection()->sendAuth(interPassword, externalIp);
}

void WorldServer::loadConfig() {
	ConfigFile config("conf/worldserver.lua");
	m_loginIp = IpUtilities::stringToIp(config.getString("login_ip"));
	m_loginPort = config.getUnsignedShort("login_inter_port");

	m_port = -1; // Will get from login server later
	m_cashPort = -1; // Will get from login server later
}

void WorldServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.getBool("log_worlds");
	if (enabled) {
		LogConfig log = conf.getLogConfig("world");
		createLogger(log);
	}
}

string WorldServer::makeLogIdentifier() {
	return "World " + boost::lexical_cast<string>(static_cast<int16_t>(getWorldId()));
}

void WorldServer::setScrollingHeader(const string &message) {
	m_config.scrollingHeader = message;
	WorldServerAcceptPacket::scrollingHeader(message);
}
