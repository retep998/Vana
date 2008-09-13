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

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace mysqlpp { class Row; };
class PlayerLogin;
class ReadPacket;

struct CharEquip {
	int32_t id;
	int16_t slot;
};

struct Character {
	int32_t id;
	string name;
	int8_t gender;
	int8_t skin;
	int32_t eyes;
	int32_t hair;
	int8_t level;
	int16_t job;
	int16_t str;
	int16_t dex;
	int16_t intt;
	int16_t luk;
	int16_t hp;
	int16_t mhp;
	int16_t mp;
	int16_t mmp;
	int16_t ap;
	int16_t sp;
	int16_t exp;
	int16_t fame;
	int32_t map;
	int8_t pos;
	vector<CharEquip> equips;
};

namespace Characters {
	void connectGame(PlayerLogin *player, ReadPacket *packet);
	void checkCharacterName(PlayerLogin *player, ReadPacket *packet);
	void createCharacter(PlayerLogin *player, ReadPacket *packet);
	void deleteCharacter(PlayerLogin *player, ReadPacket *packet);
	void showCharacters(PlayerLogin *player);
	void loadCharacter(Character &charc, mysqlpp::Row &row);
	void showEquips(int32_t id, vector<CharEquip> &vec);
	void createEquip(int32_t equipid, int32_t type, int32_t charid);
	bool ownerCheck(PlayerLogin *player, int32_t id);
	bool nameTaken(PlayerLogin *player, const string &name);
};

#endif
