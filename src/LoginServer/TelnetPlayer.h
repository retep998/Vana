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

#include "AbstractTelnetConnection.h"
#include "Types.h"
#include <string>

using std::string;

class TelnetPlayer : public AbstractTelnetConnection {
public:
	TelnetPlayer() : m_loggedOn(false), m_gotUsername(false), m_gotPass(false) { }
	~TelnetPlayer();
	void sendConnectedMessage();
	bool checkIpBanned();
	bool handleLoginRequest(const string &password);
	void realHandleRequest(const string &data);
private:
	bool m_loggedOn;
	bool m_gotUsername;
	bool m_gotPass;
	string m_username;
};

class TelnetPlayerFactory : public AbstractTelnetConnectionFactory {
public:
	AbstractTelnetConnection * createConnection() {
		return new TelnetPlayer();
	}
};
