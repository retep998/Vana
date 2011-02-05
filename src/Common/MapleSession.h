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

#include "AbstractSession.h"
#include "Decoder.h"
#include "Types.h"
#include <queue>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/tr1/memory.hpp>

using std::queue;
using std::string;
using boost::asio::ip::tcp;
using boost::shared_array;

class AbstractConnection;
class PacketCreator;

class MapleSession :
public AbstractSession,
public boost::enable_shared_from_this<MapleSession>
{
public:
	MapleSession(boost::asio::io_service &io_service,
				SessionManagerPtr sessionManager,
				AbstractConnection *player,
				bool isServer,
				const string &patchLocation = "");

	tcp::socket & getSocket() { return m_socket; }

	void start();
	void handle_start();
	void stop();
	void disconnect();
	void handle_stop();
	void send(const unsigned char *buf, int32_t len, bool encrypt = true);
	void send(const PacketCreator &packet, bool encrypt = true);
	uint32_t getIp() const;
protected:
	void start_read_header();
	void handle_write(const boost::system::error_code &error, size_t bytes_transferred);
	void handle_read_header(const boost::system::error_code &error, size_t bytes_transferred);
	void handle_read_body(const boost::system::error_code &error, size_t bytes_transferred);

	static const size_t connectHeaderLen = 2;
	static const size_t headerLen = 4;
	static const size_t maxBufferLen = 65535;

	tcp::socket m_socket;
	Decoder m_decoder;
	std::tr1::shared_ptr<AbstractConnection> m_player;
	shared_array<unsigned char> m_buffer;
	bool m_isServer;
	string m_patchLocation;

	// Packet sending
	shared_array<unsigned char> m_sendPacket;
	boost::mutex m_sendMutex;
};

typedef boost::shared_ptr<MapleSession> MapleSessionPtr;


