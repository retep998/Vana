/*
Copyright (C) 2008 Vana Development Team

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
#include "ServerPlayer.h"
#include "InterHeader.h"
#include "PacketCreator.h"

void AuthenticationPacket::sendPassword(AbstractServerConnectPlayer *player, const char *pass, const char *ip) {
	Packet packet;
	packet.addHeader(INTER_PASSWORD);
	packet.addShort(strlen(pass));
	packet.addString(pass, strlen(pass));
	packet.addByte(player->getType());
	packet.addShort(strlen(ip));
	packet.addString(ip, strlen(ip));
	packet.packetSend(player);
}