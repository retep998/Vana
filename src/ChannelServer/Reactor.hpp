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

#include "Common/Point.hpp"
#include "Common/Types.hpp"

namespace Vana {
	namespace ChannelServer {
		class Map;
		class Player;

		class Reactor {
			NONCOPYABLE(Reactor);
			NO_DEFAULT_CONSTRUCTOR(Reactor);
		public:
			Reactor(map_id_t mapId, reactor_id_t reactorId, const Point &pos, bool facesLeft);

			auto kill() -> void { m_alive = false; }
			auto revive() -> void { m_alive = true; }
			auto setState(int8_t state, bool sendPacket) -> void;
			auto setId(map_object_t id) -> void { m_id = id; }

			auto getState() const -> int8_t { return m_state; }
			auto getId() const -> map_object_t { return m_id; }
			auto getReactorId() const -> reactor_id_t { return m_reactorId; }
			auto getMapId() const -> map_id_t { return m_mapId; }
			auto isAlive() const -> bool { return m_alive; }
			auto getPos() const -> Point { return m_pos; }
			auto facesLeft() const -> bool { return m_facesLeft; }

			auto restore() -> void;
			auto drop(ref_ptr_t<Player> player) -> void;
			auto getMap() const -> Map *;
		private:
			bool m_alive = true;
			bool m_facesLeft = false;
			int8_t m_state = 0;
			map_object_t m_id = 0;
			reactor_id_t m_reactorId = 0;
			map_id_t m_mapId = 0;
			Point m_pos;
		};
	}
}