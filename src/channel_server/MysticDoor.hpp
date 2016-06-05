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

#include "common/point_temp.hpp"
#include "common/types_temp.hpp"

namespace vana {
	namespace channel_server {
		class map;
		class player;

		class mystic_door {
			NONCOPYABLE(mystic_door);
		public:
			mystic_door(ref_ptr<player> owner, game_map_id town_id, game_portal_id portal_id, const point &map_pos, const point &town_pos, bool is_displacement, seconds door_time);
			mystic_door(game_player_id owner_id, game_map_id map_id, const point &map_pos, game_map_id town_id, game_portal_id portal_id, const point &town_pos, seconds door_time);

			static const game_portal_id portal_id = 80;

			auto get_owner_id() const -> game_player_id;
			auto get_town_id() const -> game_map_id;
			auto get_map_id() const -> game_map_id;
			auto get_town() const -> map *;
			auto get_map() const -> map *;
			auto get_town_pos() const -> point;
			auto get_map_pos() const -> point;
			auto get_portal_id() const -> game_portal_id;
			auto get_door_time() const -> seconds;
			auto with_new_portal(game_portal_id portal_id, const point &town_pos) const -> ref_ptr<mystic_door>;
		private:
			game_map_id m_town_id = 0;
			game_map_id m_map_id = 0;
			game_portal_id m_portal_id = 0;
			game_player_id m_owner_id = 0;
			seconds m_door_time = seconds{0};
			point m_map_pos;
			point m_town_pos;
		};
	}
}