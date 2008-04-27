#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"
#include "PlayersPacket.h"


void PlayersPacket::showMoving(Player* player, vector <Player*> players, unsigned char* packett, int size){
	Packet packet = Packet();
	packet.addHeader(0x84);
	packet.addInt(player->getPlayerid());
	packet.addInt(0);
	packet.addBytesHex(packett+5, size);
	packet.sendTo(player, players, 0);
}	

void PlayersPacket::faceExperiment(Player* player, vector <Player*> players, int face){
	Packet packet = Packet();
	packet.addHeader(0x8c);
	packet.addInt(player->getPlayerid());
	packet.addInt(face);
	packet.sendTo(player, players, 0);
}

void PlayersPacket::showChat(Player* player, vector <Player*> players, char* msg){
	Packet packet = Packet();
	packet.addHeader(0x71);
	packet.addInt(player->getPlayerid());
	packet.addByte(0);
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	packet.sendTo(player, players, 1);
}

void PlayersPacket::damagePlayer(Player* player, vector <Player*> players, int dmg, int mob){
	Packet packet = Packet();
	packet.addHeader(0x89);
	packet.addInt(player->getPlayerid());
	packet.addByte(-1);
	packet.addInt(dmg);
	packet.addInt(mob);
	packet.addShort(1);
	packet.addByte(0);
	packet.addInt(dmg);
	packet.sendTo(player, players, 1);
}

void PlayersPacket::showMassage(char* msg, char type){
	Packet packet = Packet();
	packet.addHeader(0x2D); 
	packet.addByte(type);
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	for (hash_map<int,Player*>::iterator iter = Players::players.begin();
		 iter != Players::players.end(); iter++){
			 packet.packetSend(iter->second);
	}
}

void PlayersPacket::showInfo(Player* player, Player* getinfo){
	Packet packet = Packet();
	packet.addHeader(0x2A);
	packet.addInt(getinfo->getPlayerid());
	packet.addByte(getinfo->getLevel());
	packet.addShort(getinfo->getJob());
	packet.addShort(getinfo->getFame());
	packet.addByte(0); // Married
	packet.addShort(1); // Guild Name Len
	packet.addByte(0x2D); // Guild Name
	packet.addShort(0);
	packet.addByte(0);
	packet.packetSend(player);
}

void PlayersPacket::whisperPlayer(Player* player, Player* target, char* chat){
	Packet packet = Packet();
	packet.addHeader(0x5E);
	packet.addByte(0x12);
	packet.addShort(strlen(player->getName()));
	packet.addString(player->getName(),strlen(player->getName()));
	packet.addShort(0);//channel maybe
	packet.addShort(strlen(chat));
	packet.addString(chat,strlen(chat));
	packet.packetSend(target);
}

void PlayersPacket::findPlayer(Player* player, char* name, int map, unsigned char is){
	Packet packet = Packet();
	packet.addHeader(0x5E);
	if(map != -1){
		packet.addByte(0x09);
		packet.addShort(strlen(name));
		packet.addString(name,strlen(name));
		packet.addByte(0x1);
		packet.addInt(map);
		packet.addInt(0);
		packet.addInt(0);
	}
	else {	
		packet.addByte(0x0A);
		packet.addShort(strlen(name));
		packet.addString(name,strlen(name));
		packet.addByte(is);
	}

	packet.packetSend(player);
}