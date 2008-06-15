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

void FamePacket::SendError(Player *player, int reason) {
	Packet packet;
	packet.addHeader(SEND_FAME);
	packet.addInt(reason);
	packet.send(player);
}

void FamePacket::SendFame(Player *player, Player *player2, char *cFamer, int charLen, int FameDefame, int NewFame) {
	Packet packet;
	packet.addHeader(SEND_FAME);
	packet.addByte(0x05);
	packet.addShort(strlen(player->getName()));
	packet.addString(player->getName(),strlen(player->getName()));
	packet.addByte(FameDefame);
	packet.send(player2);

	packet = Packet();
	packet.addHeader(SEND_FAME);
	packet.addByte(0x00);
	packet.addShort(strlen(player2->getName()));
	packet.addString(player2->getName(),strlen(player2->getName()));
	packet.addByte(FameDefame);
	packet.addInt(NewFame);
	packet.send(player);
}
