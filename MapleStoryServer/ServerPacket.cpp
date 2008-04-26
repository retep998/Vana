#include "ServerPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"

void ServerPacket::showScrollingHeader(Player* player, char* msg){
	Packet packet = Packet();
	packet.addHeader(0x2D);
	packet.addByte(4);
	packet.addByte(1);
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	packet.packetSend(player);
}
void ServerPacket::changeScrollingHeader(char* msg){
	Packet packet = Packet();
	packet.addHeader(0x2D);
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
	Packet packet = Packet();
	packet.addHeader(0x2D);
	packet.addByte(4);
	packet.addByte(0);
	for (hash_map<int,Player*>::iterator iter = Players::players.begin();
		 iter != Players::players.end(); iter++){
			 packet.packetSend(iter->second);
	}
}