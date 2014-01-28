/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "WorldServer.hpp"
#include "ConnectionManager.hpp"
#include "InitializeCommon.hpp"
#include "InitializeWorld.hpp"
#include "PacketCreator.hpp"
#include "StringUtilities.hpp"
#include "SyncPacket.hpp"
#include "VanaConstants.hpp"
#include "WorldServerAcceptPacket.hpp"

WorldServer::WorldServer() :
	m_loginIp(0)
{
	setServerType(ServerTypes::World);
}

auto WorldServer::listen() -> void {
	ConnectionManager::getInstance().accept(Ip::Type::Ipv4, m_port, [] { return new WorldServerAcceptConnection(); }, m_loginConfig, true);
}

auto WorldServer::loadData() -> void {
	Initializing::checkSchemaVersion();
	Initializing::loadData();

	m_loginConnection = new LoginServerConnection;
	ConnectionManager::getInstance().connect(m_loginIp, m_loginPort, m_loginConfig, m_loginConnection);
	const string_t &interPassword = getInterPassword();
	const string_t &salt = getSalt();
	const IpMatrix &externalIps = getExternalIps();
	m_loginConnection->sendAuth(interPassword, salt, externalIps);
}

auto WorldServer::loadConfig() -> void {
	ConfigFile config("conf/interserver.lua");
	InterServerConfig conf = config.getClass<InterServerConfig>();

	m_loginIp = conf.loginIp;
	m_loginPort = conf.port;

	m_port = -1; // Will get from LoginServer later
}

auto WorldServer::rehashConfig(const WorldConfig &config) -> void {
	setConfig(config);
	WorldServerAcceptPacket::rehashConfig(config);
}

auto WorldServer::setConfig(const WorldConfig &config) -> void {
	m_config = config;
	m_defaultRates = config.rates;
}

auto WorldServer::setRates(const Rates &rates) -> void {
	m_config.rates = rates;
	SyncPacket::ConfigPacket::setRates(rates);
}

auto WorldServer::resetRates() -> void {
	setRates(m_defaultRates);
}

auto WorldServer::loadLogConfig() -> void {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.get<bool>("log_worlds");
	if (enabled) {
		LogConfig log = conf.getClass<LogConfig>("world");
		createLogger(log);
	}
}

auto WorldServer::makeLogIdentifier() -> opt_string_t {
	return "World " + StringUtilities::lexical_cast<string_t>(getWorldId());
}

auto WorldServer::setScrollingHeader(const string_t &message) -> void {
	m_config.scrollingHeader = message;
	SyncPacket::ConfigPacket::scrollingHeader(message);
}

auto WorldServer::sendPacketToLogin(const PacketCreator &packet) -> void {
	m_loginConnection->getSession()->send(packet);
}