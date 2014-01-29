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
#include "AbstractConnection.hpp"
#include "ConnectionAcceptor.hpp"
#include "Session.hpp"
#include <functional>

ConnectionAcceptor::ConnectionAcceptor(boost::asio::io_service &ioService, const boost::asio::ip::tcp::endpoint &endpoint, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &patchLocation) :
	m_acceptor(ioService, endpoint),
	m_connectionCreator(createConnection),
	m_config(config),
	m_patchLocation(patchLocation),
	m_isServer(isServer)
{
	m_sessionManager = make_ref_ptr<SessionManager>();
	startAccepting();
}

auto ConnectionAcceptor::stop() -> void {
	m_acceptor.close();
	m_sessionManager->stopAll();
}

auto ConnectionAcceptor::startAccepting() -> void {
	bool ping = m_isServer ? m_config.serverPing : m_config.clientPing;
	bool encrypt = m_config.clientEncryption || m_isServer;
	auto newSession = make_ref_ptr<Session>(m_acceptor.get_io_service(), m_sessionManager, m_connectionCreator(), true, encrypt, ping, m_patchLocation);

	m_acceptor.async_accept(newSession->getSocket(), [this, newSession](const boost::system::error_code &error) {
		if (!error) {
			newSession->start();
			startAccepting();
		}
	});
}