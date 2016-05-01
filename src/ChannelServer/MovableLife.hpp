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

#include "Common/MapPosition.hpp"
#include "Common/Point.hpp"

namespace Vana {
	namespace ChannelServer {
		// A base class for player, mobs that can move
		class MovableLife {
			NONCOPYABLE(MovableLife);
			NO_DEFAULT_CONSTRUCTOR(MovableLife);
		public:
			MovableLife(foothold_id_t foothold, const Point &pos, int8_t stance) :
				m_stance{stance},
				m_foothold{foothold},
				m_pos{pos}
			{
			}

			virtual ~MovableLife() = default;

			auto isFacingRight() const -> bool { return m_stance % 2 == 0; }
			auto isFacingLeft() const -> bool { return m_stance % 2 == 1; }
			auto getStance() const -> int8_t { return m_stance; }
			auto getFoothold() const -> foothold_id_t { return m_foothold; }
			virtual auto getPos() const -> Point { return m_pos; }
			auto getMapPosition() const -> MapPosition { return MapPosition{getPos(), m_foothold}; }

			auto setPos(const Point &val) -> void { m_pos = val; }
			auto resetMovement(foothold_id_t foothold, const Point &pos, int8_t stance) -> void {
				m_stance = stance;
				m_foothold = foothold;
				m_pos = pos;
			}
		protected:
			auto setStance(int8_t val) -> void { m_stance = val; }
			auto setFoothold(foothold_id_t val) -> void { m_foothold = val; }

			int8_t m_stance = 0;
			foothold_id_t m_foothold = 0;
			Point m_pos;
		};
	}
}