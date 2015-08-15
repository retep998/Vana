/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Session.hpp"
#include "AbstractConnection.hpp"
#include "ConnectionManager.hpp"
#include "Decoder.hpp"
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "Randomizer.hpp"
#include <functional>
#include <iostream>

Session::Session(asio::io_service &ioService, ConnectionManager &manager, AbstractConnection *connection, bool isForClient, bool isEncrypted, bool usePing, const string_t &subversion) :
	m_manager{manager},
	m_socket{ioService},
	m_connection{connection},
	m_isForClient{isForClient},
	m_subversion{subversion},
	m_decoder{!isForClient || isEncrypted},
	m_usePing{usePing}
{
}

auto Session::getSocket() -> asio::ip::tcp::socket & {
	return m_socket;
}

auto Session::getDecoder() -> Decoder & {
	return m_decoder;
}

auto Session::getBuffer() -> MiscUtilities::shared_array<unsigned char> & {
	return m_buffer;
}

auto Session::start() -> void {
	// TODO FIXME support IPv6
	m_connection->setSession(this, m_usePing, Ip(m_socket.remote_endpoint().address().to_v4().to_ulong()));

	if (m_isForClient) {
		m_decoder.setRecvIv(Randomizer::rand<uint32_t>());
		m_decoder.setSendIv(Randomizer::rand<uint32_t>());

		PacketBuilder connectPacket = getConnectPacket(m_subversion);
		send(connectPacket.getBuffer(), connectPacket.getSize(), false);
	}

	startReadHeader();
}

auto Session::disconnect() -> void {
	m_manager.stop(shared_from_this());

	asio::error_code ec;
	m_socket.close(ec);
	if (ec) {
		std::cerr << "FAILURE TO CLOSE SESSION (" << ec.value() << "): " << ec.message() << std::endl;
	}
}

auto Session::send(const PacketBuilder &builder) -> void {
	send(builder.getBuffer(), builder.getSize(), true);
}

auto Session::send(const unsigned char *buf, int32_t len, bool encrypt) -> void {
	owned_lock_t<mutex_t> l{m_sendMutex};

	unsigned char *sendBuffer;
	size_t realLength = len;

	if (encrypt) {
		realLength += headerLen;
		sendBuffer = new unsigned char[realLength];
		m_sendPacket.reset(sendBuffer);

		memcpy(sendBuffer + headerLen, buf, len);
		m_decoder.createHeader(sendBuffer, static_cast<uint16_t>(len));
		m_decoder.encrypt(sendBuffer + headerLen, len, headerLen);
	}
	else {
		sendBuffer = new unsigned char[realLength];
		m_sendPacket.reset(sendBuffer);

		memcpy(sendBuffer, buf, len);
	}

	asio::async_write(m_socket, asio::buffer(sendBuffer, realLength),
		std::bind(&Session::handleWrite, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

auto Session::startReadHeader() -> void {
	m_buffer.reset(new unsigned char[headerLen]);

	asio::async_read(m_socket,
		asio::buffer(m_buffer.get(), headerLen),
		std::bind(&Session::handleReadHeader, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

auto Session::handleWrite(const asio::error_code &error, size_t bytesTransferred) -> void {
	owned_lock_t<mutex_t> l{m_sendMutex};
	if (error) {
		disconnect();
	}
}

auto Session::handleReadHeader(const asio::error_code &error, size_t bytesTransferred) -> void {
	if (!error) {
		// TODO FIXME
		// Figure out how to distinguish between client versions and server versions, can use this after
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

		asio::async_read(m_socket,
			asio::buffer(m_buffer.get(), len),
			std::bind(&Session::handleReadBody, shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}
	else {
		disconnect();
	}
}

auto Session::handleReadBody(const asio::error_code &error, size_t bytesTransferred) -> void {
	if (!error) {
		m_decoder.decrypt(m_buffer.get(), bytesTransferred, headerLen);

		PacketReader packet{m_buffer.get(), bytesTransferred};
		m_connection->baseHandleRequest(packet);

		startReadHeader();
	}
	else {
		disconnect();
	}
}

auto Session::getIp() const -> const Ip & {
	return m_connection->getIp();
}

auto Session::getConnectPacket(const string_t &subversion) const -> PacketBuilder {
	PacketBuilder builder;
	builder
		.add<header_t>(0)
		.add<version_t>(MapleVersion::Version)
		.add<string_t>(subversion)
		.add<iv_t>(m_decoder.getRecvIv())
		.add<iv_t>(m_decoder.getSendIv())
		.add<game_locale_t>(MapleVersion::Locale);

	builder.set<header_t>(static_cast<header_t>(builder.getSize() - sizeof(header_t)), 0);
	return builder;
}