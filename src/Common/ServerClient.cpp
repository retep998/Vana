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
#include "ServerClient.hpp"
#include "AbstractConnection.hpp"
#include "ExitCodes.hpp"
#include "MapleVersion.hpp"
#include "PacketReader.hpp"
#include <cstdio>
#include <iostream>
#include <stdexcept>

ServerClient::ServerClient(boost::asio::io_service &ioService, const Ip &serverIp, port_t serverPort, ref_ptr_t<SessionManager> sessionManager, AbstractConnection *connection, bool ping) :
	Session(ioService, sessionManager, connection, false, true, ping, MapleVersion::LoginSubversion),
	m_server(serverIp),
	m_port(serverPort),
	m_resolver(ioService)
{
}

auto ServerClient::startConnect() -> void {
	// Synchronously connect and process the connect packet

	boost::asio::ip::address endAddress;
	if (m_server.getType() == Ip::Type::Ipv4) {
		endAddress = boost::asio::ip::address_v4(m_server.asIpv4());
	}
	else {
		throw std::invalid_argument("IPv6 unsupported");
	}
	boost::asio::ip::tcp::endpoint endpoint(endAddress, m_port);

	boost::system::error_code error;
	getSocket().connect(endpoint, error);

	if (!error) {
		// Now let's process the connect packet
		try {
			readConnectPacket();
			// Start the normal Session routine
			start();
		}
		catch (PacketContentException) {
			std::cerr << "ERROR: Malformed IV packet" << std::endl;
			disconnect();
			ExitCodes::exit(ExitCodes::ServerMalformedIvPacket);
		}
	}
	else {
		std::cerr << "ERROR: " << error.message() << std::endl;
		ExitCodes::exit(ExitCodes::ServerConnectionError);
	}
}

auto ServerClient::readConnectPacket() -> void {
	boost::system::error_code error;

	auto &buffer = getBuffer();
	buffer.reset(new unsigned char[maxBufferLen]);

	size_t packetSize = boost::asio::read(getSocket(),
		boost::asio::buffer(buffer.get(), maxBufferLen),
		boost::asio::transfer_at_least(10), // May require maintenance if the IV packet ever dips below 10 bytes
		error);

	if (error) {
		disconnect();
		return;
	}

	PacketReader reader(buffer.get(), packetSize);

	header_t header = reader.get<header_t>(); // Gives us the packet length
	version_t version = reader.get<version_t>();
	string_t subversion = reader.get<string_t>();
	uint32_t sendIv = reader.get<uint32_t>();
	uint32_t recvIv = reader.get<uint32_t>();
	locale_t locale = reader.get<locale_t>();

	if (version != MapleVersion::Version || locale != MapleVersion::Locale || subversion != MapleVersion::LoginSubversion) {
		std::cerr << "ERROR: The server you are connecting to lacks the same MapleStory version." << std::endl;
		std::cerr << "Expected locale/version (subversion): " << static_cast<int16_t>(locale) << "/" << version << " (" << subversion << ")" << std::endl;
		std::cerr << "Local locale/version (subversion): " << static_cast<int16_t>(MapleVersion::Locale) << "/" << MapleVersion::Version << " (" << MapleVersion::LoginSubversion << ")" << std::endl;
		disconnect();
		ExitCodes::exit(ExitCodes::ServerVersionMismatch);
	}

	auto &decoder = getDecoder();
	decoder.setSendIv(sendIv);
	decoder.setRecvIv(recvIv);
}