/*
Copyright (C) 2008-2016 Vana Development Team

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

#include "common/types_temp.hpp"
#include <string>
#include <vector>

namespace vana {
	class packet_reader;
	class soci::row;

	namespace login_server {
		class user;

		struct char_equip {
			game_item_id id = 0;
			game_inventory_slot slot = 0;
		};

		struct character {
			int8_t pos = 0;
			game_gender_id gender = 0;
			game_skin_id skin = 0;
			game_player_level level = 0;
			game_job_id job = 0;
			game_stat str = 0;
			game_stat dex = 0;
			game_stat intt = 0;
			game_stat luk = 0;
			game_health hp = 0;
			game_health mhp = 0;
			game_health mp = 0;
			game_health mmp = 0;
			game_stat ap = 0;
			game_stat sp = 0;
			game_fame fame = 0;
			game_map_id map = 0;
			game_face_id face = 0;
			game_hair_id hair = 0;
			game_experience exp = 0;
			int32_t world_rank_change = 0;
			int32_t job_rank_change = 0;
			game_player_id id = 0;
			uint32_t world_rank = 0;
			uint32_t job_rank = 0;
			string name;
			vector<char_equip> equips;
		};

		namespace characters {
			auto connect_game(ref_ptr<user> user_value, game_player_id char_id) -> void;
			auto connect_game(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto connect_game_world_from_view_all_characters(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto check_character_name(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto create_character(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto delete_character(ref_ptr<user> user_value, packet_reader &reader) -> void;
			auto show_all_characters(ref_ptr<user> user_value) -> void;
			auto show_characters(ref_ptr<user> user_value) -> void;
			auto load_character(character &charc, const soci::row &row) -> void;
			auto load_equips(game_player_id id, vector<char_equip> &vec) -> void;
			auto create_item(game_item_id item_id, ref_ptr<user> user_value, game_player_id char_id, game_inventory_slot slot, game_slot_qty amount = 1) -> void;
			auto owner_check(ref_ptr<user> user_value, game_player_id id) -> bool;
			auto name_taken(const string &name) -> bool;
			auto name_invalid(const string &name) -> bool;
		}
	}
}