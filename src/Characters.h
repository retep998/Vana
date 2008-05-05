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

#include "PlayerLogin.h"
#include "LoginPacket.h"
#include "BufferUtilities.h"
#include "MySQLM.h"
#include <vector>

using namespace std;

struct CharEquip {
	char type;
	int id;
};

struct Character {
	int id;
	char name[15];
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

class PlayerLogin;

namespace Characters {
	void connectGame(PlayerLogin* player, unsigned char* packet);
	void checkCharacterName(PlayerLogin* player, unsigned char* packet);
	void createCharacter(PlayerLogin* player, unsigned char* packet);
	void deleteCharacter(PlayerLogin* player, unsigned char* packet);
	void showCharacters(PlayerLogin* player);
	extern private void loadCharacter(Character &charc, mysqlpp::Row row);
	extern private void showEquips(int id, vector <CharEquip> &vec);
	extern private void createEquip(int equipid, int type, int charid);
	extern private bool ownerCheck(PlayerLogin* player, int id);
	extern private bool nameTaken(char *name);
};

#endif