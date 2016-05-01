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
#pragma once

#include "Common/Ip.hpp"
#include "Common/ServerType.hpp"
#include "Common/Session.hpp"
#include "Common/Types.hpp"
#include <asio.hpp>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace Vana {
	class AbstractConnection;
	class AbstractServer;
	class ConnectionListener;
	class PacketHandler;
	struct ConnectionListenerConfig;
	struct PingConfig;

	class ConnectionManager {
	public:
		ConnectionManager(AbstractServer *server);
		~ConnectionManager();
		auto listen(const ConnectionListenerConfig &listener, HandlerCreator handlerCreator) -> void;
		auto connect(const Ip &destination, port_t port, const PingConfig &ping, ServerType sourceType, HandlerCreator handlerCreator) -> pair_t<Result, ref_ptr_t<Session>>;
		auto run() -> void;
		auto stop() -> void;
		auto stop(ref_ptr_t<Session> session) -> void;
		auto start(ref_ptr_t<Session> session) -> void;
		auto getServer() -> AbstractServer *;
	private:
		vector_t<ref_ptr_t<ConnectionListener>> m_servers;
		hash_set_t<ref_ptr_t<Session>> m_sessions;
		ref_ptr_t<thread_t> m_thread;
		owned_ptr_t<asio::io_service::work> m_work;
		asio::io_service m_ioService;
		AbstractServer *m_server;
	};
}