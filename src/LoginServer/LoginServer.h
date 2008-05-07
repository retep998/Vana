/*
Copyright (C) 2008 Vana Development Team

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
#include "InitializeLogin.h"
#include "Config.h"
#include "PlayerLogin.h"

// LoginServer main application class, implemented as singleton
class LoginServer : public AbstractServer {
public:
	static LoginServer * Instance() {
		if (singleton == 0)
			singleton = new LoginServer;
		return singleton;
	}
	void loadData();
	void loadConfig();
	void listen();

	bool getPinEnabled() const { return _pinEnabled; }
	void setPinEnabled(bool enabled) { _pinEnabled = enabled; }
private:
	LoginServer() {};
	LoginServer(const LoginServer&);
	LoginServer& operator=(const LoginServer&);
	static LoginServer *singleton;

	bool _pinEnabled;
};

#endif