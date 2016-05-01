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
#include "Common/Rect.hpp"
#include "Common/Types.hpp"

namespace Vana {
	struct SkillLevelInfo;
	struct MobSkillLevelInfo;

	namespace ChannelServer {
		class Map;
		class Mob;
		class Player;

		class Mist {
			NONCOPYABLE(Mist);
			NO_DEFAULT_CONSTRUCTOR(Mist);
		public:
			Mist(map_id_t mapId, ref_ptr_t<Player> owner, seconds_t time, const Rect &area, skill_id_t skillId, skill_level_t level, bool isPoison = false);
			Mist(map_id_t mapId, Mob *owner, seconds_t time, const Rect &area, mob_skill_id_t skillId, mob_skill_level_t level);

			auto setId(mist_id_t id) -> void { m_id = id; }
			auto getSkillLevel() const -> skill_level_t { return m_level; }
			auto getTime() const -> seconds_t { return m_time; }
			auto getDelay() const -> int16_t { return m_delay; }
			auto getSkillId() const -> skill_id_t { return m_skill; }
			auto getId() const -> mist_id_t { return m_id; }
			auto getOwnerId() const -> int32_t { return m_ownerId; }
			auto isMobMist() const -> bool { return m_isMobMist; }
			auto isPoison() const -> bool { return m_poison; }
			auto getArea() const -> Rect { return m_area; }
			auto getMap() const -> Map *;
		private:
			bool m_isMobMist = true;
			bool m_poison = true;
			skill_level_t m_level = 0;
			int16_t m_delay = 0;
			mist_id_t m_id = 0;
			skill_id_t m_skill = 0;
			map_id_t m_ownerMap = 0;
			int32_t m_ownerId = 0;
			seconds_t m_time = seconds_t{0};
			Rect m_area;
		};
	}
}