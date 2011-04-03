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

#include "Session.h"
#include "SessionManager.h"
#include "Types.h"
#include <string>
#include <boost/asio.hpp>

using std::string;
using boost::asio::ip::tcp;

class AbstractConnection;

class ServerClient : public Session {
public:
	friend class ConnectionManager;
	ServerClient(boost::asio::io_service &ioService, ip_t serverIp, port_t serverPort, SessionManagerPtr sessionManager, AbstractConnection *connection);
private:
	void startConnect();
	void readConnectPacket();

	ip_t m_server;
	port_t m_port;
	tcp::resolver m_resolver;
};

typedef boost::shared_ptr<ServerClient> ServerClientPtr;