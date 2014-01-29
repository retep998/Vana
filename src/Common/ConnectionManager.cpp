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
#include "ConnectionManager.hpp"
#include "AbstractConnection.hpp"
#include "Configuration.hpp"
#include "ServerClient.hpp"

ConnectionManager::ConnectionManager()
{
	m_clients = make_ref_ptr<SessionManager>();
	m_work = make_owned_ptr<boost::asio::io_service::work>(m_ioService);
}

auto ConnectionManager::accept(const Ip::Type &ipType, port_t port, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &patchLocation) -> void {
	boost::asio::ip::tcp::endpoint endpoint(ipType == Ip::Type::Ipv4 ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6(), port);
	m_servers.push_back(make_ref_ptr<ConnectionAcceptor>(m_ioService, endpoint, createConnection, config, isServer, patchLocation));
}

auto ConnectionManager::connect(const Ip &serverIp, port_t serverPort, const InterServerConfig &config, AbstractConnection *connection) -> void {
	auto c = make_ref_ptr<ServerClient>(m_ioService, serverIp, serverPort, m_clients, connection, config.serverPing);
	c->startConnect();
}

auto ConnectionManager::stop() -> void {
	// Post a call to io_service so it is safe to call from all threads
	m_ioService.post([this]() { this->handleStop(); });
}

auto ConnectionManager::run() -> void {
	m_thread = make_owned_ptr<thread_t>([this]() { this->handleRun(); });
}

#ifdef WIN32
auto ConnectionManager::join() -> void {
	m_thread->join();
}
#endif

auto ConnectionManager::handleRun() -> void {
	m_ioService.run();
}

auto ConnectionManager::handleStop() -> void {
	for (const auto &server : m_servers) {
		server->stop();
	}

	m_clients->stopAll();

	// Destroy the "work" so io_service would return
	m_work.reset();
}