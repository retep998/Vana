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
#include "Npc.h"
#include "FileUtilities.h"
#include "LuaNpc.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "ScriptDataProvider.h"
#include <string>

using std::string;

NPC::NPC(int32_t npcid, Player *player, int16_t questid, bool isstart) :
pos(Pos(0, 0))
{
	initData(player, npcid);
	initScript(player, npcid, getScript(questid, isstart));
}

NPC::NPC(int32_t npcid, Player *player, const Pos &pos, int16_t questid, bool isstart) :
pos(pos)
{
	initData(player, npcid);
	initScript(player, npcid, getScript(questid, isstart));
}

NPC::NPC(int32_t npcid, Player *player, const string &script) :
pos(Pos(0, 0))
{
	initData(player, npcid);
	initScript(player, npcid, script);
}

NPC::~NPC() {
	player->setNPC(0);
}

void NPC::initData(Player *p, int32_t id) {
	player = p;
	npcid = id;

	nextnpc = 0;
	script = "";

	state = 0;
	text = "";
	cend = false;
}

bool NPC::hasScript(int32_t npcid, int16_t questid, bool start) {
	string script = "";
	if (questid == 0) {
		script = ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	else {
		script = ScriptDataProvider::Instance()->getQuestScript(questid, (start ? 0 : 1));
	}
	return FileUtilities::fileExists(script);
}

string NPC::getScript(int16_t questid, bool start) {
	if (questid == 0) {
		return ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	return ScriptDataProvider::Instance()->getQuestScript(questid, (start ? 0 : 1));
}

void NPC::initScript(Player *player, int32_t npcid, const string &filename) {
	if (FileUtilities::fileExists(filename)) {
		luaNPC.reset(new LuaNPC(filename, player->getId()));
		player->setNPC(this);
	}
	else {
		end();
	}
}

void NPC::setEndScript(int32_t npcid, const string &fullscript) {
	nextnpc = npcid;
	script = fullscript;
}

bool NPC::checkEnd() {
	if (isEnd()) {
		if (nextnpc != 0) {
			NPC *npc = new NPC(nextnpc, player, script);
			npc->run();
		}
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
	packet.add<int16_t>(SMSG_NPC_TALK);
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
	packet.addBool(back);
	packet.addBool(next);
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
