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
#include "Session.h"
#include "AbstractConnection.h"
#include "Decoder.h"
#include "IpUtilities.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "SessionManager.h"
#include <boost/bind.hpp>
#include <iostream>

Session::Session(boost::asio::io_service &ioService, SessionManagerPtr sessionManager, AbstractConnection *connection, bool isServer, bool isEncrypted, const string &patchLocation) :
	// Apparently, "isServer" is true from sessions created by the server for the client
	// In addition, it's false from sessions created for the server clients
	AbstractSession(sessionManager, (!isServer || isEncrypted)),
	m_socket(ioService),
	m_connection(connection),
	m_isServer(isServer),
	m_patchLocation(patchLocation),
	m_decoder(!isServer || isEncrypted)
{
}

void Session::start() {
	m_sessionManager->start(shared_from_this());
}

void Session::handleStart() {
	m_connection->setSession(this);
	m_connection->setIp(m_socket.remote_endpoint().address().to_v4().to_ulong());

	if (m_isServer) {
		PacketCreator connectPacket = m_decoder.getConnectPacket(m_patchLocation);
		sendIv(connectPacket);
	}

	startReadHeader();
}

void Session::stop() {
	m_sessionManager->stop(shared_from_this());
}

void Session::disconnect() {
	stop();
}

void Session::handleStop() {
	boost::system::error_code ec;
	m_socket.close(ec);
	if (ec) {
		std::cerr << "FAILURE TO CLOSE SESSION (" << ec.value() << "): " << ec.message() << std::endl;
	}
}

void Session::send(const PacketCreator &packet) {
	send(packet.getBuffer(), packet.getSize());
}

void Session::send(const unsigned char *buf, int32_t len) {
	boost::mutex::scoped_lock l(m_sendMutex);

	size_t realLength = len + headerLen;
	unsigned char *buffer = new unsigned char[realLength];
	m_sendPacket.reset(buffer);

	memcpy(buffer + headerLen, buf, len);
	m_decoder.createHeader(buffer, (int16_t) len);
	m_decoder.encrypt(buffer + headerLen, len);

	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, realLength),
		boost::bind(&Session::handleWrite, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Session::sendIv(const PacketCreator &packet) {
	boost::mutex::scoped_lock l(m_sendMutex);

	int32_t len = packet.getSize();
	unsigned char *buffer = new unsigned char[len];
	m_sendPacket.reset(buffer);

	memcpy(buffer, packet.getBuffer(), len);

	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, len),
		boost::bind(&Session::handleWrite, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void Session::startReadHeader() {
	m_buffer.reset(new unsigned char[headerLen]);

	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_buffer.get(), headerLen),
		boost::bind(
			&Session::handleReadHeader, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void Session::handleWrite(const boost::system::error_code &error, size_t bytesTransferred) {
	boost::mutex::scoped_lock l(m_sendMutex);
	if (error) {
		disconnect();
	}
}

void Session::handleReadHeader(const boost::system::error_code &error, size_t bytesTransferred) {
	if (!error) {
		// TODO: Figure out how to distinguish between client versions and server versions, can use this after
		//if (!m_decoder.validPacket(m_buffer.get())) {
		//	// Hacking or trying to crash server
		//	disconnect();
		//	return;
		//}

		size_t len = m_decoder.getLength(m_buffer.get());
		if (len < 2) {
			// Hacking or trying to crash server
			disconnect();
			return;
		}

		m_buffer.reset(new unsigned char[len]);

		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_buffer.get(), len),
			boost::bind(
				&Session::handleReadBody, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		disconnect();
	}
}

void Session::handleReadBody(const boost::system::error_code &error, size_t bytesTransferred) {
	if (!error) {
		m_decoder.decrypt(m_buffer.get(), bytesTransferred);

		PacketReader packet(m_buffer.get(), bytesTransferred);
		m_connection->baseHandleRequest(packet);

		startReadHeader();
	}
	else {
		disconnect();
	}
}

ip_t Session::getIp() const {
	return m_connection->getIp();
}