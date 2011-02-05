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
#include "AuthenticationPacket.h"
#include "InterHeader.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "ServerConnection.h"
#include <algorithm>

void AuthenticationPacket::sendPassword(AbstractServerConnection *player, const string &pass, IpMatrix &extIp) {
	PacketCreator packet;
	packet.addHeader(IMSG_PASSWORD);
	packet.addString(pass);

	packet.add<uint32_t>(extIp.size());
	std::for_each(extIp.begin(), extIp.end(), IpUtilities::SendIpArray(packet));

	packet.add<int8_t>(player->getType());
	player->getSession()->send(packet);
}
