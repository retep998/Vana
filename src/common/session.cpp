/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "session.hpp"
#include "common/abstract_server.hpp"
#include "common/common_header.hpp"
#include "common/common_packet.hpp"
#include "common/connection_manager.hpp"
#include "common/exit_code.hpp"
#include "common/log/base_logger.hpp"
#include "common/packet_builder.hpp"
#include "common/packet_handler.hpp"
#include "common/packet_reader.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/time.hpp"
#include <functional>
#include <iostream>

namespace vana {
session::session(
	asio::io_service &service,
	connection_manager &manager,
	handler handler) :
	m_manager{manager},
	m_socket{service},
	m_handler{handler},
	m_ip{0}
{
}

auto session::get_socket() -> asio::ip::tcp::socket & {
	return m_socket;
}

auto session::get_codec() -> packet_transformer & {
	return *m_codec;
}

auto session::get_buffer() -> vana::util::shared_array<unsigned char> & {
	return m_buffer;
}

auto session::get_latency() const -> milliseconds {
	return m_latency;
}

auto session::get_type() const -> connection_type {
	return m_type;
}

auto session::set_type(connection_type type) -> void {
	if (m_type != connection_type::unknown) THROW_CODE_EXCEPTION(invalid_operation_exception, "Connection type may only be set once");
	m_type = type;
}

auto session::ping() -> void {
	if (m_ping_count == m_max_ping_count) {
		// We have a timeout now
		disconnect();
		return;
	}

	m_ping_count++;
	m_last_ping = vana::util::time::get_now();
	send(packets::ping());
}

auto session::start(const config::ping &ping, ref_ptr<packet_transformer> transformer) -> void {
	// TODO FIXME support IPv6
	auto &addr = m_socket.remote_endpoint().address();
	if (addr.is_v4()) {
		m_ip = ip{addr.to_v4().to_ulong()};
	}
	else {
		THROW_CODE_EXCEPTION(not_implemented_exception, "i_pv6");
	}

	if (ping.enable) {
		m_max_ping_count = ping.timeout_ping_count;
		timer::timer::create(
			[this](const time_point &now) { this->ping(); },
			timer::id{timer::type::ping_timer},
			get_timers(),
			ping.initial_delay,
			ping.interval);
	}

	m_codec = transformer;

	m_handler->on_connect_base(shared_from_this());

	m_is_connected = true;
	start_read_header();
}

auto session::sync_read(size_t minimum_bytes) -> pair<asio::error_code, packet_reader> {
	asio::error_code error;

	m_buffer.reset(new unsigned char[max_buffer_len]);

	size_t packet_size = asio::read(m_socket,
		asio::buffer(m_buffer.get(), max_buffer_len),
		asio::transfer_at_least(minimum_bytes),
		error);

	return std::make_pair(error, packet_reader{m_buffer.get(), packet_size});
}

auto session::disconnect() -> void {
	if (!m_is_connected) return;
	m_handler->on_disconnect_base();
	m_manager.stop(shared_from_this());
	m_is_connected = false;

	asio::error_code ec;
	m_socket.close(ec);
	if (ec) {
		m_manager.get_server()->log(vana::log::type::error, [&](out_stream &str) {
			str << "FAILURE TO CLOSE SESSION (" << ec.value() << "): " << ec.message();
		});
	}
}

auto session::send(const packet_builder &builder, bool encrypt) -> void {
	send(builder.get_buffer(), builder.get_size(), encrypt);
}

auto session::send(const unsigned char *buf, int32_t len, bool encrypt) -> void {
	owned_lock<mutex> l{m_send_mutex};

	unsigned char *send_buffer;
	size_t real_length = len;

	if (encrypt) {
		real_length += header_len;
		send_buffer = new unsigned char[real_length];
		m_send_packet.reset(send_buffer);

		memcpy(send_buffer + header_len, buf, len);
		m_codec->set_packet_header(send_buffer, static_cast<uint16_t>(len));
		m_codec->encrypt_packet(send_buffer + header_len, len, header_len);
	}
	else {
		send_buffer = new unsigned char[real_length];
		m_send_packet.reset(send_buffer);

		memcpy(send_buffer, buf, len);
	}

	asio::async_write(m_socket, asio::buffer(send_buffer, real_length),
		std::bind(&session::handle_write, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

auto session::start_read_header() -> void {
	m_buffer.reset(new unsigned char[header_len]);

	asio::async_read(m_socket,
		asio::buffer(m_buffer.get(), header_len),
		std::bind(&session::handle_read_header, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

auto session::handle_write(const asio::error_code &error, size_t bytes_transferred) -> void {
	owned_lock<mutex> l{m_send_mutex};
	if (error) {
		disconnect();
	}
}

auto session::handle_read_header(const asio::error_code &error, size_t bytes_transferred) -> void {
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

	size_t len = m_codec->get_packet_length(m_buffer.get());
	if (len < 2) {
		// Hacking or trying to crash server
		disconnect();
		return;
	}

	m_buffer.reset(new unsigned char[len]);

	asio::async_read(m_socket,
		asio::buffer(m_buffer.get(), len),
		std::bind(&session::handle_read_body, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

auto session::handle_read_body(const asio::error_code &error, size_t bytes_transferred) -> void {
	if (error) {
		disconnect();
		return;
	}

	m_codec->decrypt_packet(m_buffer.get(), bytes_transferred, header_len);

	packet_reader packet{m_buffer.get(), bytes_transferred};
	base_handle_request(packet);

	start_read_header();
}

auto session::get_ip() const -> const ip & {
	return m_ip;
}

auto session::base_handle_request(packet_reader &reader) -> void {
	try {
		switch (reader.peek<packet_header>()) {
			case SMSG_PING:
				if (m_type != connection_type::end_user) {
					send(packets::pong());
				}
				break;
			case CMSG_PONG:
				if (m_ping_count == 0) {
					// Trying to spoof pongs without pings
					disconnect();
					return;
				}
				m_ping_count = 0;
				// This is for the trip to and from, so latency is averaged between them
				m_latency = duration_cast<milliseconds>(vana::util::time::get_now() - m_last_ping) / 2;
				break;
		}

		if (m_handler->handle(reader) == result::failure) {
			disconnect();
		}
	}
#if DEBUG
	catch (not_implemented_exception &e) {
		std::cerr << "SESSION HIT UNIMPLEMENTED FEATURE AT " << e.get_file() << " L" << e.get_line() << ": " << e.what() << std::endl;
	}
	catch (invalid_operation_exception &e) {
		std::cerr << "SESSION HIT INVALID OPERATION AT " << e.get_file() << " L" << e.get_line() << ": " << e.what() << std::endl;
	}
	catch (codepath_invalid_exception &e) {
		std::cerr << "SESSION HIT INVALID CODE PATH AT " << e.get_file() << " L" << e.get_line() << ": " << e.what() << std::endl;
	}
#endif
	catch (std::exception &e) {
		// TODO FIXME log?
		std::cerr << "SESSION BASEHANDLEREQUEST ERROR: " << e.what() << std::endl;
	}
}

}