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

class AbstractTelnetConnection;
class PacketCreator;

class TelnetSession :
public AbstractSession,
public boost::enable_shared_from_this<TelnetSession>
{
public:
	TelnetSession(boost::asio::io_service &io_service,
				SessionManagerPtr sessionManager,
				AbstractTelnetConnection *player);

	tcp::socket & getSocket() { return m_socket; }

	void start();
	void handle_start();
	void stop();
	void disconnect();
	void handle_stop();
	void send(const unsigned char *buf, int32_t len);
	void send(const string &data, bool newline = true, bool starttext = true);
	void send();
	uint32_t getIp() const;
	void SetRemoveLastCharacter(bool on);
protected:
	void start_read();
	void handle_write(const boost::system::error_code &error, size_t bytes_transferred);
	void handle_read(const boost::system::error_code &error, size_t bytes_transferred);

	static const size_t maxBufferLen = 65535;
	static const int8_t Backspace = 0x08;
	static const int8_t LineFeed = 0x0A;
	static const int8_t CarriageReturn = 0x0D;

	bool m_removeLastChar;

	tcp::socket m_socket;
	Decoder m_decoder;
	std::tr1::shared_ptr<AbstractTelnetConnection> m_player;
	std::stringstream m_inputStream;
	size_t m_inputStreamSize;
	shared_array<unsigned char> m_buffer;

	// Packet sending
	shared_array<unsigned char> m_sendPacket;
	boost::mutex m_sendMutex;
};

typedef boost::shared_ptr<TelnetSession> TelnetSessionPtr;


