/*
Copyright (C) 2008 Vana Development Team

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
#ifndef NPCS_H
#define NPCS_H

#include "LuaNPC.h"
#include <string>
#include <boost/scoped_ptr.hpp>

using std::string;
using boost::scoped_ptr;

class Player;
class PacketCreator;
class ReadPacket;

namespace NPCs {
	void handleNPC(Player *player, ReadPacket *packet);
	void handleQuestNPC(Player *player, int32_t npcid, bool start);
	void handleNPCIn(Player *player, ReadPacket *packet);
	void handleNPCAnimation(Player *player, ReadPacket *packet);
};

class NPC {
public:
	NPC(int32_t npcid, Player *player, bool isquest = false, bool isstart = false);
	~NPC();

	bool run();

	PacketCreator & npcPacket(int8_t type);

	void sendSimple();
	void sendYesNo();
	void sendNext();
	void sendBackNext();
	void sendBackOK();
	void sendOK();
	void sendAcceptDecline();
	void sendGetText();
	void sendGetNumber(int32_t def, int32_t min, int32_t max);
	void sendStyle(int32_t styles[], int8_t size);
	void setState(int32_t state);
	void addText(const string &text) { this->text += text; }
	void setSelected(int32_t selected) { this->selected = selected; }
	void setGetNumber(int32_t num) { this->getnum = num; }
	void setGetText(const string &text) { gettext = text; }
	void end() { cend = true; }

	Player * getPlayer() const { return player; }
	int32_t getNpcID() const { return npcid; }
	int32_t getNumber() const { return getnum; }
	int32_t getState() const { return state; }
	int32_t getSelected() const { return selected; }

	string & getText() { return gettext; }
	bool isEnd() const { return cend; }

	void showShop();
private:
	bool checkEnd();

	Player *player;
	int32_t npcid;
	int32_t state;
	int32_t selected;
	int32_t getnum;
	string text;
	string gettext;
	bool cend;
	scoped_ptr<LuaNPC> luaNPC;
};

namespace NPCDialogs {
	const uint8_t normal = 0x00;
	const uint8_t yesNo = 0x01;
	const uint8_t getText = 0x02;
	const uint8_t getNumber = 0x03;
	const uint8_t simple = 0x04;
	const uint8_t style = 0x07;
	const uint8_t acceptDecline = 0x0c;
};

#endif
