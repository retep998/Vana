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

#include "Ip.h"
#include "Types.h"
#include <vector>
#include <boost/utility.hpp>

using std::vector;

class WorldServerAcceptConnection;

class Channel : boost::noncopyable {
public:
	Channel();
	void setId(uint16_t id) { m_id = id; }
	void setPort(uint16_t port) { m_port = port; }
	void setIp(uint32_t ip) { m_ip = ip; }
	void setExternalIps(const IpMatrix &ipMatrix) { m_externalIps = ipMatrix; }
	void setConnection(WorldServerAcceptConnection *connection) { m_connection = connection; }
	void setPlayers(int32_t players) { m_players = players; }

	int32_t increasePlayers() { return ++m_players; }
	int32_t decreasePlayers() { return --m_players; }
	uint16_t getId() const { return m_id; }
	uint16_t getPort() const { return m_port; }
	uint32_t getIp() const { return m_ip; }
	const IpMatrix & getExternalIps() { return m_externalIps; }
	WorldServerAcceptConnection * getConnection() const { return m_connection; }
private:
	uint16_t m_id;
	uint16_t m_port;
	int32_t m_players;
	uint32_t m_ip;
	IpMatrix m_externalIps;
	WorldServerAcceptConnection *m_connection;
};
