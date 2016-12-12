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
#include <string>

namespace vana {
	struct point;

	namespace channel_server {
		class mist;
		class mystic_door;
		class player;

		namespace packets {
			namespace map {

				enum class portal_blocked_reason : int8_t {
					no_reason = 0,
					closed_for_now = 1, // Its a blocked map or portal is not enabled
					cannot_go = 2, // Scroll used is not in same 'map category'
					unable_to_approach_force_of_ground = 3,
					no_teleport_to_or_on_this_map = 4,
					unable_to_approach_force_of_ground_2 = 5, // Duplicate
					cashshop_unavailable = 6,
				};

				PACKET(player_packet, ref_ptr<vana::channel_server::player> player);
				PACKET(remove_player, game_player_id player_id);
				PACKET(change_map, ref_ptr<vana::channel_server::player> player, bool spawn_by_position, const point &spawn_position);
				PACKET(portal_blocked, portal_blocked_reason reason);
				PACKET(show_clock, int8_t hour, int8_t min, int8_t sec);
				PACKET(show_timer, const seconds &sec);
				PACKET(force_map_equip);
				PACKET(show_event_instructions);
				PACKET(spawn_mist, mist *mist, bool map_entry);
				PACKET(remove_mist, game_map_object id);
				PACKET(spawn_door, ref_ptr<mystic_door> door, bool is_inside_town, bool already_open);
				PACKET(remove_door, ref_ptr<mystic_door> door, bool is_fade);
				PACKET(spawn_portal, ref_ptr<mystic_door> door, game_map_id calling_map);
				PACKET(remove_portal);
				PACKET(instant_warp, game_portal_id portal_id);
				PACKET(boat_dock_update, bool docked, int8_t ship_kind);
				PACKET(change_weather, bool admin_weather, game_item_id item_id = 0, const string &message = "");
				PACKET(upgrade_tomb_effect, game_player_id player_id, game_item_id item_id, int32_t x, int32_t y);
			}
		}
	}
}