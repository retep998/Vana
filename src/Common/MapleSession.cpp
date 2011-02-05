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

MapleSession::MapleSession(boost::asio::io_service &io_service,
						   SessionManagerPtr sessionManager,
						   AbstractConnection *player,
						   bool isServer,
						   const string &patchLocation) :
AbstractSession(sessionManager),
m_socket(io_service),
m_player(player),
m_isServer(isServer),
m_patchLocation(patchLocation)
{
}

void MapleSession::start() {
	m_sessionManager->start(shared_from_this());
}

void MapleSession::handle_start() {
	m_player->setSession(this);
	m_player->setIp(m_socket.remote_endpoint().address().to_v4().to_ulong());

	if (m_isServer) {
		PacketCreator connectPacket = m_decoder.getConnectPacket(m_patchLocation);
		send(connectPacket, false);
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

void MapleSession::send(const unsigned char *buf, int32_t len, bool encrypt) {
	boost::mutex::scoped_lock l(m_sendMutex);
	size_t realLength = encrypt ? len + headerLen : len;
	unsigned char *buffer = new unsigned char[realLength];
	m_sendPacket.reset(buffer);

	if (encrypt) {
		memcpy(buffer + headerLen, buf, len);

		// Encrypt packet
		m_decoder.createHeader(buffer, (int16_t) len);
		m_decoder.encrypt(buffer + headerLen, len);
		m_decoder.next();
	}
	else {
		memcpy(buffer, buf, len);
	}

	boost::asio::async_write(m_socket, boost::asio::buffer(buffer, realLength),
		boost::bind(&MapleSession::handle_write, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void MapleSession::send(const PacketCreator &packet, bool encrypt) {
	return send(packet.getBuffer(), packet.getSize(), encrypt);
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

void MapleSession::handle_write(const boost::system::error_code &error, size_t bytes_transferred) {
	boost::mutex::scoped_lock l(m_sendMutex);
	if (error) {
		disconnect();
	}
}

void MapleSession::handle_read_header(const boost::system::error_code &error, size_t bytes_transferred) {
	if (!error) {
		size_t len = Decoder::getLength(m_buffer.get());

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

void MapleSession::handle_read_body(const boost::system::error_code &error, size_t bytes_transferred) {
	if (!error) {
		m_decoder.decrypt(m_buffer.get(), bytes_transferred);

		PacketReader packet(m_buffer.get(), bytes_transferred);
		m_player->handleRequest(packet);

		start_read_header();
	}
	else {
		disconnect();
	}
}

uint32_t MapleSession::getIp() const {
	return m_player->getIp();
}