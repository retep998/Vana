/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "ConnectionManager.h"
#include "AbstractConnection.h"
#include "Configuration.h"
#include "ServerClient.h"
#include <algorithm>
#include <functional>

ConnectionManager * ConnectionManager::singleton = nullptr;

ConnectionManager::ConnectionManager()
{
	m_clients = std::make_shared<SessionManager>();
	m_work = std::make_unique<boost::asio::io_service::work>(m_ioService);
}

void ConnectionManager::accept(const Ip::Type &ipType, port_t port, AbstractConnectionFactory *acf, const LoginConfig &loginConfig, bool isServer, const string &patchLocation) {
	tcp::endpoint endpoint(ipType == Ip::Type::Ipv4 ? tcp::v4() : tcp::v6(), port);
	m_servers.push_back(std::make_shared<ConnectionAcceptor>(m_ioService, endpoint, acf, loginConfig, isServer, patchLocation));
}

void ConnectionManager::connect(const Ip &serverIp, port_t serverPort, const LoginConfig &loginConfig, AbstractConnection *connection) {
	ServerClientPtr c = std::make_shared<ServerClient>(m_ioService, serverIp, serverPort, m_clients, connection, loginConfig.serverPing);
	c->startConnect();
}

void ConnectionManager::stop() {
	// Post a call to io_service so it is safe to call from all threads
	m_ioService.post(std::bind(&ConnectionManager::handleStop, this));
}

void ConnectionManager::run() {
	m_thread = std::make_unique<std::thread>(std::bind(&ConnectionManager::handleRun, this));
}

void ConnectionManager::join() {
	m_thread->join();
}

void ConnectionManager::handleRun() {
	m_ioService.run();
}

void ConnectionManager::handleStop() {
	std::for_each(m_servers.begin(), m_servers.end(), std::bind(&ConnectionAcceptor::stop, std::placeholders::_1));

	m_clients->stopAll();

	// Destroy the "work" so io_service would return
	m_work.reset();
}