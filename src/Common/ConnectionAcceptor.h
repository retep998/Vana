/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "Configuration.h"
#include "Session.h"
#include "SessionManager.h"
#include <string>
#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

using std::string;
using boost::asio::ip::tcp;

class AbstractConnectionFactory;

class ConnectionAcceptor {
public:
	ConnectionAcceptor(boost::asio::io_service &ioService, const tcp::endpoint &endpoint, AbstractConnectionFactory *apf, const LoginConfig &loginConfig, bool isServer, const string &patchLocation);
	void stop();
private:
	void startAccepting();
	void handleConnection(SessionPtr newSession, const boost::system::error_code &error);

	tcp::acceptor m_acceptor;
	boost::scoped_ptr<AbstractConnectionFactory> m_apf;
	SessionManagerPtr m_sessionManager;
	string m_patchLocation;
	LoginConfig m_loginConfig;
	bool m_isServer;
};

typedef boost::shared_ptr<ConnectionAcceptor> ConnectionAcceptorPtr;