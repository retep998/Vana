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
#include "MapleSession.h"
#include "AbstractConnection.h"
#include "Decoder.h"
#include "IpUtilities.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "SessionManager.h"
#include <boost/bind.hpp>
#include <iostream>

MapleSession::MapleSession(boost::asio::io_service &ioService, SessionManagerPtr sessionManager, AbstractConnection *connection, bool isServer, bool isEncrypted, const string &patchLocation) :
	AbstractSession(sessionManager, (!isServer || isEncrypted)),
		// Apparently, "isServer" is true from sessions created by the server for the client
		// In addition, it's false from sessions created for the server clients
	m_socket(ioService),
	m_connection(connection),
	m_isServer(isServer),
	m_patchLocation(patchLocation)
{
	m_decoder.setEncryption(this->isEncrypted());
}

void MapleSession::start() {
	m_sessionManager->start(shared_from_this());
}

void MapleSession::handle_start() {
	m_connection->setSession(this);
	m_connection->setIp(m_socket.remote_endpoint().address().to_v4().to_ulong());

	if (m_isServer) {
		PacketCreator connectPacket = m_decoder.getConnectPacket(m_patchLocation);
		sendIv(connectPacket);
	}

	start_read_header();
}

void MapleSession::stop() {
	m_sessionManager->stop(shared_from_this());
}

void MapleSession::disconnect() {
	stop();
}

void MapleSession::handle_stop() {
	boost::system::error_code ec;
	m_socket.close(ec);
	if (ec) {
		std::cout << "FAILURE TO CLOSE SESSION (" << ec.value() << "): " << ec.message() << std::endl;
	}
}

void MapleSession::send(const PacketCreator &packet) {
	send(packet.getBuffer(), packet.getSize());
}

void MapleSession::send(const unsigned char *buf, int32_t len) {
	boost::mutex::scoped_lock l(m_sendMutex);

	size_t realLength = len + headerLen;
	unsigned char *buffer = new unsigned char[realLength];
	m_sendPacket.reset(buffer);

	memcpy(buffer + headerLen, buf, len);
	m_decoder.createHeader(buffer, (int16_t) len);
	m_decoder.encrypt(buffer + headerLen, len);

	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, realLength),
		boost::bind(&MapleSession::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void MapleSession::sendIv(const PacketCreator &packet) {
	boost::mutex::scoped_lock l(m_sendMutex);

	int32_t len = packet.getSize();
	unsigned char *buffer = new unsigned char[len];
	m_sendPacket.reset(buffer);

	memcpy(buffer, packet.getBuffer(), len);

	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, len),
		boost::bind(&MapleSession::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void MapleSession::start_read_header() {
	m_buffer.reset(new unsigned char[headerLen]);

	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_buffer.get(), headerLen),
		boost::bind(
			&MapleSession::handle_read_header, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void MapleSession::handle_write(const boost::system::error_code &error, size_t bytesTransferred) {
	boost::mutex::scoped_lock l(m_sendMutex);
	if (error) {
		disconnect();
	}
}

void MapleSession::handle_read_header(const boost::system::error_code &error, size_t bytesTransferred) {
	if (!error) {
		size_t len = Decoder::getLength(m_buffer.get(), isEncrypted());

		if (len < 2 || len > maxBufferLen) {
			// Hacking or trying to crash server
			disconnect();
		}

		m_buffer.reset(new unsigned char[len]);

		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_buffer.get(), len),
			boost::bind(
				&MapleSession::handle_read_body, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		disconnect();
	}
}

void MapleSession::handle_read_body(const boost::system::error_code &error, size_t bytesTransferred) {
	if (!error) {
		m_decoder.decrypt(m_buffer.get(), bytesTransferred);

		PacketReader packet(m_buffer.get(), bytesTransferred);
		m_connection->baseHandleRequest(packet);

		start_read_header();
	}
	else {
		disconnect();
	}
}

ip_t MapleSession::getIp() const {
	return m_connection->getIp();
}