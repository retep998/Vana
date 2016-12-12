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
	class item;
	class packet_builder;

	namespace channel_server {
		class move_path;
		class pet;
		class player;

		namespace packets {
			namespace pets {
				SPLIT_PACKET(pet_summoned, game_player_id player_id, pet *pet, bool kick = false, int8_t index = -1);
				SPLIT_PACKET(show_chat, game_player_id player_id, pet *pet, const string &message, int8_t act);
				SPLIT_PACKET(show_movement, game_player_id player_id, pet *pet, const move_path &path);
				PACKET(show_animation, game_player_id player_id, pet *pet, int8_t animation);
				PACKET(update_pet, pet *pet, item *pet_item);
				SPLIT_PACKET(level_up, game_player_id player_id, pet *pet);
				SPLIT_PACKET(change_name, game_player_id player_id, pet *pet);
				// TODO FIXME packet
				// This doesn't appear to be used anywhere, not sure if that's by mistake or not
				//auto show_pet(ref_ptr<player> player, pet *pet) -> void;
				PACKET(update_summoned_pets, ref_ptr<player> player);
				PACKET(blank_update);
				PACKET(add_info, pet *pet, item *pet_item);
			}
		}
	}
}