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
#include "Inventory.h"
#include "FileUtilities.h"
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
	if (Inventory::showShop(player, npcs.id)) // Shop
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
	if (type != npc->getSentDialog()) {
		// hacking
		return;
	}

	int8_t what = packet.get<int8_t>();

	if (type == NPCDialogs::normal) {
		switch (what) {
			case 0: npc->proceedBack(); break;
			case 1:	npc->proceedNext(); break;
			default: npc->end(); break;
		}
	}
	else if (type == NPCDialogs::yesNo || type == NPCDialogs::acceptDecline) {
		switch (what) {
			case 0: npc->proceedSelection(0); break;
			case 1:	npc->proceedSelection(1); break;
			default: npc->end(); break;
		}
	}
	else if (type == NPCDialogs::getText) {
		if (what != 0) {
			npc->proceedText(packet.getString());
		}
		else {
			npc->end();
		}
	}
	else if (type == NPCDialogs::getNumber) {
		if (what == 1) {
			npc->proceedNumber(packet.get<int32_t>());
		}
		else {
			npc->end();
		}
	}
	else if (type == NPCDialogs::simple) {
		if (what == 0) {
			npc->end();
		}
		else {
			npc->proceedSelection(packet.get<uint8_t>());
		}
	}
	else if (type == NPCDialogs::style) {
		if (what == 1) {
			npc->proceedSelection(packet.get<uint8_t>());
		}
		else  {
			npc->end();
		}
	}
	else {
		npc->end();
	}
	npc->checkEnd();
}

void NPCs::handleNPCAnimation(Player *player, PacketReader &packet) {
	NPCPacket::animateNPC(player, packet);
}

NPC::NPC(int32_t npcid, Player *player, int16_t questid, bool isstart) :
player(player),
npcid(npcid),
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
	string filename;
	if (questid == 0) {
		filename = ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	else {
		filename = ScriptDataProvider::Instance()->getQuestScript(questid, (isstart ? 0 : 1));
	}
	if (FileUtilities::fileExists(filename)) {
		luaNPC.reset(new LuaNPC(filename, player->getId()));
		player->setNPC(this);
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

void NPC::run() {
	if (checkEnd()) {
		return;
	}
	luaNPC->run();
	checkEnd();
}

PacketCreator NPC::npcPacket(int8_t type) {
	sentDialog = type;

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

void NPC::sendDialog(bool back, bool next, bool save) {
	if (save) { // Store the current NPC state, for future "back" button use
		previousStates.push_back(StatePtr(new State(text, back, next)));
	}

	PacketCreator packet = npcPacket(NPCDialogs::normal);
	packet.add<int8_t>(back);
	packet.add<int8_t>(next);
	player->getSession()->send(packet);
}

void NPC::sendDialog(StatePtr npcState) {
	text = npcState->text;
	sendDialog(npcState->back, npcState->next, false);
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

void NPC::proceedBack() {
	if (state == 0) {
		// hacking
		return;
	}

	state--;
	sendDialog(previousStates[state]);
}

void NPC::proceedNext() {
	state++;
	if (state < previousStates.size()) {
		// Usage of "next" button after the "back" button
		sendDialog(previousStates[state]);
	}
	else {
		luaNPC->proceedNext();
	}
}

void NPC::proceedSelection(uint8_t selected) {
	luaNPC->proceedSelection(selected);
}

void NPC::proceedNumber(int32_t number) {
	luaNPC->proceedNumber(number);
}

void NPC::proceedText(const string &text) {
	luaNPC->proceedText(text);
}
