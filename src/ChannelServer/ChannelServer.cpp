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
#include "ChannelServer.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "ConnectionManager.h"
#include "InitializeChannel.h"
#include "InitializeCommon.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "ServerPacket.h"
#include "SyncPacket.h"
#include "WorldServerConnection.h"
#include "WorldServerConnectPacket.h"

ChannelServer * ChannelServer::singleton = nullptr;

ChannelServer::ChannelServer() :
	m_channelId(-1),
	m_worldIp(0),
	m_loginIp(0)
{
}

void ChannelServer::listen() {
	ConnectionManager::Instance()->accept(Ip::Type::Ipv4, m_port, new PlayerFactory(), m_loginConfig, false);
	Initializing::setUsersOffline(getOnlineId());
}

void ChannelServer::shutdown() {
	m_channelId = -1; // Otherwise when WorldServerConnection disconnects, it will try to call shutdown() again
	AbstractServer::shutdown();
}

void ChannelServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::checkMcdbVersion();
	Initializing::loadData();

	WorldServerConnection *loginPlayer = new WorldServerConnection;
	ConnectionManager::Instance()->connect(m_loginIp, m_loginPort, m_loginConfig, loginPlayer);
	const string &interPassword = getInterPassword();
	const string &salt = getSalt();
	const IpMatrix &externalIps = getExternalIps();
	loginPlayer->sendAuth(interPassword, salt, externalIps);
}

void ChannelServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.get<bool>("log_channels");
	if (enabled) {
		LogConfig log = conf.getClass<LogConfig>("channel");
		createLogger(log);
	}
}

opt_string ChannelServer::makeLogIdentifier() {
	std::ostringstream identifier;
	identifier << "World: " << static_cast<int16_t>(getWorldId()) << "; ID: " << getChannelId();
	return identifier.str();
}

void ChannelServer::connectWorld() {
	m_worldConnection = new WorldServerConnection;
	ConnectionManager::Instance()->connect(m_worldIp, m_worldPort, m_loginConfig, m_worldConnection);
	const string &interPassword = getInterPassword();
	const string &salt = getSalt();
	const IpMatrix &externalIps = getExternalIps();
	getWorldConnection()->sendAuth(interPassword, salt, externalIps);
}

void ChannelServer::loadConfig() {
	ConfigFile config("conf/channelserver.lua");
	m_loginIp = Ip(Ip::stringToIpv4(config.getString("login_ip")));
	m_loginPort = config.get<port_t>("login_inter_port");

	 // Will get from world server
	m_world = -1;
	m_port = -1;
	m_pianusChannel = false;
	m_papChannel = false;
	m_zakumChannel = false;
	m_horntailChannel = false;
	m_pinkbeanChannel = false;
}

void ChannelServer::sendPacketToWorld(PacketCreator &packet) {
	getWorldConnection()->getSession()->send(packet);
}

void ChannelServer::setScrollingHeader(const string &message) {
	if (getScrollingHeader() != message) {
		m_config.scrollingHeader = message;
		if (message.size() == 0) {
			ServerPacket::scrollingHeaderOff();
		}
		else {
			ServerPacket::changeScrollingHeader(message);
		}
	}
}

void ChannelServer::modifyRate(int32_t rateType, int32_t newValue) {
	Rates currentRates = m_config.rates;
	if (rateType & Rates::Types::MobExpRate) currentRates.mobExpRate = newValue;
	if (rateType & Rates::Types::MobMesoRate) currentRates.mobMesoRate = newValue;
	if (rateType & Rates::Types::QuestExpRate) currentRates.questExpRate = newValue;
	if (rateType & Rates::Types::DropRate) currentRates.dropRate = newValue;
	SyncPacket::ConfigPacket::modifyRates(currentRates);
}

void ChannelServer::setRates(const Rates &rates) {
	m_config.rates = rates;
}

void ChannelServer::setConfig(const WorldConfig &config) {
	setScrollingHeader(config.scrollingHeader);
	m_config = config;
	int8_t channelId = static_cast<int8_t>(m_channelId + 1);
	m_pianusChannel = MiscUtilities::isBossChannel(config.pianus.channels, channelId);
	m_papChannel = MiscUtilities::isBossChannel(config.pap.channels, channelId);
	m_zakumChannel = MiscUtilities::isBossChannel(config.zakum.channels, channelId);
	m_horntailChannel = MiscUtilities::isBossChannel(config.horntail.channels, channelId);
	m_pinkbeanChannel = MiscUtilities::isBossChannel(config.pinkbean.channels, channelId);
}