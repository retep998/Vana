/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Npc.hpp"
#include "ChannelServer.hpp"
#include "FileUtilities.hpp"
#include "LuaNpc.hpp"
#include "NpcPacket.hpp"
#include "Player.hpp"
#include "ScriptDataProvider.hpp"
#include "Session.hpp"
#include "SmsgHeader.hpp"
#include <string>

namespace Vana {

Npc::Npc(npc_id_t npcId, Player *player, quest_id_t questId, bool isStart) :
	m_player{player},
	m_npcId{npcId}
{
	initScript(getScript(questId, isStart));
}

Npc::Npc(npc_id_t npcId, Player *player, const Point &pos, quest_id_t questId, bool isStart) :
	m_pos{pos},
	m_player{player},
	m_npcId{npcId}
{
	initScript(getScript(questId, isStart));
}

Npc::Npc(npc_id_t npcId, Player *player, const string_t &script) :
	m_player{player},
	m_npcId{npcId}
{
	initScript(script);
}

auto Npc::hasScript(int32_t npcId, quest_id_t questId, bool start) -> bool {
	string_t script = "";
	auto &channel = ChannelServer::getInstance();
	if (questId == 0) {
		script = channel.getScriptDataProvider().getScript(&channel, npcId, ScriptTypes::Npc);
	}
	else {
		script = channel.getScriptDataProvider().getQuestScript(&channel, questId, start ? 0 : 1);
	}
	return FileUtilities::fileExists(script);
}

auto Npc::getScript(quest_id_t questId, bool start) -> string_t {
	auto &channel = ChannelServer::getInstance();
	if (questId == 0) {
		return channel.getScriptDataProvider().getScript(&channel, m_npcId, ScriptTypes::Npc);
	}
	return channel.getScriptDataProvider().getQuestScript(&channel, questId, start ? 0 : 1);
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

auto Npc::setEndScript(npc_id_t npcId, const string_t &fullscript) -> void {
	m_nextNpc = npcId;
	m_script = fullscript;
}

auto Npc::checkEnd() -> bool {
	if (isEnd()) {
		if (m_nextNpc != 0) {
			Npc *npc = new Npc{m_nextNpc, m_player, m_script};
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

auto Npc::sendSimple() -> void {
	m_sentDialog = Packets::Npc::Dialogs::Simple;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text));
	m_text = "";
}

auto Npc::sendYesNo() -> void {
	m_sentDialog = Packets::Npc::Dialogs::YesNo;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text));
	m_text = "";
}

auto Npc::sendDialog(bool back, bool next, bool save) -> void {
	if (save) {
		// Store the current NPC state, for future "back" button use
		m_previousStates.push_back(make_ref_ptr<NpcChatState>(m_text, back, next));
	}

	m_sentDialog = Packets::Npc::Dialogs::Normal;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text)
		.add<bool>(back)
		.add<bool>(next));
	m_text = "";
}

auto Npc::sendDialog(ref_ptr_t<NpcChatState> npcState) -> void {
	m_text = npcState->text;
	sendDialog(npcState->back, npcState->next, false);
}

auto Npc::sendAcceptDecline() -> void {
	m_sentDialog = Packets::Npc::Dialogs::AcceptDecline;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text));
	m_text = "";
}

auto Npc::sendAcceptDeclineNoExit() -> void {
	m_sentDialog = Packets::Npc::Dialogs::AcceptDeclineNoExit;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text));
	m_text = "";
}

auto Npc::sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time) -> void {
	m_sentDialog = Packets::Npc::Dialogs::Quiz;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, "", false)
		.add<int8_t>(0)
		.add<int32_t>(type)
		.add<int32_t>(objectId)
		.add<int32_t>(correct)
		.add<int32_t>(questions)
		.add<int32_t>(time));
}

auto Npc::sendQuestion(const string_t &question, const string_t &clue, int32_t minLength, int32_t maxLength, int32_t time) -> void {
	m_sentDialog = Packets::Npc::Dialogs::Question;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, "", false)
		.add<int8_t>(0x00) // If it's 0x01, it does something else
		.add<string_t>(m_text)
		.add<string_t>(question) // Another question thing
		.add<string_t>(clue)
		.add<int32_t>(minLength)
		.add<int32_t>(maxLength)
		.add<int32_t>(time));
}

auto Npc::sendGetText(int16_t min, int16_t max, const string_t &def) -> void {
	m_sentDialog = Packets::Npc::Dialogs::GetText;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text)
		.add<string_t>(def)
		.add<int16_t>(min)
		.add<int16_t>(max));
	m_text = "";
}

auto Npc::sendGetNumber(int32_t def, int32_t min, int32_t max) -> void {
	m_sentDialog = Packets::Npc::Dialogs::GetNumber;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text)
		.add<int32_t>(def)
		.add<int32_t>(min)
		.add<int32_t>(max));
	m_text = "";
}

auto Npc::sendStyle(vector_t<int32_t> styles) -> void {
	m_sentDialog = Packets::Npc::Dialogs::Style;
	m_player->send(Packets::Npc::npcChat(m_sentDialog, m_npcId, m_text)
		.add<uint8_t>(static_cast<uint8_t>(styles.size()))
		.add<vector_t<int32_t>>(styles, styles.size()));
	m_text = "";
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

}