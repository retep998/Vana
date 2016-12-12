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

#include "common/map_position.hpp"
#include "common/point.hpp"
#include "channel_server/move_path.hpp"

namespace vana {
	namespace channel_server {
		class move_path;

		// A base class for player, mobs that can move
		class movable_life {
			NONCOPYABLE(movable_life);
			NO_DEFAULT_CONSTRUCTOR(movable_life);
		public:
			movable_life(game_foothold_id foothold, const point &pos, int8_t stance) :
				m_stance{stance},
				m_foothold{foothold},
				m_pos{pos}
			{
			}

			virtual ~movable_life() = default;

			auto is_facing_right() const -> bool { return m_stance % 2 == 0; }
			auto is_facing_left() const -> bool { return m_stance % 2 == 1; }
			auto get_stance() const -> int8_t { return m_stance; }
			auto get_foothold() const -> game_foothold_id { return m_foothold; }
			virtual auto get_pos() const -> point { return m_pos; }
			auto get_map_position() const -> map_position { return map_position{get_pos(), m_foothold}; }

			auto set_pos(const point &val) -> void { m_pos = val; }
			auto reset_movement(game_foothold_id foothold, const point &pos, int8_t stance) -> void {
				m_stance = stance;
				m_foothold = foothold;
				m_pos = pos;
			}

			auto reset_from_move_path(const move_path &path) -> void {
				reset_movement(path.get_new_foothold(), path.get_new_position(), path.get_new_stance());
			}
		protected:
			auto set_stance(int8_t val) -> void { m_stance = val; }
			auto set_foothold(game_foothold_id val) -> void { m_foothold = val; }

			int8_t m_stance = 0;
			game_foothold_id m_foothold = 0;
			point m_pos;
		};
	}
}