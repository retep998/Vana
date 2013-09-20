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
#include "WorldServer.h"
#include "ConnectionManager.h"
#include "InitializeCommon.h"
#include "InitializeWorld.h"
#include "PacketCreator.h"
#include "StringUtilities.h"
#include "SyncPacket.h"
#include "VanaConstants.h"
#include "WorldServerAcceptPacket.h"

WorldServer * WorldServer::singleton = nullptr;

WorldServer::WorldServer() :
	m_worldId(-1),
	m_loginIp(0)
{
	setServerType(ServerTypes::World);
}

void WorldServer::listen() {
	ConnectionManager::Instance()->accept(Ip::Type::Ipv4, m_port, new WorldServerAcceptConnectionFactory(), m_loginConfig, true);
}

void WorldServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::loadData();

	m_loginConnection = new LoginServerConnection;
	ConnectionManager::Instance()->connect(m_loginIp, m_loginPort, m_loginConfig, m_loginConnection);
	const string &interPassword = getInterPassword();
	const string &salt = getSalt();
	const IpMatrix &externalIps = getExternalIps();
	m_loginConnection->sendAuth(interPassword, salt, externalIps);
}

void WorldServer::loadConfig() {
	ConfigFile config("conf/interserver.lua");
	InterServerConfig conf = config.getClass<InterServerConfig>();

	m_loginIp = conf.loginIp; //(Ip::stringToIpv4(config.getString("login_ip")));
	m_loginPort = conf.port; //config.get<port_t>("login_inter_port");

	m_port = -1; // Will get from LoginServer later
}

void WorldServer::rehashConfig(const WorldConfig &config) {
	setConfig(config);
	WorldServerAcceptPacket::rehashConfig(config);
}

void WorldServer::setConfig(const WorldConfig &config) {
	m_config = config;
	m_defaultRates = config.rates;
}

void WorldServer::setRates(const Rates &rates) {
	m_config.rates = rates;
	SyncPacket::ConfigPacket::setRates(rates);
}

void WorldServer::resetRates() {
	setRates(m_defaultRates);
}

void WorldServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.get<bool>("log_worlds");
	if (enabled) {
		LogConfig log = conf.getClass<LogConfig>("world");
		createLogger(log);
	}
}

opt_string WorldServer::makeLogIdentifier() {
	return "World " + StringUtilities::lexical_cast<string>(getWorldId());
}

void WorldServer::setScrollingHeader(const string &message) {
	m_config.scrollingHeader = message;
	SyncPacket::ConfigPacket::scrollingHeader(message);
}

void WorldServer::sendPacketToLogin(const PacketCreator &packet) {
	m_loginConnection->getSession()->send(packet);
}