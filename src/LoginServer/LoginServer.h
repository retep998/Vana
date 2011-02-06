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
#ifndef LOGINSERVER_H
#define LOGINSERVER_H

#include "AbstractServer.h"
#include "ConfigFile.h"
#include "InitializeLogin.h"
#include "LoginServerAcceptConnection.h"
#include "PlayerLogin.h"
#include "Types.h"
#include <boost/utility.hpp>

// LoginServer main application class, implemented as singleton
class LoginServer : public AbstractServer, boost::noncopyable {
public:
	static LoginServer * Instance() {
		if (singleton == 0)
			singleton = new LoginServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void loadWorlds();
	void listen();

	bool getPinEnabled() const { return pinEnabled; }
	void setPinEnabled(bool enabled) { pinEnabled = enabled; }
	int32_t getInvalidLoginThreshold() const { return invalid_login_threshold; }
private:
	LoginServer() {};
	static LoginServer *singleton;

	bool pinEnabled;
	int16_t port;
	int16_t inter_port;
	int32_t invalid_login_threshold;
};

#endif
