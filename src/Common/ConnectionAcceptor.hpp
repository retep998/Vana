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

#include "Configuration.hpp"
#include "Session.hpp"
#include "SessionManager.hpp"
#include "Types.hpp"
#include <string>
#include <boost/asio.hpp>
#include <memory>

class ConnectionAcceptor {
public:
	ConnectionAcceptor(boost::asio::io_service &ioService, const boost::asio::ip::tcp::endpoint &endpoint, function_t<AbstractConnection *()> createConnection, const InterServerConfig &config, bool isServer, const string_t &subversion);
	auto stop() -> void;
private:
	auto startAccepting() -> void;

	bool m_isServer = true;
	string_t m_subversion;
	InterServerConfig m_config;
	boost::asio::ip::tcp::acceptor m_acceptor;
	function_t<AbstractConnection *()> m_connectionCreator;
	ref_ptr_t<SessionManager> m_sessionManager;
};