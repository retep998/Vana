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
#include "LuaNPC.h"
#include "Player.h"
#include "NPCPacket.h"
#include "InventoryPacket.h"
#include "PacketCreator.h"
#include "Maps.h"
#include "Shops.h"
#include "Inventory.h"
#include "Quests.h"
#include "Levels.h"
#include "SendHeader.h"
#include "ReadPacket.h"
#include <sys/stat.h>

void NPCs::handleNPC(Player *player, ReadPacket *packet) {
	if (player->getNPC() != 0) {
		return;
	}

	int npcid = Maps::maps[player->getMap()]->getNpc(packet->getInt()-100).id;
	if (Shops::shops.find(npcid) != Shops::shops.end()) { // Shop
		Shops::showShop(player, npcid);
		return;
	}

	NPC *npc = new NPC(npcid, player);
	npc->run();
}

void NPCs::handleQuestNPC(Player *player, int npcid, bool start) {
	if (player->getNPC() != 0) {
		return;
	}

	NPC *npc = new NPC(npcid, player, true, start);
	npc->run();
}

void NPCs::handleNPCIn(Player *player, ReadPacket *packet) {
	NPC *npc = player->getNPC();
	if (npc == 0) {
		return;
	}

	char type = packet->getByte();
	char what = packet->getByte();

	if (type == NPCDialogs::normal) {
		switch (what) {
			case 0: npc->setState(npc->getState() - 1); break;
			case 1:	npc->setState(npc->getState() + 1); break;
			default: npc->end(); break;
		}
	}
	else if (type == NPCDialogs::yesNo || type == NPCDialogs::acceptDecline) {
		npc->setState(npc->getState() + 1);
		switch (what) {
			case 0: npc->setSelected(0); break;
			case 1:	npc->setSelected(1); break;
			default: npc->end(); break;
		}
	}
	else if (type == NPCDialogs::getText) {
		npc->setState(npc->getState()+1);
		if (what != 0) {
			npc->setGetText(packet->getString());
		}
		else {
			npc->end();
		}
	}
	else if (type == NPCDialogs::getNumber) {
		npc->setState(npc->getState()+1);
		if (what == 1) {
			npc->setGetNumber(packet->getInt());
		}
		else {
			npc->end();
		}
	}
	else if (type == NPCDialogs::simple) {
		npc->setState(npc->getState()+1);
		if (what == 0) {
			npc->end();
		}
		else {
			npc->setSelected(packet->getByte());
		}
	}
	else if (type == NPCDialogs::style) {
		npc->setState(npc->getState()+1);
		if (what == 1) {
			npc->setSelected(packet->getShort());
		}
		else  {
			npc->end();
		}
	}
	else {
		npc->end();
	}

	npc->run();
}

NPC::NPC(int npcid, Player *player, bool isquest, bool isstart) :
npcid(npcid),
player(player),
text(""),
state(0),
cend(false)
{
	struct stat fileinfo;
	std::ostringstream filenameStream;
	filenameStream << "scripts/npcs/" << npcid;
	if (isquest) {
		if (isstart)
			filenameStream << "s";
		else
			filenameStream << "e";
	}
	filenameStream << ".lua";
	string filename = filenameStream.str();

	if (!stat(filename.c_str(), &fileinfo)) { // Lua NPC exists
		luaNPC.reset(new LuaNPC(filename, player->getPlayerid()));
		player->setNPC(this);
		setState(state);
	}
	else {
		end();
	}
}

NPC::~NPC() {
	player->setNPC(0);
}

bool NPC::checkEnd() {
	if (isEnd()) {
		delete this;
		return true;
	}

	return false;
}

void NPC::run() {
	if (checkEnd()) { //  NPC Ended
		return;
	}

	luaNPC->run();	
	checkEnd();
}

Packet & NPC::npcPacket(char type) {
	Packet packet;
	packet.addHeader(SEND_NPC_TALK);
	packet.addByte(4);
	packet.addInt(npcid);
	packet.addByte(type);
	packet.addString(text);
	
	text = "";

	Packet &ret = packet;
	return ret;
}

void NPC::sendSimple() {
	Packet packet = npcPacket(NPCDialogs::simple);
	packet.send(player);
}

void NPC::sendYesNo() {
	Packet packet = npcPacket(NPCDialogs::yesNo);
	packet.send(player);
}

void NPC::sendNext() {
	Packet packet = npcPacket(NPCDialogs::normal);
	packet.addByte(0);
	packet.addByte(1);
	packet.send(player);
}

void NPC::sendBackNext() {
	Packet packet = npcPacket(NPCDialogs::normal);
	packet.addByte(1);
	packet.addByte(1);
	packet.send(player);
}

void NPC::sendBackOK() {
	Packet packet = npcPacket(NPCDialogs::normal);
	packet.addByte(1);
	packet.addByte(0);
	packet.send(player);
}

void NPC::sendOK() {
	Packet packet = npcPacket(NPCDialogs::normal);
	packet.addShort(0);
	packet.send(player);
}

void NPC::sendAcceptDecline() {
	Packet packet = npcPacket(NPCDialogs::acceptDecline);
	packet.send(player);
}

void NPC::sendGetText() {
	Packet packet = npcPacket(NPCDialogs::getText);
	packet.addInt(0);
	packet.addInt(0);
	packet.send(player);
}

void NPC::sendGetNumber(int def, int min, int max) {
	Packet packet = npcPacket(NPCDialogs::getNumber);
	packet.addInt(def);
	packet.addInt(min);
	packet.addInt(max);
	packet.addInt(0);
	packet.send(player);
}

void NPC::sendStyle(int styles[], char size) {
	Packet packet = npcPacket(NPCDialogs::style);
	packet.addByte(size);
	for (int i = 0; i < size; i++)
		packet.addInt(styles[i]);
	packet.send(player);
}

void NPC::setState(int state) {
	this->state = state;
	luaNPC->setVariable("state", state);
}