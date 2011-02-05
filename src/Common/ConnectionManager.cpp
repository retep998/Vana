/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "AbstractTelnetConnection.h"
#include <algorithm>
#include <boost/bind.hpp>

ConnectionManager * ConnectionManager::singleton = nullptr;

ConnectionManager::ConnectionManager() :
m_clients(new SessionManager),
m_work(new boost::asio::io_service::work(m_ioService))
{
}

void ConnectionManager::accept(uint16_t port, AbstractConnectionFactory *apf, const string &patchLocation) {
	tcp::endpoint endpoint(tcp::v4(), port);
	m_servers.push_back(MapleServerPtr(new MapleServer(m_ioService, endpoint, apf, patchLocation)));
}

void ConnectionManager::accept(uint16_t port, AbstractTelnetConnectionFactory *atpf) {
	tcp::endpoint endpoint(tcp::v4(), port);
	m_telnetServers.push_back(TelnetServerPtr(new TelnetServer(m_ioService, endpoint, atpf)));
}

void ConnectionManager::connect(uint32_t server, uint16_t port, AbstractConnection *player) {
	MapleServerClientPtr c = MapleServerClientPtr(new MapleServerClient(m_ioService, server, port, m_clients, player));
	c->start_connect();
}

void ConnectionManager::stop() {
	// Post a call to ioService so it is safe to call from all threads
	m_ioService.post(boost::bind(&ConnectionManager::handle_stop, this));
}

void ConnectionManager::run() {
	m_thread.reset(new boost::thread(boost::bind(&ConnectionManager::handle_run, this)));
}

void ConnectionManager::join() {
	m_thread->join();
}

void ConnectionManager::handle_run() {
	m_ioService.run();
}

void ConnectionManager::handle_stop() {
	std::for_each(m_servers.begin(), m_servers.end(),
		boost::bind(&MapleServer::stop, _1));

	std::for_each(m_telnetServers.begin(), m_telnetServers.end(),
		boost::bind(&TelnetServer::stop, _1));

	m_clients->stopAll();

	// Destroy the "work" so ioService would return
	m_work.reset();
}
