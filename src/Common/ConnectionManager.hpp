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

#include "Ip.hpp"
#include "Types.hpp"
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class AbstractConnection;
class Session;
struct InterServerConfig;

class ConnectionManager {
public:
	ConnectionManager();
	~ConnectionManager();
	auto accept(const Ip::Type &ipType, port_t port, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &subversion) -> void;
	auto connect(const Ip &serverIp, port_t serverPort, const InterServerConfig &config, AbstractConnection *connection) -> Result;
	auto run() -> void;
	auto stop() -> void;
	auto stop(ref_ptr_t<Session> session) -> void;
private:
	struct Listener {
		bool isServer = true;
		bool isPinging = true;
		bool isEncrypted = true;
		string_t subversion;
		boost::asio::ip::tcp::acceptor acceptor;
		function_t<AbstractConnection *()> connectionCreator;

		Listener() = delete;
		Listener(boost::asio::io_service &ioService, const boost::asio::ip::tcp::endpoint &endpoint, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &subversion);
	};

	auto acceptConnection(ref_ptr_t<Listener> listener) -> void;

	vector_t<ref_ptr_t<Listener>> m_servers;
	hash_set_t<ref_ptr_t<Session>> m_sessions;
	ref_ptr_t<thread_t> m_thread;
	owned_ptr_t<boost::asio::io_service::work> m_work;
	boost::asio::io_service m_ioService;
};