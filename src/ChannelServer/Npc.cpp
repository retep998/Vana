/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "PacketCreator.h"
#include "Player.h"
#include "ScriptDataProvider.h"
#include "Session.h"
#include "SmsgHeader.h"
#include <string>

using std::string;

Npc::Npc(int32_t npcId, Player *player, int16_t questId, bool isStart) :
	m_pos(Pos(0, 0))
{
	initData(player, npcId);
	initScript(player, npcId, getScript(questId, isStart));
}

Npc::Npc(int32_t npcId, Player *player, const Pos &pos, int16_t questId, bool isStart) :
	m_pos(pos)
{
	initData(player, npcId);
	initScript(player, npcId, getScript(questId, isStart));
}

Npc::Npc(int32_t npcId, Player *player, const string &script) :
	m_pos(Pos(0, 0))
{
	initData(player, npcId);
	initScript(player, npcId, script);
}

void Npc::initData(Player *p, int32_t id) {
	m_player = p;
	m_npcId = id;

	m_nextNpc = 0;
	m_script = "";

	m_state = 0;
	m_text = "";
	m_cend = false;
}

bool Npc::hasScript(int32_t npcId, int16_t questId, bool start) {
	string script = "";
	if (questId == 0) {
		script = ScriptDataProvider::Instance()->getScript(npcId, ScriptTypes::Npc);
	}
	else {
		script = ScriptDataProvider::Instance()->getQuestScript(questId, (start ? 0 : 1));
	}
	return FileUtilities::fileExists(script);
}

string Npc::getScript(int16_t questId, bool start) {
	if (questId == 0) {
		return ScriptDataProvider::Instance()->getScript(m_npcId, ScriptTypes::Npc);
	}
	return ScriptDataProvider::Instance()->getQuestScript(questId, (start ? 0 : 1));
}

void Npc::initScript(Player *player, int32_t npcId, const string &filename) {
	if (FileUtilities::fileExists(filename)) {
		m_luaNpc.reset(new LuaNpc(filename, player->getId()));
		player->setNpc(this);
	}
	else {
		end();
	}
}

void Npc::setEndScript(int32_t npcId, const string &fullscript) {
	m_nextNpc = npcId;
	m_script = fullscript;
}

bool Npc::checkEnd() {
	if (isEnd()) {
		if (m_nextNpc != 0) {
			Npc *npc = new Npc(m_nextNpc, m_player, m_script);
			npc->run();
		}
		m_player->setNpc(nullptr);
		return true;
	}

	return false;
}

void Npc::run() {
	if (checkEnd()) {
		return;
	}
	m_luaNpc->run();
	checkEnd();
}

PacketCreator Npc::npcPacket(int8_t type, bool addText) {
	m_sentDialog = type;

	PacketCreator packet;
	packet.add<header_t>(SMSG_NPC_TALK);
	packet.add<int8_t>(4);
	packet.add<int32_t>(m_npcId);
	packet.add<int8_t>(type);
	if (addText) {
		packet.addString(m_text);
		m_text = "";
	}

	return packet;
}

void Npc::sendSimple() {
	PacketCreator packet = npcPacket(NpcDialogs::Simple);

	m_player->getSession()->send(packet);
}

void Npc::sendYesNo() {
	PacketCreator packet = npcPacket(NpcDialogs::YesNo);
	m_player->getSession()->send(packet);
}

void Npc::sendDialog(bool back, bool next, bool save) {
	if (save) {
		// Store the current Npc state, for future "back" button use
		m_previousStates.push_back(StatePtr(new State(m_text, back, next)));
	}

	PacketCreator packet = npcPacket(NpcDialogs::Normal);
	packet.addBool(back);
	packet.addBool(next);
	m_player->getSession()->send(packet);
}

void Npc::sendDialog(StatePtr npcState) {
	m_text = npcState->text;
	sendDialog(npcState->back, npcState->next, false);
}

void Npc::sendAcceptDecline() {
	PacketCreator packet = npcPacket(NpcDialogs::AcceptDecline);
	m_player->getSession()->send(packet);
}

void Npc::sendAcceptDeclineNoExit() {
	PacketCreator packet = npcPacket(NpcDialogs::AcceptDeclineNoExit);
	m_player->getSession()->send(packet);
}

void Npc::sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time) {
	PacketCreator packet = npcPacket(NpcDialogs::Quiz, false);
	packet.add<int8_t>(0);
	packet.add<int32_t>(type); // 0 = NPC, 1 = Mob, 2 = Item
	packet.add<int32_t>(objectId);
	packet.add<int32_t>(correct);
	packet.add<int32_t>(questions);
	packet.add<int32_t>(time);
	m_player->getSession()->send(packet);
}

void Npc::sendQuestion(const string &question, const string &clue, int32_t minCharacters, int32_t maxCharacters, int32_t time) {
	PacketCreator packet = npcPacket(NpcDialogs::Question, false);
	packet.add<int8_t>(0x00); // If it's 0x01, it does something else
	packet.addString(m_text);
	packet.addString(question); // Another question thing
	packet.addString(clue);
	packet.add<int32_t>(minCharacters);
	packet.add<int32_t>(maxCharacters);
	packet.add<int32_t>(time);
	m_player->getSession()->send(packet);
}

void Npc::sendGetText(int16_t min, int16_t max, const string &def) {
	PacketCreator packet = npcPacket(NpcDialogs::GetText);
	packet.addString(def);
	packet.add<int16_t>(min);
	packet.add<int16_t>(max);
	m_player->getSession()->send(packet);
}

void Npc::sendGetNumber(int32_t def, int32_t min, int32_t max) {
	PacketCreator packet = npcPacket(NpcDialogs::GetNumber);
	packet.add<int32_t>(def);
	packet.add<int32_t>(min);
	packet.add<int32_t>(max);
	m_player->getSession()->send(packet);
}

void Npc::sendStyle(int32_t styles[], uint8_t size) {
	PacketCreator packet = npcPacket(NpcDialogs::Style);
	packet.add<uint8_t>(size);
	for (uint8_t i = 0; i < size; i++) {
		packet.add<int32_t>(styles[i]);
	}
	m_player->getSession()->send(packet);
}

void Npc::proceedBack() {
	if (m_state == 0) {
		// Hacking
		return;
	}

	m_state--;
	sendDialog(m_previousStates[m_state]);
}

void Npc::proceedNext() {
	m_state++;
	if (m_state < m_previousStates.size()) {
		// Usage of "next" button after the "back" button
		sendDialog(m_previousStates[m_state]);
	}
	else {
		m_luaNpc->proceedNext();
	}
}

void Npc::proceedSelection(uint8_t selected) {
	m_luaNpc->proceedSelection(selected);
}

void Npc::proceedNumber(int32_t number) {
	m_luaNpc->proceedNumber(number);
}

void Npc::proceedText(const string &text) {
	m_luaNpc->proceedText(text);
}