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

#include "common/types_temp.hpp"
#include "channel_server/MovableLife.hpp"

namespace vana {
	namespace channel_server {
		class summon : public movable_life {
			NONCOPYABLE(summon);
			NO_DEFAULT_CONSTRUCTOR(summon);
		public:
			enum movement_patterns : int8_t {
				fixed = 0,
				follow = 1,
				fly_close = 3,
				fly_far = 4,
			};
			enum action_patterns : int8_t {
				do_nothing = 0,
				attack = 1,
				beholder = 2,
			};

			summon(game_map_object id, game_skill_id summon_id, game_skill_level level, bool is_facing_left, const point &position, game_foothold_id foothold = 0);

			auto get_id() -> game_summon_id { return m_id; }
			auto get_skill_id() -> game_skill_id { return m_summon_id; }
			auto get_skill_level() -> game_skill_level { return m_level; }
			auto get_movement_type() -> uint8_t { return m_movement_type; }
			auto get_action_type() -> uint8_t { return m_action_type; }
			auto get_hp() -> int32_t { return m_hp; }
			auto do_damage(game_damage damage) -> void { m_hp -= damage; }
		private:
			game_skill_level m_level = 0;
			uint8_t m_movement_type = 0;
			uint8_t m_action_type = 0;
			game_summon_id m_id = 0;
			game_skill_id m_summon_id = 0;
			int32_t m_hp = 0;
		};
	}
}