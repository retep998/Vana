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
#include "ReadPacket.h"

void NPCs::addNPC(int id, NPCInfo npc) {
	Maps::maps[id]->addNPC(npc);
}

void NPCs::showNPCs(Player *player) {
	for (unsigned int i = 0; i < Maps::maps[player->getMap()]->getNpcs().size(); i++)
		NPCPacket::showNPC(player, Maps::maps[player->getMap()]->getNpc(i), i);
}

void NPCs::handleNPC(Player *player, ReadPacket *packet) {
	if (player->getNPC() != 0)
		return;
	int npcid = Maps::maps[player->getMap()]->getNpc(packet->getInt()-100).id;
	NPC *npc = new NPC(npcid, player);
	NPCsScripts::handle(npcid, npc);
	if (npc->isEnd())
		delete npc;
}

void NPCs::handleQuestNPC(Player *player, int npcid, bool start) {
	if (player->getNPC() != 0)
		return;
	NPC *npc = new NPC(npcid, player, 1);
	npc->setIsStart(start);
	NPCsScripts::handle(npcid, npc);
	if (npc->isEnd())
		delete npc;
}
void NPCs::handleNPCIn(Player *player, ReadPacket *packet) {
	NPC *npc = player->getNPC();
	if (npc == 0)
		return;
	char type = packet->getByte();
	char what = packet->getByte();
	if (type == 0) {
		if (what == 0)
			npc->setState(npc->getState()-1);
		else if (what == 1)
			npc->setState(npc->getState()+1);
		else if (what == -1)
			npc->end();
	}
	else if (type == 1 || type == 2) {
		npc->setState(npc->getState()+1);
		if (what == 0)
			npc->setSelected(0);
		else if (what == 1)
			npc->setSelected(1);
		else if (what == -1)
			npc->end();
	}
	else if (type == 3) {
		npc->setState(npc->getState()+1);
		if (what != 0) {
			npc->setGetText(packet->getString());
		}
		else
			npc->end();
	}
	else if (type == 4) {
		npc->setState(npc->getState()+1);
		if (what == 1)
			npc->setGetNumber(packet->getInt());
		else
			npc->end();
	}
	else if (type == 5) {
		npc->setState(npc->getState()+1);
		if (what == 0)
			npc->end();
		else
			npc->setSelected(packet->getByte());
	}
	else if (type == 7) {
		npc->setState(npc->getState()+1);
		if (what == 1) {
			npc->setSelected(packet->getShort());
		}
		else 
			npc->end();
	}
	else {
		npc->end();
	}
	if (npc->isEnd()) {
		delete npc;
		return;
	}
	NPCsScripts::handle(npc->getNpcID(), npc);
	if (npc->isEnd()) {
		delete npc;
		return;
	}
}

NPC::NPC(int npcid, Player *player, bool isquest) {
	this->isquest = isquest;
	getnum = 0;
	gettext[0] = '\0';
	state = 0;
	selected = -1;
	cend = false;
	this->npcid = npcid;
	this->player = player;
	strcpy_s(text, 2, "");
	player->setNPC(this);
}

NPC::~NPC() {
	player->setNPC(0);
}

Packet NPC::npcPacket(char type) {
	Packet packet;
	packet.addHeader(SEND_NPC_TALK);
	packet.addByte(4);
	packet.addInt(npcid);
	packet.addByte(type);
	packet.addShort(strlen(text));
	packet.addString(text, strlen(text));
	this->text[0] = '\0';
	return packet;
}

void NPC::sendSimple() {
	Packet packet = npcPacket(5);
	packet.send(player);
}
void NPC::sendYesNo() {
	Packet packet = npcPacket(1);
	packet.send(player);
}
void NPC::sendNext() {
	Packet packet = npcPacket(0);
	packet.addByte(0);
	packet.addByte(1);
	packet.send(player);
}
void NPC::sendBackNext() {
	Packet packet = npcPacket(0);
	packet.addByte(1);
	packet.addByte(1);
	packet.send(player);
}
void NPC::sendBackOK() {
	Packet packet = npcPacket(0);
	packet.addByte(1);
	packet.addByte(0);
	packet.send(player);
}
void NPC::sendOK() {
	Packet packet = npcPacket(0);
	packet.addShort(0);
	packet.send(player);
}

void NPC::sendAcceptDecline() {
	Packet packet = npcPacket(2);
	packet.send(player);
}

void NPC::sendGetText() {
	Packet packet = npcPacket(3);
	packet.addInt(0);
	packet.addInt(0);
	packet.send(player);
}

void NPC::sendGetNumber(int def, int min, int max) {
	Packet packet = npcPacket(4);
	packet.addInt(def);
	packet.addInt(min);
	packet.addInt(max);
	packet.addInt(0);
	packet.send(player);
}

void NPC::sendStyle(int styles[], char size) {
	Packet packet = npcPacket(7);
	packet.addByte(size);
	for (int i = 0; i < size; i++)
		packet.addInt(styles[i]);
	packet.send(player);
}

void NPC::showShop() {
	Shops::showShop(player, getNpcID());
}
