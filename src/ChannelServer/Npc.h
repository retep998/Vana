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
#pragma once

#include "LuaNpc.h"
#include "Pos.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/memory.hpp>
#include <string>
#include <vector>

using boost::scoped_ptr;
using std::string;
using std::tr1::shared_ptr;
using std::vector;

class PacketCreator;
class Player;

class Npc {
public:
	Npc(int32_t npcid, Player *player, int16_t questid = 0, bool isstart = false);
	Npc(int32_t npcid, Player *player, const Pos &pos, int16_t questid = 0, bool isstart = false);
	Npc(int32_t npcid, Player *player, const string &script);

	static bool hasScript(int32_t npcid, int16_t questid, bool start);

	void run();

	PacketCreator npcPacket(int8_t type, bool addText = true);

	void sendSimple();
	void sendYesNo();
	void sendDialog(bool back, bool next, bool save = true);
	void sendAcceptDecline();
	void sendAcceptDeclineNoExit();
	void sendQuiz(int8_t type, int32_t objectId, int32_t correct, int32_t questions, int32_t time);
	void sendQuestion(const string &question, const string &clue, int32_t minCharacters, int32_t maxCharacters, int32_t time);
	void sendGetText(int16_t min, int16_t max, const string &def = "");
	void sendGetNumber(int32_t def, int32_t min, int32_t max);
	void sendStyle(int32_t styles[], uint8_t size);
	void addText(const string &text) { this->text += text; }
	void end() { cend = true; }

	void proceedBack();
	void proceedNext();
	void proceedSelection(uint8_t selected); // Yes/No, Accept/Decline, List
	void proceedNumber(int32_t number); // sendGetNumber
	void proceedText(const string &text); // sendGetText and sendQuiz

	Player * getPlayer() const { return player; }
	uint8_t getSentDialog() const { return sentDialog; }
	int32_t getNpcId() const { return npcid; }
	int32_t getNumber() const { return getnum; }
	int32_t getSelected() const { return selected; }
	string & getText() { return gettext; }

	bool isEnd() const { return cend; }
	Pos getPos() const { return pos; }

	void setEndScript(int32_t npcid, const string &fullscript);

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

	bool cend;
	uint8_t sentDialog; // Used to check if the user respond with the same type of the dialog sent
	int32_t npcid;
	uint8_t selected;
	int32_t getnum;
	string text;
	string gettext;
	string script;
	int32_t nextnpc;
	Player *player;
	Pos pos;

	uint32_t state; // For "back" button
	vector<StatePtr> previousStates; // For "back" button

	scoped_ptr<LuaNpc> luaNpc;

	void initData(Player *p, int32_t id);
	string getScript(int16_t questid, bool start);
	void initScript(Player *player, int32_t npcid, const string &filename);
};

namespace NpcDialogs {
	enum DialogOptions {
		Normal = 0x00,
		YesNo = 0x01,
		GetText = 0x02,
		GetNumber = 0x03,
		Simple = 0x04,
		Question = 0x05, 
		Quiz = 0x06,
		Style = 0x07,
		AcceptDecline = 0x0C,
		AcceptDeclineNoExit = 0x0D
	};
};
