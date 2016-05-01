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
#include "ChannelServer/MovableLife.hpp"

namespace Vana {
	namespace ChannelServer {
		class Summon : public MovableLife {
			NONCOPYABLE(Summon);
			NO_DEFAULT_CONSTRUCTOR(Summon);
		public:
			enum MovementPatterns : int8_t {
				Static = 0,
				Follow = 1,
				FlyClose = 3,
				FlyFar = 4,
			};
			enum ActionPatterns : int8_t {
				DoNothing = 0,
				Attack = 1,
				Beholder = 2,
			};

			Summon(map_object_t id, skill_id_t summonId, skill_level_t level, bool isFacingLeft, const Point &position, foothold_id_t foothold = 0);

			auto getId() -> summon_id_t { return m_id; }
			auto getSkillId() -> skill_id_t { return m_summonId; }
			auto getSkillLevel() -> skill_level_t { return m_level; }
			auto getMovementType() -> uint8_t { return m_movementType; }
			auto getActionType() -> uint8_t { return m_actionType; }
			auto getHp() -> int32_t { return m_hp; }
			auto doDamage(damage_t damage) -> void { m_hp -= damage; }
		private:
			skill_level_t m_level = 0;
			uint8_t m_movementType = 0;
			uint8_t m_actionType = 0;
			summon_id_t m_id = 0;
			skill_id_t m_summonId = 0;
			int32_t m_hp = 0;
		};
	}
}