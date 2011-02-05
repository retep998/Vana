/*
Copyright (C) 2008-2011 Vana Development Team

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

Npc::Npc(int32_t npcid, Player *player, int16_t questid, bool isstart) :
pos(Pos(0, 0))
{
	initData(player, npcid);
	initScript(player, npcid, getScript(questid, isstart));
}

Npc::Npc(int32_t npcid, Player *player, const Pos &pos, int16_t questid, bool isstart) :
pos(pos)
{
	initData(player, npcid);
	initScript(player, npcid, getScript(questid, isstart));
}

Npc::Npc(int32_t npcid, Player *player, const string &script) :
pos(Pos(0, 0))
{
	initData(player, npcid);
	initScript(player, npcid, script);
}

void Npc::initData(Player *p, int32_t id) {
	player = p;
	npcid = id;

	nextnpc = 0;
	script = "";

	state = 0;
	text = "";
	cend = false;
}

bool Npc::hasScript(int32_t npcid, int16_t questid, bool start) {
	string script = "";
	if (questid == 0) {
		script = ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	else {
		script = ScriptDataProvider::Instance()->getQuestScript(questid, (start ? 0 : 1));
	}
	return FileUtilities::fileExists(script);
}

string Npc::getScript(int16_t questid, bool start) {
	if (questid == 0) {
		return ScriptDataProvider::Instance()->getNpcScript(npcid);
	}
	return ScriptDataProvider::Instance()->getQuestScript(questid, (start ? 0 : 1));
}

void Npc::initScript(Player *player, int32_t npcid, const string &filename) {
	if (FileUtilities::fileExists(filename)) {
		luaNpc.reset(new LuaNpc(filename, player->getId()));
		player->setNpc(this);
	}
	else {
		end();
	}
}

void Npc::setEndScript(int32_t npcid, const string &fullscript) {
	nextnpc = npcid;
	script = fullscript;
}

bool Npc::checkEnd() {
	if (isEnd()) {
		if (nextnpc != 0) {
			Npc *npc = new Npc(nextnpc, player, script);
			npc->run();
		}
		player->setNpc(nullptr); // The resetting of the scoped_ptr will delete the npc...
		return true;
	}

	return false;
}

void Npc::run() {
	if (checkEnd()) {
		return;
	}
	luaNpc->run();
	checkEnd();
}

PacketCreator Npc::npcPacket(int8_t type, bool addText) {
	sentDialog = type;

	PacketCreator packet;
	packet.addHeader(SMSG_NPC_TALK);
	packet.add<int8_t>(4);
	packet.add<int32_t>(npcid);
	packet.add<int8_t>(type);

	if (addText) {
		packet.addString(text);
		text = "";
	}

	return packet;
}

void Npc::sendSimple() {
	PacketCreator packet = npcPacket(NpcDialogs::Simple);

	player->getSession()->send(packet);
}

void Npc::sendYesNo() {
	PacketCreator packet = npcPacket(NpcDialogs::YesNo);
	player->getSession()->send(packet);
}

void Npc::sendDialog(bool back, bool next, bool save) {
	if (save) {
		// Store the current NPC state, for future "back" button use
		previousStates.push_back(StatePtr(new State(text, back, next)));
	}

	PacketCreator packet = npcPacket(NpcDialogs::Normal);
	packet.addBool(back);
	packet.addBool(next);
	player->getSession()->send(packet);
}

void Npc::sendDialog(StatePtr npcState) {
	text = npcState->text;
	sendDialog(npcState->back, npcState->next, false);
}

void Npc::sendAcceptDecline() {
	PacketCreator packet = npcPacket(NpcDialogs::AcceptDecline);
	player->getSession()->send(packet);
}

void Npc::sendAcceptDeclineNoExit() {
	PacketCreator packet = npcPacket(NpcDialogs::AcceptDeclineNoExit);
	player->getSession()->send(packet);
}

void Npc::sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time) {
	PacketCreator packet = npcPacket(NpcDialogs::Quiz, false);
	packet.add<int8_t>(0);
	packet.add<int32_t>(type); // 0 = NPC, 1 = Mob, 2 = Item
	packet.add<int32_t>(objectId);
	packet.add<int32_t>(correct);
	packet.add<int32_t>(questions);
	packet.add<int32_t>(time);
	player->getSession()->send(packet);
}

void Npc::sendQuestion(const string &question, const string &clue, int32_t minCharacters, int32_t maxCharacters, int32_t time) {
	PacketCreator packet = npcPacket(NpcDialogs::Question, false);
	packet.add<int8_t>(0x00); // If it's 0x01, it does something else 
	packet.addString(text);
	packet.addString(question); // Another question thing
	packet.addString(clue);
	packet.add<int32_t>(minCharacters);
	packet.add<int32_t>(maxCharacters);
	packet.add<int32_t>(time);
	player->getSession()->send(packet);

	text = "";
}

void Npc::sendGetText(int16_t min, int16_t max, const string &def) {
	PacketCreator packet = npcPacket(NpcDialogs::GetText);
	packet.addString(def);
	packet.add<int16_t>(min);
	packet.add<int16_t>(max);
	player->getSession()->send(packet);
}

void Npc::sendGetNumber(int32_t def, int32_t min, int32_t max) {
	PacketCreator packet = npcPacket(NpcDialogs::GetNumber);
	packet.add<int32_t>(def);
	packet.add<int32_t>(min);
	packet.add<int32_t>(max);
	player->getSession()->send(packet);
}

void Npc::sendStyle(int32_t styles[], uint8_t size) {
	PacketCreator packet = npcPacket(NpcDialogs::Style);
	packet.add<uint8_t>(size);
	for (uint8_t i = 0; i < size; i++) {
		packet.add<int32_t>(styles[i]);
	}
	player->getSession()->send(packet);
}

void Npc::proceedBack() {
	if (state == 0) {
		// Hacking
		return;
	}

	state--;
	sendDialog(previousStates[state]);
}

void Npc::proceedNext() {
	state++;
	if (state < previousStates.size()) {
		// Usage of "next" button after the "back" button
		sendDialog(previousStates[state]);
	}
	else {
		luaNpc->proceedNext();
	}
}

void Npc::proceedSelection(uint8_t selected) {
	luaNpc->proceedSelection(selected);
}

void Npc::proceedNumber(int32_t number) {
	luaNpc->proceedNumber(number);
}

void Npc::proceedText(const string &text) {
	luaNpc->proceedText(text);
}
