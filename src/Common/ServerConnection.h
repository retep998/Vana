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

#include "AbstractConnection.h"
#include "Ip.h"
#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class AbstractServer;
class PacketReader;

class AbstractServerConnection : public AbstractConnection {
public:
	AbstractServerConnection() { m_isServer = true; }
	void sendAuth(const string &pass, IpMatrix &extIp);
	int8_t getType() const { return m_type; }
protected:
	void setType(int8_t type) { m_type = type; }
private:
	int8_t m_type;
};

class AbstractServerAcceptConnection : public AbstractConnection {
public:
	AbstractServerAcceptConnection() : m_isAuthenticated(false) { m_isServer = true; }
	bool processAuth(AbstractServer *server, PacketReader &packet, const string &pass);
	virtual void authenticated(int8_t type) = 0;

	bool isAuthenticated() const { return m_isAuthenticated; }
	const IpMatrix & getExternalIp() const { return m_externalIp; }
	int8_t getType() const { return m_type; }
protected:
	void setType(int8_t type) { m_type = type; }
private:
	int8_t m_type;
	bool m_isAuthenticated;
	IpMatrix m_externalIp;
};
