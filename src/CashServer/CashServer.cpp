/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "CashServer.h"
#include "ConfigFile.h"
#include "ConnectionManager.h"
#include "InitializeCashServer.h"
#include "InitializeCommon.h"
#include "IpUtilities.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "ServerPacket.h"
#include "WorldServerConnection.h"

CashServer * CashServer::singleton = nullptr;

void CashServer::listen() {
	ConnectionManager::Instance()->accept(m_port, new PlayerFactory());
	Initializing::setUsersOffline(getOnlineId());
}

void CashServer::shutdown() {
	m_connected = false; // Else when WorldServerConnection disconnects, it will try to call shutdown() again
	AbstractServer::shutdown();
}

void CashServer::loadData() {
	Initializing::checkSchemaVersion();
	Initializing::checkMcdbVersion();
	Initializing::loadData();

	WorldServerConnection *loginPlayer = new WorldServerConnection;
	ConnectionManager::Instance()->connect(m_loginIp, m_loginPort, loginPlayer);
	loginPlayer->sendAuth(getInterPassword(), getExternalIp());
}

void CashServer::loadLogConfig() {
	ConfigFile conf("conf/logger.lua", false);
	initializeLoggingConstants(conf);
	conf.execute();

	bool enabled = conf.getBool("log_cash");
	if (enabled) {
		LogConfig log = conf.getLogConfig("cash");
		createLogger(log);
	}
}

string CashServer::makeLogIdentifier() {
	return "World: " + boost::lexical_cast<string>(static_cast<int16_t>(getWorld())) + "; Cash server";
}

void CashServer::connectWorld() {
	m_worldConnection = new WorldServerConnection;
	ConnectionManager::Instance()->connect(m_worldIp, m_worldPort, getWorldConnection());
	getWorldConnection()->sendAuth(getInterPassword(), getExternalIp());
}

void CashServer::loadConfig() {
	ConfigFile config("conf/cashserver.lua");
	m_loginIp = IpUtilities::stringToIp(config.getString("login_ip"));
	m_loginPort = config.getShort("login_inter_port");

	 // Will get from world server
	m_world = -1;
	m_port = -1;
}

void CashServer::sendToWorld(PacketCreator &packet) {
	getWorldConnection()->getSession()->send(packet);
}

void CashServer::setScrollingHeader(const string &message) {
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
