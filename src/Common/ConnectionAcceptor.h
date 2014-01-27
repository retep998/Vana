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

#include "Configuration.h"
#include "Session.h"
#include "SessionManager.h"
#include "Types.h"
#include <string>
#include <boost/asio.hpp>
#include <memory>

class AbstractConnectionFactory;

class ConnectionAcceptor {
public:
	ConnectionAcceptor(boost::asio::io_service &ioService, const boost::asio::ip::tcp::endpoint &endpoint, AbstractConnectionFactory *apf, const LoginConfig &loginConfig, bool isServer, const string_t &patchLocation);
	auto stop() -> void;
private:
	auto startAccepting() -> void;
	auto handleConnection(ref_ptr_t<Session> newSession, const boost::system::error_code &error) -> void;

	bool m_isServer = true;
	string_t m_patchLocation;
	LoginConfig m_loginConfig;
	boost::asio::ip::tcp::acceptor m_acceptor;
	owned_ptr_t<AbstractConnectionFactory> m_apf;
	ref_ptr_t<SessionManager> m_sessionManager;
};