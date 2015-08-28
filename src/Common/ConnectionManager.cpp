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
#include "ConnectionManager.hpp"
#include "AbstractConnection.hpp"
#include "InterServerConfig.hpp"
#include "ServerClient.hpp"
#include "ThreadPool.hpp"

namespace Vana {

ConnectionManager::ConnectionManager(AbstractServer *server) :
	m_server{server}
{
	m_work = make_owned_ptr<asio::io_service::work>(m_ioService);
}

ConnectionManager::~ConnectionManager() {
	// m_work.reset() needs to be a pre-wait hook and in the destructor for the cases where the thread is never leased (e.g. DB unavailable)
	// Doing this a second time doesn't harm an already-reset m_work pointer, so we're in the clear
	m_work.reset();
	m_thread.reset();
}

ConnectionManager::Listener::Listener(asio::io_service &ioService, const asio::ip::tcp::endpoint &endpoint, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &subversion) :
	acceptor{ioService, endpoint},
	isPinging{isServer ? config.serverPing : config.clientPing},
	isEncrypted{config.clientEncryption || isServer},
	connectionCreator{createConnection},
	subversion{subversion},
	isServer{isServer}
{
}

auto ConnectionManager::accept(const Ip::Type &ipType, port_t port, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &subversion) -> void {
	asio::ip::tcp::endpoint endpoint{
		ipType == Ip::Type::Ipv4 ?
			asio::ip::tcp::v4() :
			asio::ip::tcp::v6(),
		port};

	auto listener = make_ref_ptr<Listener>(m_ioService, endpoint, createConnection, config, isServer, subversion);
	m_servers.push_back(listener);
	acceptConnection(listener);
}

auto ConnectionManager::connect(const Ip &serverIp, port_t serverPort, const InterServerConfig &config, AbstractConnection *connection) -> Result {
	auto serverConnection = make_ref_ptr<ServerClient>(m_ioService, serverIp, serverPort, *this, connection, config.serverPing);
	m_sessions.insert(serverConnection);
	return serverConnection->startConnect();
}

auto ConnectionManager::stop() -> void {
	for (auto &server : m_servers) {
		server->acceptor.close();
	}
	m_servers.clear();

	auto sessions = m_sessions;
	for (auto &session : sessions) {
		session->disconnect();
	}
}

auto ConnectionManager::stop(ref_ptr_t<Session> session) -> void {
	m_sessions.erase(session);
}

auto ConnectionManager::getServer() -> AbstractServer * {
	return m_server;
}

auto ConnectionManager::run() -> void {
	m_thread = ThreadPool::lease(
		[this] { m_ioService.run(); },
		[this] { m_work.reset(); });
}

auto ConnectionManager::acceptConnection(ref_ptr_t<Listener> listener) -> void {
	auto newSession = make_ref_ptr<Session>(listener->acceptor.get_io_service(),
		*this,
		listener->connectionCreator(),
		true,
		listener->isEncrypted,
		listener->isPinging,
		listener->subversion);

	m_sessions.insert(newSession);
	listener->acceptor.async_accept(newSession->getSocket(), [this, listener, newSession](const asio::error_code &error) mutable {
		if (!error) {
			newSession->start();
			this->acceptConnection(listener);
		}
	});
}

}