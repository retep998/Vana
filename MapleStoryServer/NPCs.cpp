#include "NPCs.h"
#include "Player.h"
#include "NPCPacket.h"
#include "InventoryPacket.h"
#include "PacketCreator.h"
#include "NPCsScripts.h"
#include "Maps.h"
#include "Shops.h"
#include "Inventory.h"
#include "Quests.h"
#include "Levels.h"
#include "BufferUtilities.h"

hash_map <int, NPCsInfo> NPCs::info;



void NPCs::showNPCs(Player* player){
	for(unsigned int i=0; i<info[player->getMap()].size(); i++)
		NPCPacket::showNPC(player, info[player->getMap()][i], i);
}

void NPCs::handleNPC(Player* player, unsigned char* packet){
	if(player->getNPC() != NULL)
		return;
	int npcid = info[player->getMap()][getInt(packet)-100].id;
	NPC* npc = new NPC(npcid, player);
	NPCsScripts::handle(npcid, npc);
	if(npc->isEnd())
		delete npc;
}

void NPCs::handleQuestNPC(Player* player, int npcid, bool start){
	if(player->getNPC() != NULL)
		return;
	NPC* npc = new NPC(npcid, player, 1);
	npc->setIsStart(start);
	QuestsScripts::handle(npcid, npc, start);
	if(npc->isEnd())
		delete npc;
}
void NPCs::handleNPCIn(Player* player, unsigned char* packet){
	NPC* npc = player->getNPC();
	if(npc == NULL)
		return;
	char type = packet[0];
	if(type == 0){
		char what = packet[1];
		if(what == 0)
			npc->setState(npc->getState()-1);
		else if(what == 1)
			npc->setState(npc->getState()+1);
		else if(what == -1)
			npc->end();
	}
	else if(type == 1 || type == 2){
		npc->setState(npc->getState()+1);
		char what = packet[1];
		if(what == 0)
			npc->setSelected(0);
		else if(what == 1)
			npc->setSelected(1);
		else if(what == -1)
			npc->end();
	}
	else if(type == 3){
		npc->setState(npc->getState()+1);
		if(packet[1] != 0){
			short len = getShort(packet+2);
			if(len>100){
				npc->end();
				return;
			}
			char temp[101];
			getString(packet+4, len, temp);
			npc->setGetText(temp);
		}
		else
			npc->end();
	}
	else if(type == 4){
		npc->setState(npc->getState()+1);
		if(packet[1] == 1)
			npc->setGetNumber(getInt(packet+2));
		else
			npc->end();
	}
	else if(type == 5){
		npc->setState(npc->getState()+1);
		if(packet[1] == 0)
			npc->end();
		else
			npc->setSelected(packet[2]);
	}
	else if(type == 7){
		npc->setState(npc->getState()+1);
		if(packet[1] == 1){
			npc->setSelected(getShort(packet+2));
		}
		else 
			npc->end();
	}
	else
		npc->end();
	if(npc->isEnd()){
		delete npc;
		return;
	}
	NPCsScripts::handle(npc->getNpcID(), npc);
	if(npc->isEnd()){
		delete npc;
		return;
	}
}

NPC::NPC(int npcid, Player* player, bool isquest){
	this->isquest = isquest;
	getnum=0;
	gettext[0] = '\0';
	state=0;
	selected=-1;
	cend = false;
	this->npcid = npcid;
	this->player = player;
	strcpy_s(text, 2, "");
	player->setNPC(this);
}

NPC::~NPC(){
	player->setNPC(NULL);
}

Packet NPC::npcPacket(char type){
	Packet packet = Packet();
	packet.addHeader(0xC2);
	packet.addByte(4);
	packet.addInt(npcid);
	packet.addByte(type);
	packet.addShort(strlen(text));
	packet.addString(text, strlen(text));
	this->text[0] = '\0';
	return packet;
}

void NPC::sendSimple(){
	Packet packet = npcPacket(5);
	packet.packetSend(player);
}
void NPC::sendYesNo(){
	Packet packet = npcPacket(1);
	packet.packetSend(player);
}
void NPC::sendNext(){
	Packet packet = npcPacket(0);
	packet.addByte(0);
	packet.addByte(1);
	packet.packetSend(player);
}
void NPC::sendBackNext(){
	Packet packet = npcPacket(0);
	packet.addByte(1);
	packet.addByte(1);
	packet.packetSend(player);
}
void NPC::sendBackOK(){
	Packet packet = npcPacket(0);
	packet.addByte(1);
	packet.addByte(0);
	packet.packetSend(player);
}
void NPC::sendOK(){
	Packet packet = npcPacket(0);
	packet.addShort(0);
	packet.packetSend(player);
}

void NPC::sendAcceptDecline(){
	Packet packet = npcPacket(2);
	packet.packetSend(player);
}

void NPC::sendGetText(){
	Packet packet = npcPacket(3);
	packet.addInt(0);
	packet.addInt(0);
	packet.packetSend(player);
}

void NPC::sendGetNumber(int def, int min, int max){
	Packet packet = npcPacket(4);
	packet.addInt(def);
	packet.addInt(min);
	packet.addInt(max);
	packet.addInt(0);
	packet.packetSend(player);
}

void NPC::sendStyle(int styles[], char size){
	Packet packet = npcPacket(7);
	packet.addByte(size);
	for(int i=0; i<size; i++)
		packet.addInt(styles[i]);
	packet.packetSend(player);
}

void NPC::teleport(int mapid){
	if(Maps::info.find(mapid) != Maps::info.end())
		Maps::changeMap(player, mapid, 0);
}
int NPC::getItemNum(int itemid){
	return player->inv->getItemAmount(itemid);
}
int NPC::getMesos(){
	return player->inv->getMesos();
}

void NPC::giveItem(int itemid, short amount){
	Quests::giveItem(player, itemid, amount);
}

void NPC::giveMesos(int amount){
	Quests::giveMesos(player, amount);
}
int NPC::getPlayerMap(){
	return player->getMap();
}
void NPC::showShop(){
	Shops::showShop(player, getNpcID());
}

void NPC::setPlayerHP(short hp){
	player->setHP(hp);
}
short NPC::getPlayerHP(){
	return player->getHP();
}
int NPC::getPlayerHair(){
	return player->getHair();
}
int NPC::getPlayerEyes(){
	return player->getEyes();
}

void NPC::addQuest(short questid){
	player->quests->addQuest(questid, npcid);
}

void NPC::endQuest(short questid){
	player->quests->finishQuest(questid, npcid);
}

void NPC::giveEXP(int exp){
	Levels::giveEXP(player, exp, 1);	
}

int NPC::getLevel(){
	return player->getLevel();	
}

void NPC::setStyle(int id){
	if(id/10000 == 0){
		player->setSkin((char)id);
	}
	else if(id/10000 == 2){
		player->setEyes(id);
	}
	else if(id/10000 == 3){
		player->setHair(id);
	}
	InventoryPacket::updatePlayer(player);
}