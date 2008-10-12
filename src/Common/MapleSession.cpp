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
#include "MapleSession.h"
#include "AbstractPlayer.h"
#include "Decoder.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "SessionManager.h"
#include <boost/bind.hpp>

MapleSession::MapleSession(boost::asio::io_service &io_service,
						   SessionManagerPtr sessionManager,
						   AbstractPlayer *player, bool isServer,
						   string connectPacketUnknown) :
AbstractSession(sessionManager),
m_socket(io_service),
m_decoder(new Decoder),
m_player(player),
m_is_server(isServer),
m_connect_packet_unknown(connectPacketUnknown)
{
}

void MapleSession::start() {
	m_session_manager->start(shared_from_this());
}

void MapleSession::handle_start() {
	m_player->setSession(this);
	m_player->setIP(m_socket.remote_endpoint().address().to_string());

	if (m_is_server) {
		std::cout << "Accepted connection from " << m_player->getIP() << std::endl;

		PacketCreator connectPacket = m_decoder->getConnectPacket(m_connect_packet_unknown);
		send(connectPacket, false);
	}

	start_read_header();
}

void MapleSession::stop() {
	m_session_manager->stop(shared_from_this());
}

void MapleSession::disconnect() {
	stop();
}

void MapleSession::handle_stop() {
	m_socket.close();
}

void MapleSession::send(unsigned char *buf, int32_t len, bool encrypt) {
	boost::mutex::scoped_lock l(m_send_mutex);
	unsigned char bufs[bufferLen];
	size_t realLength;
	if (encrypt) {
		// Encrypt packet
		m_decoder->createHeader((unsigned char *) bufs, (int16_t) len);
		m_decoder->encrypt(buf, len);
		m_decoder->next();

		memcpy_s(bufs + 4, len, buf, len);
		realLength = len + 4;
	}
	else {
		memcpy_s(bufs, len, buf, len);
		realLength = len;
	}

	// Send encrypted packet
	boost::asio::async_write(m_socket, boost::asio::buffer(bufs, realLength),
		boost::bind(&MapleSession::handle_write, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void MapleSession::send(const PacketCreator &packet, bool encrypt) {
	unsigned char tempbuf[bufferLen];
	memcpy_s(tempbuf, bufferLen, packet.getBuffer(), bufferLen); // Copying to tempbuf so the packet doesn't get emptied on send and can be sent to other players
	
	return send(tempbuf, packet.getSize(), encrypt);
}

void MapleSession::start_read_header() {
	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_buffer, headerLen),
		boost::bind(
			&MapleSession::handle_read_header, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void MapleSession::handle_write(const boost::system::error_code &error, 
								size_t bytes_transferred) {
	if (error) {
		disconnect();
	}
}

void MapleSession::handle_read_header(const boost::system::error_code &error,
									  size_t bytes_transferred) {
	if (!error) {
		size_t len = Decoder::getLength(m_buffer);

		if (len < 2 || len > bufferLen) {
			// Hacking or trying to crash server
			disconnect();
		}

		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_buffer, len),
			boost::bind(
				&MapleSession::handle_read_body, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else {
		disconnect();
	}
}

void MapleSession::handle_read_body(const boost::system::error_code &error,
									size_t bytes_transferred) {
	if (!error) {
		m_decoder->decrypt(m_buffer, bytes_transferred);

		PacketReader packet(m_buffer, bytes_transferred);
		m_player->handleRequest(packet);

		start_read_header();
	}
	else {
		disconnect();
	}
}
