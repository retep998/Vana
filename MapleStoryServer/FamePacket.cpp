#include "FamePacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"

void FamePacket::SendError(Player* player, int reason){
	Packet packet = Packet();
	packet.addHeader(0x31);
	packet.addInt(reason);
	packet.packetSend(player);
}

void FamePacket::SendFame(Player* player, Player* player2, char* cFamer, int charLen, int FameDefame, int NewFame){
	Packet packet = Packet();
	packet.addHeader(0x31);
	packet.addByte(0x05);
	packet.addShort(strlen(player->getName()));
	packet.addString(player->getName(),strlen(player->getName()));
	packet.addByte(FameDefame);
	packet.packetSend(player2);

	packet = Packet();
	packet.addHeader(0x31);
	packet.addByte(0x00);
	packet.addShort(strlen(player2->getName()));
	packet.addString(player2->getName(),strlen(player2->getName()));
	packet.addByte(FameDefame);
	packet.addInt(NewFame);
	packet.packetSend(player);
}

void FamePacket::UpdateFame(Player* player) {
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addShort(0);
	packet.addShort(2);
	packet.addShort(player->getFame());
	packet.packetSend(player);
}