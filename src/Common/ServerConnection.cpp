/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "AuthenticationPacket.h"
#include "InterHeader.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketReader.h"
#include <iostream>

void AbstractServerConnection::sendAuth(const string &pass, IpMatrix &extIp) {
	AuthenticationPacket::sendPassword(this, pass, extIp);
}

bool AbstractServerAcceptConnection::processAuth(PacketReader &packet, const string &pass) {
	if (packet.get<int16_t>() == IMSG_PASSWORD) {
		if (packet.getString() == pass) {
			std::cout << "Server successfully authenticated." << std::endl;
			m_is_authenticated = true;

			IpUtilities::extractExternalIp(packet, m_external_ip);

			int8_t type = packet.get<int8_t>();
			setType(type);
			authenticated(type);
		}
		else {
			getSession()->disconnect();
			return false;
		}
	}
	else if (m_is_authenticated == false) {
		// Trying to do something while unauthenticated? DC!
		getSession()->disconnect();
		return false;
	}
	packet.reset();
	return true;
}
