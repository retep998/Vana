#include "QuestsPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Quests.h"

void QuestsPacket::acceptQuest(Player* player, short questid, int npcid){
	Packet packet = Packet();
	packet.addHeader(0x32);
	packet.addByte(1);
	packet.addShort(questid);
	packet.addByte(1);
	packet.addInt(0);
	packet.addInt(0);
	packet.addShort(0);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x6C);
	packet.addByte(6);
	packet.addShort(questid);
	packet.addInt(npcid);
	packet.addInt(0);
	packet.packetSend(player);
}

void QuestsPacket::updateQuest(Player* player, Quest quest){
	Packet packet = Packet();
	packet.addHeader(0x32);
	packet.addByte(1);
	packet.addShort(quest.id);
	packet.addByte(1);
	char info[50];
	strcpy_s(info, 50, "");
	for(unsigned int i=0; i<quest.mobs.size(); i++){
		char temp[4];
		temp[0] = quest.mobs[i].count/100+'0';
		temp[1] = quest.mobs[i].count/10%10+'0';
		temp[2] = quest.mobs[i].count%10+'0';
		temp[3] = '\0';
		strcat_s(info, 50, temp);
	}
	packet.addShort(strlen(info));
	packet.addString(info, strlen(info));
	packet.addInt(0);
	packet.addInt(0);
	packet.packetSend(player);
}

void QuestsPacket::doneQuest(Player* player, int questid){
	Packet packet = Packet();
	packet.addHeader(0x1F);
	packet.addShort(questid);
	packet.packetSend(player);
}

void QuestsPacket::questFinish(Player* player, vector <Player*> players,short questid, int npcid, short nextquest, __int64 time){
	Packet packet = Packet();
	packet.addHeader(0x32);
	packet.addByte(1);
	packet.addShort(questid);
	packet.addByte(2);
	packet.addInt64(time);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x6C);
	packet.addByte(6);
	packet.addShort(questid); 
	packet.addInt(npcid); 
	packet.addShort(nextquest); 
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x67);
	packet.addByte(9);
	packet.packetSend(player);
	packet = Packet();
	packet.addHeader(0x85);
	packet.addInt(player->getPlayerid());
	packet.addByte(9);
	packet.sendTo(player, players, 0);
}

void QuestsPacket::giveItem(Player* player, int itemid, int amount){
	Packet packet = Packet();
	packet.addHeader(0x67); 
	packet.addByte(3);
	packet.addByte(1);
	packet.addInt(itemid);
	packet.addInt(amount);
	packet.packetSend(player);
}

void QuestsPacket::giveMesos(Player* player, int amount){
	Packet packet = Packet();
	packet.addHeader(0x32);
	packet.addByte(5);
	packet.addInt(amount);
	packet.packetSend(player);
}