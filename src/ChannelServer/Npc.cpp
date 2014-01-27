/*
Copyright (C) 2008-2014 Vana Development Team

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

Npc::Npc(int32_t npcId, Player *player, int16_t questId, bool isStart) :
	m_player(player),
	m_npcId(npcId)
{
	initScript(getScript(questId, isStart));
}

Npc::Npc(int32_t npcId, Player *player, const Pos &pos, int16_t questId, bool isStart) :
	m_pos(pos),
	m_player(player),
	m_npcId(npcId)
{
	initScript(getScript(questId, isStart));
}

Npc::Npc(int32_t npcId, Player *player, const string_t &script) :
	m_player(player),
	m_npcId(npcId)
{
	initScript(script);
}

auto Npc::hasScript(int32_t npcId, int16_t questId, bool start) -> bool {
	string_t script = "";
	if (questId == 0) {
		script = ScriptDataProvider::getInstance().getScript(npcId, ScriptTypes::Npc);
	}
	else {
		script = ScriptDataProvider::getInstance().getQuestScript(questId, (start ? 0 : 1));
	}
	return FileUtilities::fileExists(script);
}

auto Npc::getScript(int16_t questId, bool start) -> string_t {
	if (questId == 0) {
		return ScriptDataProvider::getInstance().getScript(m_npcId, ScriptTypes::Npc);
	}
	return ScriptDataProvider::getInstance().getQuestScript(questId, (start ? 0 : 1));
}

auto Npc::initScript(const string_t &filename) -> void {
	if (FileUtilities::fileExists(filename)) {
		m_luaNpc = make_owned_ptr<LuaNpc>(filename, m_player->getId());
		m_player->setNpc(this);
	}
	else {
		end();
	}
}

auto Npc::setEndScript(int32_t npcId, const string_t &fullscript) -> void {
	m_nextNpc = npcId;
	m_script = fullscript;
}

auto Npc::checkEnd() -> bool {
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

auto Npc::run() -> void {
	if (checkEnd()) {
		return;
	}
	m_luaNpc->run();
	checkEnd();
}

auto Npc::npcPacket(int8_t type, bool addText) -> PacketCreator {
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

auto Npc::sendSimple() -> void {
	PacketCreator packet = npcPacket(NpcDialogs::Simple);

	m_player->getSession()->send(packet);
}

auto Npc::sendYesNo() -> void {
	PacketCreator packet = npcPacket(NpcDialogs::YesNo);
	m_player->getSession()->send(packet);
}

auto Npc::sendDialog(bool back, bool next, bool save) -> void {
	if (save) {
		// Store the current NPC state, for future "back" button use
		m_previousStates.push_back(make_ref_ptr<NpcChatState>(m_text, back, next));
	}

	PacketCreator packet = npcPacket(NpcDialogs::Normal);
	packet.add<bool>(back);
	packet.add<bool>(next);
	m_player->getSession()->send(packet);
}

auto Npc::sendDialog(ref_ptr_t<NpcChatState> npcState) -> void {
	m_text = npcState->text;
	sendDialog(npcState->back, npcState->next, false);
}

auto Npc::sendAcceptDecline() -> void {
	PacketCreator packet = npcPacket(NpcDialogs::AcceptDecline);
	m_player->getSession()->send(packet);
}

auto Npc::sendAcceptDeclineNoExit() -> void {
	PacketCreator packet = npcPacket(NpcDialogs::AcceptDeclineNoExit);
	m_player->getSession()->send(packet);
}

auto Npc::sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time) -> void {
	PacketCreator packet = npcPacket(NpcDialogs::Quiz, false);
	packet.add<int8_t>(0);
	packet.add<int32_t>(type); // 0 = NPC, 1 = Mob, 2 = Item
	packet.add<int32_t>(objectId);
	packet.add<int32_t>(correct);
	packet.add<int32_t>(questions);
	packet.add<int32_t>(time);
	m_player->getSession()->send(packet);
}

auto Npc::sendQuestion(const string_t &question, const string_t &clue, int32_t minCharacters, int32_t maxCharacters, int32_t time) -> void {
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

auto Npc::sendGetText(int16_t min, int16_t max, const string_t &def) -> void {
	PacketCreator packet = npcPacket(NpcDialogs::GetText);
	packet.addString(def);
	packet.add<int16_t>(min);
	packet.add<int16_t>(max);
	m_player->getSession()->send(packet);
}

auto Npc::sendGetNumber(int32_t def, int32_t min, int32_t max) -> void {
	PacketCreator packet = npcPacket(NpcDialogs::GetNumber);
	packet.add<int32_t>(def);
	packet.add<int32_t>(min);
	packet.add<int32_t>(max);
	m_player->getSession()->send(packet);
}

auto Npc::sendStyle(int32_t styles[], uint8_t size) -> void {
	PacketCreator packet = npcPacket(NpcDialogs::Style);
	packet.add<uint8_t>(size);
	for (uint8_t i = 0; i < size; i++) {
		packet.add<int32_t>(styles[i]);
	}
	m_player->getSession()->send(packet);
}

auto Npc::proceedBack() -> void {
	if (m_state == 0) {
		// Hacking
		return;
	}

	m_state--;
	sendDialog(m_previousStates[m_state]);
}

auto Npc::proceedNext() -> void {
	m_state++;
	if (m_state < m_previousStates.size()) {
		// Usage of "next" button after the "back" button
		sendDialog(m_previousStates[m_state]);
	}
	else {
		m_luaNpc->proceedNext();
	}
}

auto Npc::proceedSelection(uint8_t selected) -> void {
	m_luaNpc->proceedSelection(selected);
}

auto Npc::proceedNumber(int32_t number) -> void {
	m_luaNpc->proceedNumber(number);
}

auto Npc::proceedText(const string_t &text) -> void {
	m_luaNpc->proceedText(text);
}