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

#include "AbstractSession.h"
#include "Decoder.h"
#include "Ip.h"
#include "shared_array.hpp"
#include "Types.h"
#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

class AbstractConnection;
class PacketCreator;

class Session : public AbstractSession, public enable_shared<Session> {
public:
	Session(boost::asio::io_service &ioService, ref_ptr_t<SessionManager> sessionManager, AbstractConnection *connection, bool isForClient, bool isEncrypted, bool usePing, const string_t &patchLocation = "");

	auto disconnect() -> void override;
	auto send(const PacketCreator &packet, bool encrypt = true) -> void;
	auto getIp() const -> const Ip &;
protected:
	auto getSocket() -> boost::asio::ip::tcp::socket & { return m_socket; }
	auto start() -> void override;
	auto stop() -> void override;
	auto handleStart() -> void override;
	auto handleStop() -> void override;

	auto startReadHeader() -> void;
	auto handleWrite(const boost::system::error_code &error, size_t bytesTransferred) -> void;
	auto handleReadHeader(const boost::system::error_code &error, size_t bytesTransferred) -> void;
	auto handleReadBody(const boost::system::error_code &error, size_t bytesTransferred) -> void;
	auto send(const unsigned char *buf, int32_t len, bool encrypt = true) -> void;
	auto getConnectPacket(const string_t &patchLocation) -> PacketCreator;

	static const size_t headerLen = 4;
	static const size_t maxBufferLen = 65535;

	bool m_isForClient = true;
	bool m_usePing = false;
	string_t m_patchLocation;
	ref_ptr_t<AbstractConnection> m_connection;
	Decoder m_decoder;
	boost::asio::ip::tcp::socket m_socket;
	MiscUtilities::shared_array<unsigned char> m_buffer;

	// Packet sending
	MiscUtilities::shared_array<unsigned char> m_sendPacket;
	mutex_t m_sendMutex;
private:
	friend class ConnectionAcceptor;
};