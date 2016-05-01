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
#include "Common/AbstractServer.hpp"
#include "Common/CommonHeader.hpp"
#include "Common/CommonPacket.hpp"
#include "Common/ConnectionManager.hpp"
#include "Common/Decoder.hpp"
#include "Common/ExitCodes.hpp"
#include "Common/Logger.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/PacketHandler.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Randomizer.hpp"
#include "Common/TimeUtilities.hpp"
#include <functional>
#include <iostream>

namespace Vana {
Session::Session(
	asio::io_service &service,
	ConnectionManager &manager,
	Handler handler) :
	m_manager{manager},
	m_socket{service},
	m_handler{handler},
	m_ip{0}
{
}

auto Session::getSocket() -> asio::ip::tcp::socket & {
	return m_socket;
}

auto Session::getCodec() -> PacketTransformer & {
	return *m_codec;
}

auto Session::getBuffer() -> MiscUtilities::shared_array<unsigned char> & {
	return m_buffer;
}

auto Session::getLatency() const -> milliseconds_t {
	return m_latency;
}

auto Session::getType() const -> ConnectionType {
	return m_type;
}

auto Session::setType(ConnectionType type) -> void {
	if (m_type != ConnectionType::Unknown) throw InvalidOperationException{"Connection type may only be set once"};
	m_type = type;
}

auto Session::ping() -> void {
	if (m_pingCount == m_maxPingCount) {
		// We have a timeout now
		disconnect();
		return;
	}

	m_pingCount++;
	m_lastPing = TimeUtilities::getNow();
	send(Packets::ping());
}

auto Session::start(const PingConfig &ping, ref_ptr_t<PacketTransformer> transformer) -> void {
	// TODO FIXME support IPv6
	auto &addr = m_socket.remote_endpoint().address();
	if (addr.is_v4()) {
		m_ip = Ip{addr.to_v4().to_ulong()};
	}
	else {
		throw NotImplementedException{"IPv6"};
	}

	if (ping.enable) {
		m_maxPingCount = ping.timeoutPingCount;
		Timer::Timer::create(
			[this](const time_point_t &now) { this->ping(); },
			Timer::Id{TimerType::PingTimer},
			getTimers(),
			ping.initialDelay,
			ping.interval);
	}

	m_codec = transformer;

	m_handler->onConnectBase(shared_from_this());

	m_isConnected = true;
	startReadHeader();
}

auto Session::syncRead(size_t minimumBytes) -> pair_t<asio::error_code, PacketReader> {
	asio::error_code error;

	m_buffer.reset(new unsigned char[MaxBufferLen]);

	size_t packetSize = asio::read(m_socket,
		asio::buffer(m_buffer.get(), MaxBufferLen),
		asio::transfer_at_least(minimumBytes),
		error);

	return std::make_pair(error, PacketReader{m_buffer.get(), packetSize});
}

auto Session::disconnect() -> void {
	if (!m_isConnected) return;
	m_handler->onDisconnectBase();
	m_manager.stop(shared_from_this());
	m_isConnected = false;

	asio::error_code ec;
	m_socket.close(ec);
	if (ec) {
		m_manager.getServer()->log(LogType::Error, [&](out_stream_t &str) {
			str << "FAILURE TO CLOSE SESSION (" << ec.value() << "): " << ec.message();
		});
	}
}

auto Session::send(const PacketBuilder &builder, bool encrypt) -> void {
	send(builder.getBuffer(), builder.getSize(), encrypt);
}

auto Session::send(const unsigned char *buf, int32_t len, bool encrypt) -> void {
	owned_lock_t<mutex_t> l{m_sendMutex};

	unsigned char *sendBuffer;
	size_t realLength = len;

	if (encrypt) {
		realLength += HeaderLen;
		sendBuffer = new unsigned char[realLength];
		m_sendPacket.reset(sendBuffer);

		memcpy(sendBuffer + HeaderLen, buf, len);
		m_codec->setPacketHeader(sendBuffer, static_cast<uint16_t>(len));
		m_codec->encryptPacket(sendBuffer + HeaderLen, len, HeaderLen);
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
	m_buffer.reset(new unsigned char[HeaderLen]);

	asio::async_read(m_socket,
		asio::buffer(m_buffer.get(), HeaderLen),
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
	if (error) {
		disconnect();
		return;
	}

	// TODO FIXME
	// Figure out how to distinguish between client versions and server versions, can use this after
	//if (m_codec.testPacket(m_buffer.get()) == ValidityResult::Invalid) {
	//	// Hacking or trying to crash server
	//	disconnect();
	//	return;
	//}

	size_t len = m_codec->getPacketLength(m_buffer.get());
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

auto Session::handleReadBody(const asio::error_code &error, size_t bytesTransferred) -> void {
	if (error) {
		disconnect();
		return;
	}

	m_codec->decryptPacket(m_buffer.get(), bytesTransferred, HeaderLen);

	PacketReader packet{m_buffer.get(), bytesTransferred};
	baseHandleRequest(packet);

	startReadHeader();
}

auto Session::getIp() const -> const Ip & {
	return m_ip;
}

auto Session::baseHandleRequest(PacketReader &reader) -> void {
	try {
		switch (reader.peek<header_t>()) {
			case SMSG_PING:
				if (m_type != ConnectionType::EndUser) {
					send(Packets::pong());
				}
				break;
			case CMSG_PONG:
				if (m_pingCount == 0) {
					// Trying to spoof pongs without pings
					disconnect();
					return;
				}
				m_pingCount = 0;
				// This is for the trip to and from, so latency is averaged between them
				m_latency = duration_cast<milliseconds_t>(TimeUtilities::getNow() - m_lastPing) / 2;
				break;
		}

		if (m_handler->handle(reader) == Result::Failure) {
			disconnect();
		}
	}
	catch (std::exception &e) {
		// TODO FIXME log?
		std::cerr << "SESSION BASEHANDLEREQUEST ERROR: " << e.what() << std::endl;
	}
}

}