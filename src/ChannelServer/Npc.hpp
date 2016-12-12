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
#pragma once

#include "LuaNpc.hpp"
#include "Point.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <vector>

class Player;

class Npc {
	NONCOPYABLE(Npc);
	NO_DEFAULT_CONSTRUCTOR(Npc);
public:
	Npc(npc_id_t npcId, Player *player, quest_id_t questId = 0, bool isStart = false);
	Npc(npc_id_t npcId, Player *player, const Point &pos, quest_id_t questId = 0, bool isStart = false);
	Npc(npc_id_t npcId, Player *player, const string_t &script);

	static auto hasScript(npc_id_t npcId, quest_id_t questId, bool start) -> bool;

	auto run() -> void;

	auto sendSimple() -> void;
	auto sendYesNo() -> void;
	auto sendDialog(bool back, bool next, bool save = true) -> void;
	auto sendAcceptDecline() -> void;
	auto sendAcceptDeclineNoExit() -> void;
	auto sendGetText(int16_t min, int16_t max, const string_t &def = "") -> void;
	auto sendGetNumber(int32_t def, int32_t min, int32_t max) -> void;
	auto sendStyle(vector_t<int32_t> styles) -> void;
	auto sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time) -> void;
	auto sendQuestion(const string_t &question, const string_t &clue, int32_t minCharacters, int32_t maxCharacters, int32_t time) -> void;
	auto addText(const string_t &text) -> void { m_text += text; }
	auto end() -> void { m_cend = true; }

	auto proceedBack() -> void;
	auto proceedNext() -> void;
	auto proceedSelection(uint8_t selected) -> void;
	auto proceedNumber(int32_t number) -> void;
	auto proceedText(const string_t &text) -> void;

	auto getPlayer() const -> Player * { return m_player; }
	auto getSentDialog() const -> uint8_t { return m_sentDialog; }
	auto getNpcId() const -> npc_id_t { return m_npcId; }
	auto getNumber() const -> int32_t { return m_getNum; }
	auto getSelected() const -> int32_t { return m_selected; }
	auto getText() -> string_t & { return m_getText; }

	auto isEnd() const -> bool { return m_cend; }
	auto getPos() const -> Point { return m_pos; }

	auto setEndScript(int32_t npcId, const string_t &fullscript) -> void;

	auto checkEnd() -> bool;
	auto showShop() -> void;
private:
	struct NpcChatState {
		NONCOPYABLE(NpcChatState);
	public:
		NpcChatState(const string_t &text, bool back, bool next) :
			text(text),
			back(back),
			next(next)
		{
		}

		bool back = false;
		bool next = false;
		string_t text;
	};

	auto sendDialog(ref_ptr_t<NpcChatState> npcState) -> void;
	auto getScript(quest_id_t questId, bool start) -> string_t;
	auto initScript(const string_t &filename) -> void;

	bool m_cend = false;
	uint8_t m_sentDialog = 0; // Used to check if the user respond with the same type of the dialog sent
	uint8_t m_selected = 0;
	int32_t m_nextNpc = 0;
	npc_id_t m_npcId = 0;
	int32_t m_getNum = 0;
	uint32_t m_state = 0;
	Player *m_player = nullptr;
	string_t m_text;
	string_t m_getText;
	string_t m_script;
	Point m_pos;
	owned_ptr_t<LuaNpc> m_luaNpc;
	vector_t<ref_ptr_t<NpcChatState>> m_previousStates;
};