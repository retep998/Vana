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
#ifndef MAPLESESSION_H
#define MAPLESESSION_H

#include "AbstractSession.h"
#include "Decoder.h"
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

using std::string;
using boost::asio::ip::tcp;

class AbstractPlayer;
class PacketCreator;

class MapleSession :
public AbstractSession,
public boost::enable_shared_from_this<MapleSession>
{
public:
	MapleSession(boost::asio::io_service &io_service,
				SessionManagerPtr sessionManager,
				AbstractPlayer *player, bool isServer,
				string connectPacketUnknown = "");

	tcp::socket & getSocket() { return m_socket; }

	void start();
	void handle_start();
	void stop();
	void disconnect();
	void handle_stop();
	void send(unsigned char *buf, int len, bool encrypt = true);
	void send(const PacketCreator &packet, bool encrypt = true);
protected:
	void start_read_header();
	void handle_write(const boost::system::error_code &error,
		size_t bytes_transferred);
	void handle_read_header(const boost::system::error_code &error,
		size_t bytes_transferred);
	void handle_read_body(const boost::system::error_code &error,
		size_t bytes_transferred);

	static const size_t connectHeaderLen = 2;
	static const size_t headerLen = 4;
	static const size_t bufferLen = 10000;

	tcp::socket m_socket;
	boost::scoped_ptr<Decoder> m_decoder;
	std::tr1::shared_ptr<AbstractPlayer> m_player;
	unsigned char m_buffer[bufferLen];
	bool m_is_server;
	string m_connect_packet_unknown;
};

typedef boost::shared_ptr<MapleSession> MapleSessionPtr;

#endif