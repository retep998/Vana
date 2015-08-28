/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Ip.hpp"
#include "Session.hpp"
#include "Types.hpp"
#include <string>
#include <asio.hpp>

namespace Vana {
	class AbstractConnection;
	class ConnectionManager;

	class ServerClient : public Session {
	public:
		ServerClient(asio::io_service &ioService, const Ip &serverIp, port_t serverPort, ConnectionManager &manager, AbstractConnection *connection, bool ping);
	private:
		friend class ConnectionManager;
		auto startConnect() -> Result;
		auto readConnectPacket() -> void;

		port_t m_port = 0;
		Ip m_server;
		asio::ip::tcp::resolver m_resolver;
	};
}