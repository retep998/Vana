/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "Ip.h"
#include "Session.h"
#include "SessionManager.h"
#include "Types.h"
#include <string>
#include <boost/asio.hpp>

class AbstractConnection;

class ServerClient : public Session {
public:
	ServerClient(boost::asio::io_service &ioService, const Ip &serverIp, port_t serverPort, ref_ptr_t<SessionManager> sessionManager, AbstractConnection *connection, bool ping);
private:
	friend class ConnectionManager;
	auto startConnect() -> void;
	auto readConnectPacket() -> void;

	port_t m_port = 0;
	Ip m_server;
	boost::asio::ip::tcp::resolver m_resolver;
};