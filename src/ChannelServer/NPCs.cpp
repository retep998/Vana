/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "MapleSession.h"
#include "Maps.h"
#include "NPCPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Quests.h"
#include "PacketReader.h"
#include "SendHeader.h"
#include "ScriptDataProvider.h"
#include "ShopDataProvider.h"
#include <sys/stat.h>
#include <string>

using std::string;

void NPCs::handleNPC(Player *player, PacketReader &packet) {
	if (player->getNPC() != 0) {
		return;
	}

	int32_t npcid = packet.get<int32_t>() - 100;

	if (!Maps::getMap(player->getMap())->isValidNpcIndex(npcid)) {
		// Shouldn't ever happen except in edited packets
		return;
	}

	NPCSpawnInfo npcs = Maps::getMap(player->getMap())->getNpc(npcid);
	if (ShopDataProvider::Instance()->showShop(player, npcs.id)) // Shop
		return;

	NPC *npc = new NPC(npcs.id, player, npcs.pos);
	npc->run();
}

void NPCs::handleQuestNPC(Player *player, int32_t npcid, bool start, int16_t questid) {
	if (player->getNPC() != 0) {
		return;
	}

	NPC *npc = new NPC(npcid, player, questid, start);
	npc->run();
}

void NPCs::handleNPCIn(Player *player, PacketReader &packet) {
	NPC *npc = player->getNPC();
	if (npc == 0) {
		return;
	}

	int8_t type = packet.get<int8_t>();
	int8_t what = packet.get<int8_t>();

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
		npc->setState(npc->getState() + 1);
		if (what != 0) {
			npc->setGetText(packet.getString());
		}
		else {
			npc->end();
		}
	}
	else if (type == NPCDialogs::getNumber) {
		npc->setState(npc->getState() + 1);
		if (what == 1) {
			npc->setGetNumber(packet.get<int32_t>());
		}
		else {
			npc->end();
		}
	}
	else if (type == NPCDialogs::simple) {
		npc->setState(npc->getState() + 1);
		if (what == 0) {
			npc->end();
		}
		else {
			npc->setSelected(packet.get<uint8_t>());
		}
	}
	else if (type == NPCDialogs::style) {
		npc->setState(npc->getState() + 1);
		if (what == 1) {
			npc->setSelected(packet.get<uint8_t>());
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

void NPCs::handleNPCAnimation(Player *player, PacketReader &packet) {
	NPCPacket::animateNPC(player, packet);
}

NPC::NPC(int32_t npcid, Player *player, int16_t questid, bool isstart) :
player(player),
npcid(npcid),
questid(questid),
pos(Pos(0, 0)),
state(0),
text(""),
cend(false)
{
	initScript(npcid, player, questid, isstart);
}

NPC::NPC(int32_t npcid, Player *player, const Pos &pos, int16_t questid, bool isstart) :
player(player),
npcid(npcid),
questid(questid),
pos(pos),
state(0),
text(""),
cend(false)
{
	initScript(npcid, player, questid, isstart);
}

NPC::~NPC() {
	player->setNPC(0);
}

void NPC::initScript(int32_t npcid, Player *player, int16_t questid, bool isstart) {
	struct stat fileinfo;
	string filename;
	if (questid == 0) {
		filename = ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	else {
		filename = ScriptDataProvider::Instance()->getQuestScript(questid, (isstart ? 0 : 1));
	}
	if (!stat(filename.c_str(), &fileinfo)) { // Lua NPC exists
		luaNPC.reset(new LuaNPC(filename, player->getId(), questid));
		player->setNPC(this);
		setState(state);
	}
	else {
		end();
	}
}

bool NPC::checkEnd() {
	if (isEnd()) {
		delete this;
		return true;
	}

	return false;
}

bool NPC::run() {
	if (checkEnd()) { //  NPC Ended
		return false;
	}

	bool ret = luaNPC->run();

	if (checkEnd()) {
		return false;
	}

	return ret;
}

PacketCreator NPC::npcPacket(int8_t type) {
	PacketCreator packet;
	packet.add<int16_t>(SEND_NPC_TALK);
	packet.add<int8_t>(4);
	packet.add<int32_t>(npcid);
	packet.add<int8_t>(type);
	packet.addString(text);

	text = "";

	return packet;
}

void NPC::sendSimple() {
	PacketCreator packet = npcPacket(NPCDialogs::simple);

	player->getSession()->send(packet);
}

void NPC::sendYesNo() {
	PacketCreator packet = npcPacket(NPCDialogs::yesNo);
	player->getSession()->send(packet);
}

void NPC::sendNext() {
	PacketCreator packet = npcPacket(NPCDialogs::normal);
	packet.add<int8_t>(0);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void NPC::sendBackNext() {
	PacketCreator packet = npcPacket(NPCDialogs::normal);
	packet.add<int8_t>(1);
	packet.add<int8_t>(1);
	player->getSession()->send(packet);
}

void NPC::sendBackOK() {
	PacketCreator packet = npcPacket(NPCDialogs::normal);
	packet.add<int8_t>(1);
	packet.add<int8_t>(0);
	player->getSession()->send(packet);
}

void NPC::sendOK() {
	PacketCreator packet = npcPacket(NPCDialogs::normal);
	packet.add<int16_t>(0);
	player->getSession()->send(packet);
}

void NPC::sendAcceptDecline() {
	PacketCreator packet = npcPacket(NPCDialogs::acceptDecline);
	player->getSession()->send(packet);
}

void NPC::sendGetText(int16_t min, int16_t max) {
	PacketCreator packet = npcPacket(NPCDialogs::getText);
	packet.add<int32_t>(0);
	packet.add<int16_t>(max);
	packet.add<int16_t>(min);
	player->getSession()->send(packet);
}

void NPC::sendGetNumber(int32_t def, int32_t min, int32_t max) {
	PacketCreator packet = npcPacket(NPCDialogs::getNumber);
	packet.add<int32_t>(def);
	packet.add<int32_t>(min);
	packet.add<int32_t>(max);
	packet.add<int32_t>(0);
	player->getSession()->send(packet);
}

void NPC::sendStyle(int32_t styles[], int8_t size) {
	PacketCreator packet = npcPacket(NPCDialogs::style);
	packet.add<int8_t>(size);
	for (int8_t i = 0; i < size; i++)
		packet.add<int32_t>(styles[i]);
	player->getSession()->send(packet);
}

void NPC::setState(int32_t state) {
	this->state = state;
	luaNPC->setVariable("state", state);
}
