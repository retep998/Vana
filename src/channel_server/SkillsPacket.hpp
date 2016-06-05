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

#include "common_temp/PacketBuilder.hpp"
#include "common_temp/SplitPacketBuilder.hpp"
#include "common_temp/Types.hpp"

namespace vana {
	struct charge_or_stationary_skill_data;
	struct player_skill_info;

	namespace channel_server {
		class player;

		namespace packets {
			namespace skills {
				PACKET(add_skill, game_skill_id skill_id, const player_skill_info &skill_info);
				SPLIT_PACKET(show_skill, game_player_id player_id, game_skill_id skill_id, game_skill_level level, uint8_t direction, bool party = false, bool self = false);
				PACKET(heal_hp, game_health hp);
				SPLIT_PACKET(show_skill_effect, game_player_id player_id, game_skill_id skill_id);
				SPLIT_PACKET(show_charge_or_stationary_skill, game_player_id player_id, const charge_or_stationary_skill_data &info);
				SPLIT_PACKET(end_charge_or_stationary_skill, game_player_id player_id, const charge_or_stationary_skill_data &info);
				SPLIT_PACKET(show_magnet_success, game_map_object map_mob_id, uint8_t success);
				PACKET(send_cooldown, game_skill_id skill_id, seconds time);
				SPLIT_PACKET(show_berserk, game_player_id player_id, game_skill_level level, bool on);
			}
		}
	}
}