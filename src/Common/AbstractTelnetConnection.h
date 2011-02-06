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

#include "TelnetSession.h"
#include <string>
#include <boost/scoped_ptr.hpp>

using std::string;

class AbstractTelnetConnection {
public:
	virtual ~AbstractTelnetConnection() { }

	virtual void sendConnectedMessage() = 0;
	virtual void realHandleRequest(const string &data) = 0;
	void handleRequest(const string &data);

	TelnetSession * getSession() const { return m_session; }
	void setSession(TelnetSession *val);
	uint32_t getIp() const { return m_ip; }
	void setIp(uint32_t ip) { m_ip = ip; }
protected:
	TelnetSession *m_session;
	uint32_t m_ip;
};

class AbstractTelnetConnectionFactory {
public:
	virtual AbstractTelnetConnection * createConnection() = 0;
	virtual ~AbstractTelnetConnectionFactory() { }
};

