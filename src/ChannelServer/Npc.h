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
#pragma once

#include "LuaNpc.h"
#include "Pos.h"
#include "Types.h"
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

class PacketCreator;
class Player;

class Npc {
public:
	Npc(int32_t npcId, Player *player, int16_t questId = 0, bool isStart = false);
	Npc(int32_t npcId, Player *player, const Pos &pos, int16_t questId = 0, bool isStart = false);
	Npc(int32_t npcId, Player *player, const string &script);

	static bool hasScript(int32_t npcId, int16_t questId, bool start);

	void run();

	PacketCreator npcPacket(int8_t type, bool addText = true);

	void sendSimple();
	void sendYesNo();
	void sendDialog(bool back, bool next, bool save = true);
	void sendAcceptDecline();
	void sendAcceptDeclineNoExit();
	void sendGetText(int16_t min, int16_t max, const string &def = "");
	void sendGetNumber(int32_t def, int32_t min, int32_t max);
	void sendStyle(int32_t styles[], uint8_t size);
	void sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time);
	void sendQuestion(const string &question, const string &clue, int32_t minCharacters, int32_t maxCharacters, int32_t time);
	void addText(const string &text) { m_text += text; }
	void end() { m_cend = true; }

	void proceedBack();
	void proceedNext();
	void proceedSelection(uint8_t selected); // Yes/No, Accept/Decline, List
	void proceedNumber(int32_t number); // sendGetNumber
	void proceedText(const string &text); // sendGetText and sendQuiz

	Player * getPlayer() const { return m_player; }
	uint8_t getSentDialog() const { return m_sentDialog; }
	int32_t getNpcId() const { return m_npcId; }
	int32_t getNumber() const { return m_getNum; }
	int32_t getSelected() const { return m_selected; }
	string & getText() { return m_getText; }

	bool isEnd() const { return m_cend; }
	Pos getPos() const { return m_pos; }

	void setEndScript(int32_t npcId, const string &fullscript);

	bool checkEnd();
	void showShop();
private:
	struct State { // For "back" button
		State(const string &text, bool back, bool next) : text(text), back(back), next(next) {}
		string text;
		bool back;
		bool next;
	};
	typedef shared_ptr<State> StatePtr;

	void sendDialog(StatePtr npcState);
	void initData(Player *p, int32_t id);
	string getScript(int16_t questId, bool start);
	void initScript(Player *player, int32_t npcId, const string &filename);

	bool m_cend;
	uint8_t m_sentDialog; // Used to check if the user respond with the same type of the dialog sent
	uint8_t m_selected;
	int32_t m_nextNpc;
	int32_t m_npcId;
	int32_t m_getNum;
	string m_text;
	string m_getText;
	string m_script;
	Player *m_player;
	Pos m_pos;

	uint32_t m_state; // For "back" button
	vector<StatePtr> m_previousStates; // For "back" button

	unique_ptr<LuaNpc> m_luaNpc;
};

namespace NpcDialogs {
	enum DialogOptions : int8_t {
		Normal = 0x00,
		YesNo = 0x01,
		GetText = 0x02,
		GetNumber = 0x03,
		Simple = 0x05,
		Question = 0x06,
		Quiz = 0x07,
		Style = 0x09, // 0x15 same
		AcceptDecline = 0x0F,
		AcceptDeclineNoExit = 0x010
	};
}