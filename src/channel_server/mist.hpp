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

#include "common/point.hpp"
#include "common/rect.hpp"
#include "common/types.hpp"

namespace vana {
	struct skill_level_info;
	struct mob_skill_level_info;

	namespace channel_server {
		class map;
		class mob;
		class player;

		class mist {
			NONCOPYABLE(mist);
			NO_DEFAULT_CONSTRUCTOR(mist);
		public:
			mist(game_map_id map_id, ref_ptr<player> owner, seconds time, const rect &area, game_skill_id skill_id, game_skill_level level, bool is_poison = false);
			mist(game_map_id map_id, mob *owner, seconds time, const rect &area, game_mob_skill_id skill_id, game_mob_skill_level level);

			auto set_id(game_mist_id id) -> void { m_id = id; }
			auto get_skill_level() const -> game_skill_level { return m_level; }
			auto get_time() const -> seconds { return m_time; }
			auto get_delay() const -> int16_t { return m_delay; }
			auto get_skill_id() const -> game_skill_id { return m_skill; }
			auto get_id() const -> game_mist_id { return m_id; }
			auto get_owner_id() const -> int32_t { return m_owner_id; }
			auto is_mob_mist() const -> bool { return m_is_mob_mist; }
			auto is_poison() const -> bool { return m_poison; }
			auto get_area() const -> rect { return m_area; }
			auto get_map() const -> map *;
		private:
			bool m_is_mob_mist = true;
			bool m_poison = true;
			game_skill_level m_level = 0;
			int16_t m_delay = 0;
			game_mist_id m_id = 0;
			game_skill_id m_skill = 0;
			game_map_id m_owner_map = 0;
			int32_t m_owner_id = 0;
			seconds m_time = seconds{0};
			rect m_area;
		};
	}
}