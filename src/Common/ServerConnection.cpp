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
#include "ServerConnection.h"
#include "AbstractServer.h"
#include "AuthenticationPacket.h"
#include "InterHeader.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketReader.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

void AbstractServerConnection::sendAuth(const string &pass, IpMatrix &extIp) {
	AuthenticationPacket::sendPassword(this, pass, extIp);
}

bool AbstractServerAcceptConnection::processAuth(AbstractServer *server, PacketReader &packet, const string &pass) {
	if (packet.getHeader() == IMSG_PASSWORD) {
		if (packet.getString() == pass) {
			m_isAuthenticated = true;

			IpUtilities::extractExternalIp(packet, m_externalIp);

			int8_t type = packet.get<int8_t>();
			setType(type);
			authenticated(type);
		}
		else {
			server->log(LogTypes::ServerAuthFailure, "IP: " + IpUtilities::ipToString(getSession()->getIp()));
			getSession()->disconnect();
			return false;
		}
	}
	else if (!m_isAuthenticated) {
		// Trying to do something while unauthenticated? DC!
		getSession()->disconnect();
		return false;
	}
	packet.reset();
	return true;
}
