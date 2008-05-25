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
#include "ServerPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"
#include "SendHeader.h"

void ServerPacket::showScrollingHeader(Player* player, const char *msg){
	Packet packet;
	packet.addHeader(SEND_NOTICE);
	packet.addByte(4);
	packet.addByte(1);
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	packet.packetSend(player);
}
void ServerPacket::changeScrollingHeader(const char *msg){
	Packet packet;
	packet.addHeader(SEND_NOTICE);
	packet.addByte(4);
	packet.addByte(1);
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	for (hash_map<int,Player*>::iterator iter = Players::players.begin();
		 iter != Players::players.end(); iter++){
			 packet.packetSend(iter->second);
	}
}

void ServerPacket::scrollingHeaderOff(){
	Packet packet;
	packet.addHeader(SEND_NOTICE);
	packet.addByte(4);
	packet.addByte(0);
	for (hash_map<int,Player*>::iterator iter = Players::players.begin();
		 iter != Players::players.end(); iter++){
			 packet.packetSend(iter->second);
	}
}
