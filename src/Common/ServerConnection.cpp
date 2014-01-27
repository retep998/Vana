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
#include "MiscUtilities.hpp"
#include "PacketReader.hpp"
#include "Session.hpp"
#include <boost/asio.hpp>
#include <iostream>

auto AbstractServerConnection::sendAuth(const string_t &pass, const string_t &salt, const IpMatrix &extIp) -> void {
	AuthenticationPacket::sendPassword(this, MiscUtilities::hashPassword(pass, salt), extIp);
}

auto AbstractServerAcceptConnection::processAuth(AbstractServer &server, PacketReader &packet) -> bool {
	if (packet.getHeader() == IMSG_PASSWORD) {
		string_t pass = MiscUtilities::hashPassword(server.getInterPassword(), server.getSalt());
		if (packet.getString() == pass) {
			m_isAuthenticated = true;

			setExternalIpInformation(getIp(), packet.getClassVector<ExternalIp>());

			int8_t type = packet.get<int8_t>();
			setType(type);
			authenticated(type);
		}
		else {
			server.log(LogTypes::ServerAuthFailure, "IP: " + getSession()->getIp().toString());

			getSession()->disconnect();
			return false;
		}
	}
	else if (!m_isAuthenticated) {
		getSession()->disconnect();
		return false;
	}
	packet.reset();
	return true;
}