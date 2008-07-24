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
#include "FamePacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"
#include "SendHeader.h"

void FamePacket::sendError(Player *player, int reason) {
	PacketCreator packet;
	packet.addShort(SEND_FAME);
	packet.addInt(reason);
	packet.send(player);
}

void FamePacket::sendFame(Player *player, Player *player2, int type, int newFame) {
	PacketCreator packet;
	packet.addShort(SEND_FAME);
	packet.addByte(0x05);
	packet.addString(player->getName());
	packet.addByte(type);
	packet.send(player2);

	packet = PacketCreator();
	packet.addShort(SEND_FAME);
	packet.addByte(0x00);
	packet.addString(player2->getName());
	packet.addByte(type);
	packet.addInt(newFame);
	packet.send(player);
}
