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
#include "MapleServerClient.h"
#include "AbstractConnection.h"
#include "ExitCodes.h"
#include "MapleVersion.h"
#include "PacketReader.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <iostream>
#include <stdexcept>

using std::cout;
using std::endl;

MapleServerClient::MapleServerClient(boost::asio::io_service &io_service,
		uint32_t server, uint16_t port,
		SessionManagerPtr sessionManager,
		AbstractConnection *player) :
MapleSession(io_service, sessionManager, player, false),
m_server(server),
m_port(port),
m_resolver(io_service)
{
}

void MapleServerClient::start_connect() {
	// Synchronously connect and process the connect packet

	tcp::endpoint endpoint(boost::asio::ip::address_v4(m_server), m_port);
	boost::system::error_code error;
	m_socket.connect(endpoint, error);

	if (!error) {
		// Now let's process the connect packet
		try {
			readConnectPacket();

			// Start the normal MapleSession routine
			start();
		}
		catch (std::range_error) {
			cout << "Error: Malformed IV packet" << endl;
			cout << "Press enter to quit ...";
			disconnect();
			getchar();
			exit(ExitCodes::ServerConnectionError);
		}
	}
	else {
		cout << "Error: " << error.message() << endl;
		cout << "Press enter to quit ...";
		getchar();
		exit(ExitCodes::ServerConnectionError);
	}
}

void MapleServerClient::readConnectPacket() {
	boost::system::error_code error;

	m_buffer.reset(new unsigned char[maxBufferLen]);

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
	uint32_t sendIv = packet.get<uint32_t>(); // Using the packet buffer directly is unsafe
	uint32_t recvIv = packet.get<uint32_t>();
	int8_t locale = packet.get<int8_t>();

	if (version != MapleVersion::Version || locale != MapleVersion::Locale) {
		cout << "ERROR: The server you are connecting to lacks the same MapleStory version." << endl;
		cout << "Expected locale/version: " << static_cast<int16_t>(locale) << "/" << version << endl;
		cout << "Local locale/version: " << static_cast<int16_t>(MapleVersion::Locale) << "/" << MapleVersion::Version << endl;
		cout << "Press enter to quit ...";
		disconnect();
		getchar();
		exit(ExitCodes::ServerVersionMismatch);
	}

	m_decoder.setIvSend(reinterpret_cast<unsigned char*>(&sendIv));
	m_decoder.setIvRecv(reinterpret_cast<unsigned char*>(&recvIv));
}
