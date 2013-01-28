/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "PingPacket.h"
#include "AbstractConnection.h"
#include "CmsgHeader.h"
#include "PacketCreator.h"
#include "Session.h"
#include "SmsgHeader.h"

void PingPacket::ping(AbstractConnection *connection) {
	PacketCreator packet;
	packet.add<header_t>(SMSG_PING);
	connection->getSession()->send(packet);
}

void PingPacket::pong(AbstractConnection *connection) {
	PacketCreator packet;
	packet.add<header_t>(CMSG_PONG);
	connection->getSession()->send(packet);
}