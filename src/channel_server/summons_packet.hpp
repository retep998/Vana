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

namespace vana {
	struct point;

	namespace channel_server {
		class move_path;
		class player;
		class summon;

		namespace packets {
			SPLIT_PACKET(show_summon, game_player_id player_id, summon *summon, bool is_map_entry = true);
			PACKET(move_summon, game_player_id player_id, summon *summon, const move_path &path);
			SPLIT_PACKET(remove_summon, game_player_id player_id, summon *summon, int8_t message);
			SPLIT_PACKET(damage_summon, game_player_id player_id, game_summon_id summon_id, int8_t unk, game_damage damage, game_map_object mob_id);
			PACKET(summon_skill, game_player_id player_id, game_skill_id skill_id, uint8_t display, game_skill_level level);
			SPLIT_PACKET(summon_skill_effect, game_player_id player_id, game_skill_id skill_id, uint8_t display, game_skill_level level);
		}
	}
}