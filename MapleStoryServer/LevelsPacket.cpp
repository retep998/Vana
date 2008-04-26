#include "LevelsPacket.h"
#include "PacketCreator.h"
#include "Player.h"

void LevelsPacket::showEXP(Player* player, int exp, char type){
	Packet packet = Packet();
	packet.addHeader(0x32);
	packet.addByte(3);
	packet.addByte(1);
	packet.addInt(exp);
	packet.addInt(type);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
	packet.packetSend(player);
}

void LevelsPacket::levelUP(Player* player, vector <Player*> players){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addShort(0x7C10);
	packet.addShort(1);
	packet.addByte(player->getLevel());
	packet.addShort(player->getHP());
	packet.addShort(player->getRMHP());
	packet.addShort(player->getMP());
	packet.addShort(player->getRMMP());
	packet.addShort(player->getAp());
	packet.addInt(player->getExp());
	packet.packetSend(player); // TODO
	packet = Packet();
	packet.addHeader(0x85);
	packet.addInt(player->getPlayerid());
	packet.addByte(0);
	packet.sendTo(player, players, 0);

}

void LevelsPacket::statOK(Player* player){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(1);
	packet.addInt(0);
	packet.packetSend(player);
}
void LevelsPacket::changeStat(Player* player, int nam, short val){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(1);
	packet.addInt(nam);
	packet.addShort(val);
	packet.addShort(player->getAp());
	packet.packetSend(player);
}

void LevelsPacket::jobChange(Player *player, std::vector<Player*> players){
	Packet packet = Packet();
	packet.addHeader(0x23);
	packet.addShort(0);
	packet.addInt(0x20);
	packet.addShort(player->getJob());
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x85);
	packet.addInt(player->getPlayerid());
	packet.addByte(8);
	packet.sendTo(player, players, 0);
}