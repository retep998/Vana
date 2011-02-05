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

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace mysqlpp { class Row; };
class Player;
class PacketReader;

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
	uint32_t w_rank;
	int32_t w_rankmove;
	uint32_t j_rank;
	int32_t j_rankmove;
	vector<CharEquip> equips;
};

namespace Characters {
	void connectGame(Player *player, int32_t charid);
	void connectGame(Player *player, PacketReader &packet);
	void connectGameWorld(Player *player, PacketReader &packet); // From "view all character"
	void checkCharacterName(Player *player, PacketReader &packet);
	void createCharacter(Player *player, PacketReader &packet);
	void deleteCharacter(Player *player, PacketReader &packet);
	void showAllCharacters(Player *player);
	void showCharacters(Player *player);
	void loadCharacter(Character &charc, const mysqlpp::Row &row);
	void loadEquips(int32_t id, vector<CharEquip> &vec);
	void createItem(int32_t itemid, int32_t charid, int32_t slot, int16_t amount = 1);
	bool ownerCheck(Player *player, int32_t id);
	bool nameIllegal(Player *player, const string &name); // Name is taken or not valid
};
