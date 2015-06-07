/*
Copyright (C) 2008-2015 Vana Development Team

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
	item_id_t id = 0;
	inventory_slot_t slot = 0;
};

struct Character {
	int8_t pos = 0;
	gender_id_t gender = 0;
	skin_id_t skin = 0;
	player_level_t level = 0;
	job_id_t job = 0;
	stat_t str = 0;
	stat_t dex = 0;
	stat_t intt = 0;
	stat_t luk = 0;
	health_t hp = 0;
	health_t mhp = 0;
	health_t mp = 0;
	health_t mmp = 0;
	stat_t ap = 0;
	stat_t sp = 0;
	fame_t fame = 0;
	map_id_t map = 0;
	face_id_t face = 0;
	hair_id_t hair = 0;
	experience_t exp = 0;
	int32_t worldRankChange = 0;
	int32_t jobRankChange = 0;
	player_id_t id = 0;
	uint32_t worldRank = 0;
	uint32_t jobRank = 0;
	string_t name;
	vector_t<CharEquip> equips;
};

namespace Characters {
	auto connectGame(UserConnection *user, player_id_t charId) -> void;
	auto connectGame(UserConnection *user, PacketReader &reader) -> void;
	auto connectGameWorldFromViewAllCharacters(UserConnection *user, PacketReader &reader) -> void;
	auto checkCharacterName(UserConnection *user, PacketReader &reader) -> void;
	auto createCharacter(UserConnection *user, PacketReader &reader) -> void;
	auto deleteCharacter(UserConnection *user, PacketReader &reader) -> void;
	auto showAllCharacters(UserConnection *user) -> void;
	auto showCharacters(UserConnection *user) -> void;
	auto loadCharacter(Character &charc, const soci::row &row) -> void;
	auto loadEquips(player_id_t id, vector_t<CharEquip> &vec) -> void;
	auto createItem(item_id_t itemId, UserConnection *user, player_id_t charId, inventory_slot_t slot, slot_qty_t amount = 1) -> void;
	auto ownerCheck(UserConnection *user, player_id_t id) -> bool;
	auto nameTaken(const string_t &name) -> bool;
	auto nameInvalid(const string_t &name) -> bool;
}