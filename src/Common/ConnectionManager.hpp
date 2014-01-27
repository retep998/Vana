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
#pragma once

#include "ConnectionAcceptor.hpp"
#include "Ip.hpp"
#include "SessionManager.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <thread>
#include <vector>

class AbstractConnectionFactory;
class ConnectionAcceptor;

class ConnectionManager {
	SINGLETON_CUSTOM_CONSTRUCTOR(ConnectionManager);
public:
	auto accept(const Ip::Type &ipType, port_t port, AbstractConnectionFactory *acf, const LoginConfig &loginConfig, bool isServer, const string_t &patchLocation = "") -> void;
	auto connect(const Ip &serverIp, port_t serverPort, const LoginConfig &loginConfig, AbstractConnection *connection) -> void;
	auto stop() -> void;
	auto run() -> void;
	auto join() -> void;
private:
	auto handleRun() -> void;
	auto handleStop() -> void;

	vector_t<ref_ptr_t<ConnectionAcceptor>> m_servers;
	ref_ptr_t<SessionManager> m_clients;
	owned_ptr_t<thread_t> m_thread;
	owned_ptr_t<boost::asio::io_service::work> m_work;
	boost::asio::io_service m_ioService;
};