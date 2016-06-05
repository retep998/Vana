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

#include "common/ip_temp.hpp"
#include "common/server_type.hpp"
#include "common/session_temp.hpp"
#include "common/types_temp.hpp"
#include <asio.hpp>
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace vana {
	class abstract_connection;
	class abstract_server;
	class connection_listener;
	class packet_handler;
	struct connection_listener_config;
	struct ping_config;

	class connection_manager {
	public:
		connection_manager(abstract_server *server);
		~connection_manager();
		auto listen(const connection_listener_config &listener, handler_creator handler_creator) -> void;
		auto connect(const ip &destination, connection_port port, const ping_config &ping, server_type source_type, handler_creator handler_creator) -> pair<result, ref_ptr<session>>;
		auto run() -> void;
		auto stop() -> void;
		auto stop(ref_ptr<session> session) -> void;
		auto start(ref_ptr<session> session) -> void;
		auto get_server() -> abstract_server *;
	private:
		vector<ref_ptr<connection_listener>> m_servers;
		hash_set<ref_ptr<session>> m_sessions;
		ref_ptr<thread> m_thread;
		owned_ptr<asio::io_service::work> m_work;
		asio::io_service m_io_service;
		abstract_server *m_server;
	};
}