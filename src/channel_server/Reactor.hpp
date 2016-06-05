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

		class reactor {
			NONCOPYABLE(reactor);
			NO_DEFAULT_CONSTRUCTOR(reactor);
		public:
			reactor(game_map_id map_id, game_reactor_id reactor_id, const point &pos, bool faces_left);

			auto kill() -> void { m_alive = false; }
			auto revive() -> void { m_alive = true; }
			auto set_state(int8_t state, bool send_packet) -> void;
			auto set_id(game_map_object id) -> void { m_id = id; }

			auto get_state() const -> int8_t { return m_state; }
			auto get_id() const -> game_map_object { return m_id; }
			auto get_reactor_id() const -> game_reactor_id { return m_reactor_id; }
			auto get_map_id() const -> game_map_id { return m_map_id; }
			auto is_alive() const -> bool { return m_alive; }
			auto get_pos() const -> point { return m_pos; }
			auto faces_left() const -> bool { return m_faces_left; }

			auto restore() -> void;
			auto drop(ref_ptr<player> player) -> void;
			auto get_map() const -> map *;
		private:
			bool m_alive = true;
			bool m_faces_left = false;
			int8_t m_state = 0;
			game_map_object m_id = 0;
			game_reactor_id m_reactor_id = 0;
			game_map_id m_map_id = 0;
			point m_pos;
		};
	}
}