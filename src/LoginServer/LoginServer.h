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
#include "ConfigFile.h"
#include "InitializeLogin.h"
#include "LoginServerAcceptConnection.h"
#include "Player.h"
#include "Types.h"
#include <boost/utility.hpp>

// LoginServer main application class, implemented as singleton
class LoginServer : public AbstractServer, boost::noncopyable {
public:
	static LoginServer * Instance() {
		if (singleton == nullptr)
			singleton = new LoginServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void loadLogConfig();
	void loadWorlds();
	void listen();
	string makeLogIdentifier();

	bool getPinEnabled() const { return m_pinEnabled; }
	void setPinEnabled(bool enabled) { m_pinEnabled = enabled; }
	int32_t getInvalidLoginThreshold() const { return m_maxInvalidLogins; }
	uint16_t getCashServerPort() const { return m_cashPort; }
	string getTelnetPassword() const { return m_telnetPassword; }
private:
	LoginServer();
	static LoginServer *singleton;

	bool m_pinEnabled;
	bool m_telnetEnabled;
	uint16_t m_port;
	uint16_t m_interPort;
	uint16_t m_cashPort;
	uint16_t m_telnetPort;
	int32_t m_maxInvalidLogins;
	string m_telnetPassword;
};
