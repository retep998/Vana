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
#include "MapleClient.h"
#include "AbstractPlayer.h"
#include "ReadPacket.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

MapleClient::MapleClient(boost::asio::io_service &io_service,
		const string &server, unsigned short port,
		SessionManagerPtr sessionManager,
		AbstractPlayer *player) :
MapleSession(io_service, sessionManager, player, false),
m_server(server),
m_port(port),
m_resolver(io_service)
{
}

void MapleClient::start_connect() {
	// Synchronously connect and process the connect packet

	tcp::resolver::query query(m_server, boost::lexical_cast<string>(m_port));
	tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
	tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_iterator != end) {
		m_socket.close();
		m_socket.connect(*endpoint_iterator++, error);
	}

	if (!error) {
		// Now let's process the connect packet
		readConnectPacket();

		// Start the normal MapleSession routine
		start();
	}
	else {
		std::cout << "Error: " << error.message() << std::endl;
	}
}

void MapleClient::readConnectPacket() {
	boost::system::error_code error;

	// Get the size of the connect packet
	boost::asio::read(m_socket,
		boost::asio::buffer(m_buffer, connectHeaderLen),
		boost::asio::transfer_all(), 
		error);

	if (error) {
		disconnect();
		return;
	}

	unsigned short packetLen = m_buffer[0] + m_buffer[1] * 0x100;

	// Get the rest of the packet
	boost::asio::read(m_socket,
		boost::asio::buffer(m_buffer, packetLen),
		boost::asio::transfer_all(),
		error);

	if (error) {
		disconnect();
		return;
	}

	// Now finally process it
	ReadPacket packet(m_buffer, packetLen);

	unsigned short version = packet.getShort(); // Maple Version, TODO: Verify it
	packet.getString(); // Unknown

	unsigned char *rawpacket = packet.getBuffer();
	m_decoder->setIvSend(rawpacket);
	m_decoder->setIvRecv(rawpacket + 4);
}
