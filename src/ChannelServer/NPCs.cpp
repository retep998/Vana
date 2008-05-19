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
#include "SendHeader.h"

hash_map <int, NPCsInfo> NPCs::info;

void NPCs::addNPC(int id, NPCsInfo npc){
	info[id] = npc;
}

void NPCs::showNPCs(Player* player){
	for(unsigned int i=0; i<info[player->getMap()].size(); i++)
		NPCPacket::showNPC(player, info[player->getMap()][i], i);
}

void NPCs::handleNPC(Player* player, unsigned char* packet){
	if(player->getNPC() != NULL)
		return;
	int npcid = info[player->getMap()][BufferUtilities::getInt(packet)-100].id;
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
			short len = BufferUtilities::getShort(packet+2);
			if(len>100){
				npc->end();
				return;
			}
			char temp[101];
			BufferUtilities::getString(packet+4, len, temp);
			npc->setGetText(temp);
		}
		else
			npc->end();
	}
	else if(type == 4){
		npc->setState(npc->getState()+1);
		if(packet[1] == 1)
			npc->setGetNumber(BufferUtilities::getInt(packet+2));
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
			npc->setSelected(BufferUtilities::getShort(packet+2));
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
	packet.addHeader(SEND_NPC_TALK);
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

void NPC::showShop(){
	Shops::showShop(player, getNpcID());
}

/* TO BE REMOVED */
void NPC::teleport(int mapid){
	if(Maps::info.find(mapid) != Maps::info.end())
		Maps::changeMap(player, mapid, 0);
}
/* TO BE REMOVED */
int NPC::getItemNum(int itemid){
	return player->inv->getItemAmount(itemid);
}
/* TO BE REMOVED */
int NPC::getMesos(){
	return player->inv->getMesos();
}
/* TO BE REMOVED */
void NPC::giveItem(int itemid, short amount){
	Quests::giveItem(player, itemid, amount);
}
/* TO BE REMOVED */
void NPC::giveMesos(int amount){
	Quests::giveMesos(player, amount);
}
/* TO BE REMOVED */
int NPC::getPlayerMap(){
	return player->getMap();
}
/* TO BE REMOVED */
void NPC::setPlayerHP(short hp){
	player->setHP(hp);
}
/* TO BE REMOVED */
short NPC::getPlayerHP(){
	return player->getHP();
}
/* TO BE REMOVED */
int NPC::getPlayerHair(){
	return player->getHair();
}
/* TO BE REMOVED */
int NPC::getPlayerEyes(){
	return player->getEyes();
}
/* TO BE REMOVED */
void NPC::addQuest(short questid){
	player->quests->addQuest(questid, npcid);
}
/* TO BE REMOVED */
void NPC::endQuest(short questid){
	player->quests->finishQuest(questid, npcid);
}
/* TO BE REMOVED */
void NPC::giveEXP(int exp){
	Levels::giveEXP(player, exp, 1);	
}
/* TO BE REMOVED */
int NPC::getLevel(){
	return player->getLevel();	
}
/* TO BE REMOVED */
short NPC::getGender(){
	return player->getGender();
}
/* TO BE REMOVED */
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
