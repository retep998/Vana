/*
Copyright (C) 2008-2011 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.d

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "PingPacket.h"
#include "AbstractConnection.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "RecvHeader.h"
#include "SendHeader.h"

void PingPacket::ping(AbstractConnection *player) {
	PacketCreator packet;
	packet.addHeader(SMSG_PING);
	player->getSession()->send(packet);
}

void PingPacket::pong(AbstractConnection *player) {
	PacketCreator packet;
	packet.addHeader(CMSG_PONG);
	player->getSession()->send(packet);
}
