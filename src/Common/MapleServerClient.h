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

#include "MapleSession.h"
#include "SessionManager.h"
#include "Types.h"
#include <string>
#include <boost/asio.hpp>

using std::string;
using boost::asio::ip::tcp;

class AbstractConnection;

class MapleServerClient : public MapleSession {
public:
	MapleServerClient(boost::asio::io_service &io_service,
		uint32_t server, uint16_t port,
		SessionManagerPtr sessionManager,
		AbstractConnection *player);
	void start_connect();
private:
	void readConnectPacket();

	uint32_t m_server;
	uint16_t m_port;
	tcp::resolver m_resolver;
};

typedef boost::shared_ptr<MapleServerClient> MapleServerClientPtr;
