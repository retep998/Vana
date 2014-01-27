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

ConnectionAcceptor::ConnectionAcceptor(boost::asio::io_service &ioService, const boost::asio::ip::tcp::endpoint &endpoint, AbstractConnectionFactory *apf, const LoginConfig &loginConfig, bool isServer, const string_t &patchLocation) :
	m_acceptor(ioService, endpoint),
	m_apf(apf),
	m_patchLocation(patchLocation),
	m_loginConfig(loginConfig),
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
	bool ping = (m_isServer ? m_loginConfig.serverPing : m_loginConfig.clientPing);
	ref_ptr_t<Session> newSession = make_ref_ptr<Session>(m_acceptor.get_io_service(), m_sessionManager, m_apf->createConnection(), true, m_loginConfig.clientEncryption || m_isServer, ping, m_patchLocation);
	m_acceptor.async_accept(newSession->getSocket(), std::bind(&ConnectionAcceptor::handleConnection, this, newSession, std::placeholders::_1));
}

auto ConnectionAcceptor::handleConnection(ref_ptr_t<Session> newSession, const boost::system::error_code &error) -> void {
	if (!error) {
		newSession->start();
		startAccepting();
	}
}