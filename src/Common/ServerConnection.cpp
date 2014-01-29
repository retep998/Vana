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
#include "ServerConnection.hpp"
#include "AbstractServer.hpp"
#include "AuthenticationPacket.hpp"
#include "InterHeader.hpp"
#include "PacketReader.hpp"
#include "Session.hpp"
#include <boost/asio.hpp>
#include <iostream>

auto AbstractServerConnection::sendAuth(const string_t &pass, const IpMatrix &extIp) -> void {
	AuthenticationPacket::sendPassword(this, pass, extIp);
}

auto AbstractServerAcceptConnection::processAuth(AbstractServer &server, PacketReader &packet) -> Result {
	if (packet.getHeader() == IMSG_PASSWORD) {
		if (packet.getString() == server.getInterPassword()) {
			m_isAuthenticated = true;

			setExternalIpInformation(getIp(), packet.getClassVector<ExternalIp>());

			ServerType type = static_cast<ServerType>(packet.get<int8_t>());
			setType(type);
			authenticated(type);
		}
		else {
			server.log(LogType::ServerAuthFailure, [&](out_stream_t &log) { log << "IP: " << getSession()->getIp(); });

			getSession()->disconnect();
			return Result::Failure;
		}
	}
	else if (!m_isAuthenticated) {
		getSession()->disconnect();
		return Result::Failure;
	}
	packet.reset();
	return Result::Successful;
}