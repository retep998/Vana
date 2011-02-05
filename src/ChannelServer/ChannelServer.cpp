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
#include "ChannelServer.h"
#include "ConfigFile.h"
#include "Configuration.h"
#include "ConnectionManager.h"
#include "InitializeChannel.h"
#include "InitializeCommon.h"
#include "IpUtilities.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "ServerPacket.h"
#include "WorldServerConnection.h"
#include <boost/lexical_cast.hpp>

ChannelServer * ChannelServer::singleton = nullptr;

ChannelServer::ChannelServer() :
m_channel(-1)
{
	setServerType(ServerTypes::Channel);
}

void ChannelServer::listen() {
	ConnectionManager::Instance()->accept(m_port, new PlayerFactory());
	Initializing::setUsersOffline(getOnlineId());
}

void ChannelServer::shutdown() {
	m_channel = -1; // Else when WorldServerConnection disconnects, it will try to call shutdown() again
	AbstractServer::shutdown();
}

void ChannelServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::checkMcdbVersion();
	Initializing::loadData();

	WorldServerConnection *loginPlayer = new WorldServerConnection;
	ConnectionManager::Instance()->connect(m_loginIp, m_loginPort, loginPlayer);
	string interPassword = getInterPassword();
	IpMatrix externalIp = getExternalIp();
	loginPlayer->sendAuth(interPassword, externalIp);
}

void ChannelServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.getBool("log_channels");
	if (enabled) {
		LogConfig log = conf.getLogConfig("channel");
		createLogger(log);
	}
}

string ChannelServer::makeLogIdentifier() {
	return "World: " + boost::lexical_cast<string>(static_cast<int16_t>(getWorld())) + "; ID: " + boost::lexical_cast<string>(getChannel());
}

void ChannelServer::connectWorld() {
	m_worldConnection = new WorldServerConnection;
	ConnectionManager::Instance()->connect(m_worldIp, m_worldPort, getWorldConnection());
	string interPassword = getInterPassword();
	IpMatrix externalIp = getExternalIp();
	getWorldConnection()->sendAuth(interPassword, externalIp);
}

void ChannelServer::loadConfig() {
	ConfigFile config("conf/channelserver.lua");
	m_loginIp = IpUtilities::stringToIp(config.getString("login_ip"));
	m_loginPort = config.getUnsignedShort("login_inter_port");

	 // Will get from world server
	m_world = -1;
	m_port = -1;
	m_pianusChannel = false;
	m_papChannel = false;
	m_zakumChannel = false;
	m_horntailChannel = false;
	m_pinkbeanChannel = false;
}

void ChannelServer::sendToWorld(PacketCreator &packet) {
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
