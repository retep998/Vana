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
#ifndef CHARACTERS_H
#define CHARACTERS_H

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace mysqlpp { class Row; };
class PlayerLogin;
class ReadPacket;

struct CharEquip {
	int id;
	short slot;
};

struct Character {
	int id;
	string name;
	char gender;
	char skin;
	int eyes;
	int hair;
	char level;
	short job;
	short str;
	short dex;
	short intt;
	short luk;
	short hp;
	short mhp;
	short mp;
	short mmp;
	short ap;
	short sp;
	short exp;
	short fame;
	int map;
	char pos;
	vector <CharEquip> equips;
};

namespace Characters {
	void connectGame(PlayerLogin *player, ReadPacket *packet);
	void checkCharacterName(PlayerLogin *player, ReadPacket *packet);
	void createCharacter(PlayerLogin *player, ReadPacket *packet);
	void deleteCharacter(PlayerLogin *player, ReadPacket *packet);
	void showCharacters(PlayerLogin *player);
	void loadCharacter(Character &charc, mysqlpp::Row &row);
	void showEquips(int id, vector <CharEquip> &vec);
	void createEquip(int equipid, int type, int charid);
	bool ownerCheck(PlayerLogin *player, int id);
	bool nameTaken(PlayerLogin *player, const string &name);
};

#endif
