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

#include "Types.hpp"
#include <string>
#include <vector>

namespace soci { class row; }
class PacketReader;
class UserConnection;

struct CharEquip {
	int32_t id = 0;
	int16_t slot = 0;
};

struct Character {
	int8_t pos = 0;
	int8_t gender = 0;
	int8_t skin = 0;
	uint8_t level = 0;
	int16_t job = 0;
	int16_t str = 0;
	int16_t dex = 0;
	int16_t intt = 0;
	int16_t luk = 0;
	int16_t hp = 0;
	int16_t mhp = 0;
	int16_t mp = 0;
	int16_t mmp = 0;
	int16_t ap = 0;
	int16_t sp = 0;
	int16_t fame = 0;
	int32_t map = 0;
	int32_t eyes = 0;
	int32_t hair = 0;
	int32_t exp = 0;
	int32_t worldRankChange = 0;
	int32_t jobRankChange = 0;
	int32_t id = 0;
	uint32_t worldRank = 0;
	uint32_t jobRank = 0;
	string_t name;
	vector_t<CharEquip> equips;
};

namespace Characters {
	auto connectGame(UserConnection *user, int32_t charId) -> void;
	auto connectGame(UserConnection *user, PacketReader &reader) -> void;
	auto connectGameWorldFromViewAllCharacters(UserConnection *user, PacketReader &reader) -> void;
	auto checkCharacterName(UserConnection *user, PacketReader &reader) -> void;
	auto createCharacter(UserConnection *user, PacketReader &reader) -> void;
	auto deleteCharacter(UserConnection *user, PacketReader &reader) -> void;
	auto showAllCharacters(UserConnection *user) -> void;
	auto showCharacters(UserConnection *user) -> void;
	auto loadCharacter(Character &charc, const soci::row &row) -> void;
	auto loadEquips(int32_t id, vector_t<CharEquip> &vec) -> void;
	auto createItem(int32_t itemId, UserConnection *user, int32_t charId, int32_t slot, int16_t amount = 1) -> void;
	auto ownerCheck(UserConnection *user, int32_t id) -> bool;
	auto nameTaken(const string_t &name) -> bool;
	auto nameInvalid(const string_t &name) -> bool;
}