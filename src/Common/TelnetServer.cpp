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
#include "TelnetServer.h"
#include "AbstractTelnetConnection.h"
#include "TelnetSession.h"
#include <boost/bind.hpp>

TelnetServer::TelnetServer(boost::asio::io_service &io_service,
						 const tcp::endpoint &endpoint,
						 AbstractTelnetConnectionFactory *atpf) :
m_acceptor(io_service, endpoint),
m_atpf(atpf),
m_sessionManager(new SessionManager)
{
	start_accept();
}

void TelnetServer::stop() {
	m_acceptor.close();
	m_sessionManager->stopAll();
}

void TelnetServer::start_accept() {
	TelnetSessionPtr new_session(new TelnetSession(m_acceptor.io_service(),
		m_sessionManager, m_atpf->createConnection()));

	m_acceptor.async_accept(new_session->getSocket(),
		boost::bind(&TelnetServer::handle_accept, this, new_session,
			boost::asio::placeholders::error));
}

void TelnetServer::handle_accept(TelnetSessionPtr new_session, const boost::system::error_code &error) {
	if (!error) {
		new_session->start();
		start_accept();
	}
}
