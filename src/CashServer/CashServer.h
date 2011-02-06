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
#pragma once

#include "AbstractServer.h"
#include "Configuration.h"
#include "Types.h"
#include <boost/utility.hpp>
#include <string>

using std::string;

class WorldServerConnection;
class PacketCreator;

// CashServer main application class, implemented as singleton
class CashServer : public AbstractServer, boost::noncopyable {
public:
	static CashServer * Instance() {
		if (singleton == nullptr)
			singleton = new CashServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void loadLogConfig();
	void listen();
	void shutdown();
	void connectWorld();
	void sendToWorld(PacketCreator &packet);
	string makeLogIdentifier();

	void setWorld(int8_t id) { m_world = id; }
	void setWorldPort(uint16_t port) { m_worldPort = port; }
	void setPort(uint16_t port) { m_port = port; }
	void setScrollingHeader(const string &message);
	void setWorldIp(uint32_t ip) { m_worldIp = ip; }
	void setConnected(bool wat) { m_connected = wat; }

	bool isConnected() const { return m_connected; }
	int8_t getWorld() const { return m_world; }
	int32_t getOnlineId() const { return 20000 + (int32_t) m_world * 100 + 50; }
	int32_t getMaxChars() const { return m_config.maxChars; }
	string getScrollingHeader() const { return m_config.scrollingHeader; }
	WorldServerConnection * getWorldConnection() const { return m_worldConnection; }

private:
	CashServer() : m_connected(false) {};
	static CashServer *singleton;

	WorldServerConnection *m_worldConnection;

	bool m_connected;
	int8_t m_world;
	uint16_t m_worldPort;
	uint16_t m_loginPort;
	uint16_t m_port;
	uint32_t m_worldIp;
	uint32_t m_loginIp;
	Configuration m_config;
};
