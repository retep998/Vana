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

#include "common/packet_builder.hpp"
#include "common/split_packet_builder.hpp"
#include "common/types.hpp"
#include <string>

namespace vana {
	class packet_reader;
	struct attack_data;
	struct return_damage_data;
	struct wide_point;

	namespace channel_server {
		class move_path;
		class player;

		namespace packets {
			namespace players {
				SPLIT_PACKET(show_moving, game_player_id player_id, const move_path &move_path);
				SPLIT_PACKET(face_expression, game_player_id player_id, int32_t face);
				PACKET(show_chat, game_player_id player_id, bool is_gm, const string &msg, bool bubble_only);
				SPLIT_PACKET(damage_player, game_player_id player_id, game_damage dmg, game_mob_id mob, uint8_t hit, int8_t type, uint8_t stance, game_skill_id no_damage_skill, const return_damage_data &pgmr);
				PACKET(show_info, ref_ptr<vana::channel_server::player> get_info, bool is_self);
				PACKET(find_player, const string &name, opt_int32_t map, uint8_t is = 0, bool is_channel = false);
				PACKET(whisper_player, const string &whisperer_name, game_channel_id channel, const string &message);
				SPLIT_PACKET(use_melee_attack, game_player_id player_id, game_skill_id mastery_skill_id, game_skill_level mastery_level, const attack_data &attack);
				SPLIT_PACKET(use_ranged_attack, game_player_id player_id, game_skill_id mastery_skill_id, game_skill_level mastery_level, const attack_data &attack);
				SPLIT_PACKET(use_spell_attack, game_player_id player_id, const attack_data &attack);
				SPLIT_PACKET(use_summon_attack, game_player_id player_id, const attack_data &attack);
				SPLIT_PACKET(use_bomb_attack, game_player_id player_id, game_charge_time charge_time, game_skill_id skill_id, const wide_point &pos);
				SPLIT_PACKET(use_energy_charge_attack, game_player_id player_id, game_skill_id mastery_skill_id, game_skill_level mastery_level, const attack_data &attack);
			}
		}
	}
}