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
#include <hash_map>
#include <vector>
#include <string>
#include <sstream>
#include <boost/scoped_ptr.hpp>

using std::string;
using std::vector;
using stdext::hash_map;
using boost::scoped_ptr;

class Player;
class PacketCreator;
class ReadPacket;

namespace NPCs {
	void handleNPC(Player *player, ReadPacket *packet);
	void handleQuestNPC(Player *player, int npcid, bool start);
	void handleNPCIn(Player *player, ReadPacket *packet);
};

class NPC {
private:
	bool checkEnd();

	int npcid;
	Player *player;
	string text;
	int state;
	int selected;
	bool cend;
	int getnum;
	string gettext;
	scoped_ptr<LuaNPC> luaNPC;
public:
	NPC(int npcid, Player *player, bool isquest = false, bool isstart = false);
	~NPC();

	bool run();

	void addText(const string &text) {
		this->text += text;
	}

	PacketCreator & npcPacket(char type);
	void sendSimple();
	void sendYesNo();
	void sendNext();
	void sendBackNext();
	void sendBackOK();
	void sendOK();
	void sendAcceptDecline();
	void sendGetText();
	void sendGetNumber(int def, int min, int max);
	void sendStyle(int styles[], char size);
	void setState(int state);
	int getState() {
		return state;
	}
	int getSelected() {
		return selected;
	}
	void setSelected(int selected) {
		this->selected = selected;
	}
	void setGetNumber(int num) {
		this->getnum = num;
	}
	int getNumber() {
		return getnum;
	}
	void setGetText(const string &text) {
		gettext = text;
	}
	string & getText() {
		return gettext;
	}
	void end() {
		cend = true;
	}
	bool isEnd() {
		return cend;
	}
	int getNpcID() {
		return npcid;
	}
	Player * getPlayer() {
		return player;
	}
	void showShop();
};

namespace NPCDialogs {
	const unsigned char normal = 0x00;
	const unsigned char yesNo = 0x01;
	const unsigned char getText = 0x02;
	const unsigned char getNumber = 0x03;
	const unsigned char simple = 0x04;
	const unsigned char style = 0x07;
	const unsigned char acceptDecline = 0x0c;
};

#endif
