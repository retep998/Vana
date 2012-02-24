/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "shared_array.hpp"
#include "Types.h"
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <memory>
#include <queue>
#include <string>

using boost::asio::ip::tcp;
using std::queue;
using std::string;

class AbstractConnection;
class PacketCreator;

class Session : public AbstractSession, public std::enable_shared_from_this<Session> {
public:
	friend class ConnectionAcceptor;
	Session(boost::asio::io_service &ioService, SessionManagerPtr sessionManager, AbstractConnection *connection, bool isForClient, bool isEncrypted, bool usePing, const string &patchLocation = "");

	void disconnect();
	void send(const PacketCreator &packet);
	ip_t getIp() const;
protected:
	tcp::socket & getSocket() { return m_socket; }
	void start();
	void stop();
	void handleStart();
	void handleStop();

	void startReadHeader();
	void handleWrite(const boost::system::error_code &error, size_t bytesTransferred);
	void handleReadHeader(const boost::system::error_code &error, size_t bytesTransferred);
	void handleReadBody(const boost::system::error_code &error, size_t bytesTransferred);
	void send(const unsigned char *buf, int32_t len);
	void sendIv(const PacketCreator &packet);

	static const size_t headerLen = 4;
	static const size_t maxBufferLen = 65535;

	tcp::socket m_socket;
	Decoder m_decoder;
	std::shared_ptr<AbstractConnection> m_connection;
	std::shared_array<unsigned char> m_buffer;
	bool m_isForClient;
	bool m_usePing;
	string m_patchLocation;

	// Packet sending
	std::shared_array<unsigned char> m_sendPacket;
	boost::mutex m_sendMutex;
};

typedef std::shared_ptr<Session> SessionPtr;