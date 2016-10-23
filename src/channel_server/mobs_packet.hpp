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
#include "common/types.hpp"
#include <vector>

namespace vana {
	struct point;

	namespace channel_server {
		class mob;
		class move_path;
		class player;
		struct status_info;

		namespace packets {
			namespace mobs {
				PACKET(spawn_mob, ref_ptr<mob> value, int8_t summon_effect, ref_ptr<mob> owner = nullptr, mob_spawn_type spawn = mob_spawn_type::existing);
				PACKET(request_control, ref_ptr<mob> value, mob_spawn_type spawn);
				PACKET(mob_packet, ref_ptr<mob> value, int8_t summon_effect, ref_ptr<mob> owner, mob_spawn_type spawn);
				PACKET(end_control_mob, game_map_object map_mob_id);
				PACKET(move_mob_response, game_map_object map_mob_id, int16_t move_id, bool skill_possible, int32_t mp, game_mob_skill_id skill, game_mob_skill_level level);
				PACKET(move_mob, game_map_object map_mob_id, bool skill_possible, int8_t raw_action, game_mob_skill_id skill, game_mob_skill_level level, int16_t option, const move_path &path);
				PACKET(heal_mob, game_map_object map_mob_id, int32_t amount);
				PACKET(hurt_mob, game_map_object map_mob_id, game_damage amount);
				PACKET(damage_friendly_mob, ref_ptr<mob> value, game_damage damage);
				PACKET(apply_status, game_map_object map_mob_id, int32_t status_mask, const vector<status_info> &info, int16_t delay, const vector<int32_t> &reflection);
				PACKET(remove_status, game_map_object map_mob_id, int32_t status);
				PACKET(show_hp, game_map_object map_mob_id, int8_t percentage);
				PACKET(show_boss_hp, ref_ptr<mob> value);
				PACKET(die_mob, game_map_object map_mob_id, int8_t death = 1);
				PACKET(show_spawn_effect, int8_t summon_effect, const point &pos);
			}
		}
	}
}