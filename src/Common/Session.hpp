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

#include "Common/Decoder.hpp"
#include "Common/ConnectionType.hpp"
#include "Common/Ip.hpp"
#include "Common/PacketTransformer.hpp"
#include "Common/PingConfig.hpp"
#include "Common/shared_array.hpp"
#include "Common/TimerContainerHolder.hpp"
#include "Common/Types.hpp"
#include <asio.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

namespace Vana {
	class ConnectionManager;
	class PacketBuilder;
	class PacketHandler;
	class PacketReader;
	class Session;

	using Handler = ref_ptr_t<PacketHandler>;
	using HandlerCreator = function_t<Handler()>;

	class Session : public enable_shared<Session>, public TimerContainerHolder {
	public:
		Session(
			asio::io_service &service,
			ConnectionManager &manager,
			Handler handler);

		auto disconnect() -> void;
		auto send(const PacketBuilder &builder, bool encrypt = true) -> void;
		auto getIp() const -> const Ip &;
		auto getLatency() const -> milliseconds_t;
		auto getType() const -> ConnectionType;
		auto setType(ConnectionType type) -> void;
	private:
		static const size_t HeaderLen = 4;
		static const size_t MaxBufferLen = 65535;

		auto syncRead(size_t minimumBytes) -> pair_t<asio::error_code, PacketReader>;
		auto startReadHeader() -> void;
		auto handleWrite(const asio::error_code &error, size_t bytesTransferred) -> void;
		auto handleReadHeader(const asio::error_code &error, size_t bytesTransferred) -> void;
		auto handleReadBody(const asio::error_code &error, size_t bytesTransferred) -> void;
		auto getSocket() -> asio::ip::tcp::socket &;
		auto getCodec() -> PacketTransformer &;
		auto getBuffer() -> MiscUtilities::shared_array<unsigned char> &;
		auto start(const PingConfig &ping, ref_ptr_t<PacketTransformer> transformer) -> void;
		auto send(const unsigned char *buf, int32_t len, bool encrypt = true) -> void;
		auto ping() -> void;
		auto baseHandleRequest(PacketReader &reader) -> void;

		friend class ConnectionManager;
		friend class ConnectionListener;

		bool m_isConnected = false;
		ConnectionType m_type = ConnectionType::Unknown;
		int8_t m_pingCount = 0;
		int32_t m_maxPingCount = 0;
		milliseconds_t m_latency = milliseconds_t{0};
		time_point_t m_lastPing;
		Handler m_handler;
		Ip m_ip;
		ConnectionManager &m_manager;
		asio::ip::tcp::socket m_socket;
		MiscUtilities::shared_array<unsigned char> m_buffer;
		MiscUtilities::shared_array<unsigned char> m_sendPacket;
		ref_ptr_t<PacketTransformer> m_codec;
		mutex_t m_sendMutex;
	};
}