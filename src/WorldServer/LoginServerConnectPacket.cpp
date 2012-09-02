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
#include "LoginServerConnectPacket.h"
#include "InterHeader.h"
#include "IpUtilities.h"
#include "LoginServerConnection.h"
#include "PacketCreator.h"
#include "Session.h"
#include "WorldServer.h"

void LoginServerConnectPacket::registerChannel(int32_t channel, ip_t ip, const IpMatrix &extIp, port_t port) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_REGISTER_CHANNEL);
	packet.add<int32_t>(channel);
	packet.add<ip_t>(ip);

	packet.add<uint32_t>(extIp.size());
	std::for_each(extIp.begin(), extIp.end(), IpUtilities::SendIpArray(packet));

	packet.add<port_t>(port);
	WorldServer::Instance()->getLoginConnection()->getSession()->send(packet);
}

void LoginServerConnectPacket::updateChannelPop(int32_t channel, int32_t population) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_UPDATE_CHANNEL_POP);
	packet.add<int32_t>(channel);
	packet.add<int32_t>(population);

	WorldServer::Instance()->getLoginConnection()->getSession()->send(packet);
}

void LoginServerConnectPacket::removeChannel(int32_t channel) {
	PacketCreator packet;
	packet.add<header_t>(IMSG_REMOVE_CHANNEL);
	packet.add<int32_t>(channel);

	WorldServer::Instance()->getLoginConnection()->getSession()->send(packet);
}