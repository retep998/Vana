/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "ServerClient.h"
#include "AbstractConnection.h"
#include "ExitCodes.h"
#include "MapleVersion.h"
#include "PacketReader.h"
#include <cstdio>
#include <iostream>
#include <stdexcept>

using std::endl;

ServerClient::ServerClient(boost::asio::io_service &ioService, const Ip &serverIp, port_t serverPort, SessionManagerPtr sessionManager, AbstractConnection *connection, bool ping) :
	Session(ioService, sessionManager, connection, false, true, ping),
	m_server(serverIp),
	m_port(serverPort),
	m_resolver(ioService)
{
}

void ServerClient::startConnect() {
	// Synchronously connect and process the connect packet

	boost::asio::ip::address endAddress;
	if (m_server.getType() == Ip::Type::Ipv4) {
		endAddress = boost::asio::ip::address_v4(m_server.asIpv4());
	}
	else {
		throw std::invalid_argument("IPv6 unsupported");
	}
	tcp::endpoint endpoint(endAddress, m_port);

	boost::system::error_code error;
	m_socket.connect(endpoint, error);

	if (!error) {
		// Now let's process the connect packet
		try {
			readConnectPacket();
			// Start the normal Session routine
			start();
		}
		catch (std::range_error) {
			std::cerr << "ERROR: Malformed IV packet" << endl;
			disconnect();
			ExitCodes::exit(ExitCodes::ServerMalformedIvPacket);
		}
	}
	else {
		std::cerr << "ERROR: " << error.message() << endl;
		ExitCodes::exit(ExitCodes::ServerConnectionError);
	}
}

void ServerClient::readConnectPacket() {
	boost::system::error_code error;

	m_buffer.reset(new unsigned char[maxBufferLen]);

	// Get the size of the connect packet
	size_t packetSize = boost::asio::read(m_socket,
		boost::asio::buffer(m_buffer.get(), maxBufferLen),
		boost::asio::transfer_at_least(10), // May require maintenance if the IV packet ever dips below 10 bytes
		error);

	if (error) {
		disconnect();
		return;
	}

	// Now process it
	PacketReader packet(m_buffer.get(), packetSize);

	packet.skipBytes(2); // Header, unimportant because this isn't a client that might need to be patched
	int16_t version = packet.get<int16_t>();
	uint16_t stringSize = packet.get<uint16_t>();
	packet.skipBytes(stringSize); // Patch location, unimportant for the same reason as header
	uint32_t sendIv = packet.get<uint32_t>();
	uint32_t recvIv = packet.get<uint32_t>();
	int8_t locale = packet.get<int8_t>();

	if (version != MapleVersion::Version || locale != MapleVersion::Locale) {
		std::cerr << "ERROR: The server you are connecting to lacks the same MapleStory version." << endl;
		std::cout << "Expected locale/version: " << static_cast<int16_t>(locale) << "/" << version << endl;
		std::cout << "Local locale/version: " << static_cast<int16_t>(MapleVersion::Locale) << "/" << MapleVersion::Version << endl;
		disconnect();
		ExitCodes::exit(ExitCodes::ServerVersionMismatch);
	}
	m_decoder.setSendIv(sendIv);
	m_decoder.setRecvIv(recvIv);
}