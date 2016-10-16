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
#pragma once

#include "common/config/ping.hpp"
#include "common/connection_type.hpp"
#include "common/ip.hpp"
#include "common/packet_transformer.hpp"
#include "common/timer/container_holder.hpp"
#include "common/types.hpp"
#include "common/util/shared_array.hpp"
#include <asio.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

namespace vana {
	class connection_manager;
	class packet_builder;
	class packet_handler;
	class packet_reader;
	class session;

	using handler = ref_ptr<packet_handler>;
	using handler_creator = function<handler()>;

	class session : public enable_shared<session>, public timer::container_holder {
	public:
		session(
			asio::io_service &service,
			connection_manager &manager,
			handler handler);

		auto disconnect() -> void;
		auto send(const packet_builder &builder, bool encrypt = true) -> void;
		auto get_ip() const -> const ip &;
		auto get_latency() const -> milliseconds;
		auto get_type() const -> connection_type;
		auto set_type(connection_type type) -> void;
	private:
		static const size_t header_len = 4;
		static const size_t max_buffer_len = 65535;

		auto sync_read(size_t minimum_bytes) -> pair<asio::error_code, packet_reader>;
		auto start_read_header() -> void;
		auto handle_write(const asio::error_code &error, size_t bytes_transferred) -> void;
		auto handle_read_header(const asio::error_code &error, size_t bytes_transferred) -> void;
		auto handle_read_body(const asio::error_code &error, size_t bytes_transferred) -> void;
		auto get_socket() -> asio::ip::tcp::socket &;
		auto get_codec() -> packet_transformer &;
		auto get_buffer() -> vana::util::shared_array<unsigned char> &;
		auto start(const config::ping &ping, ref_ptr<packet_transformer> transformer) -> void;
		auto send(const unsigned char *buf, int32_t len, bool encrypt = true) -> void;
		auto ping() -> void;
		auto base_handle_request(packet_reader &reader) -> void;

		friend class connection_manager;
		friend class connection_listener;

		bool m_is_connected = false;
		connection_type m_type = connection_type::unknown;
		int8_t m_ping_count = 0;
		int32_t m_max_ping_count = 0;
		milliseconds m_latency = milliseconds{0};
		time_point m_last_ping;
		handler m_handler;
		ip m_ip;
		connection_manager &m_manager;
		asio::ip::tcp::socket m_socket;
		vana::util::shared_array<unsigned char> m_buffer;
		vana::util::shared_array<unsigned char> m_send_packet;
		ref_ptr<packet_transformer> m_codec;
		mutex m_send_mutex;
	};
}