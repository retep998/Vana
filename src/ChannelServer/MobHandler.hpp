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

#include "Common/Types.hpp"

namespace vana {
	class packet_reader;
	struct mob_skill_level_info;

	namespace channel_server {
		class mob;
		class player;
		namespace mob_handler {
			auto handle_mob_status(game_player_id player_id, ref_ptr<mob> mob, game_skill_id skill_id, game_skill_level level, game_item_id weapon, int8_t hits, game_damage damage = 0) -> int32_t;
			auto handle_bomb(ref_ptr<player> player, packet_reader &reader) -> void;
			auto monster_control(ref_ptr<player> player, packet_reader &reader) -> void;
			auto friendly_damaged(ref_ptr<player> player, packet_reader &reader) -> void;
			auto handle_turncoats(ref_ptr<player> player, packet_reader &reader) -> void;
		}
	}
}