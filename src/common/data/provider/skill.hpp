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

#include "common/constant/skill.hpp"
#include "common/data/type/banish_field_info.hpp"
#include "common/data/type/mob_skill_level_info.hpp"
#include "common/data/type/morph_info.hpp"
#include "common/data/type/skill_level_info.hpp"
#include "common/types.hpp"
#include <unordered_map>

namespace vana {
	namespace data {
		namespace provider {
			class skill {
			public:
				auto load_data() -> void;

				auto is_valid_skill(game_skill_id skill_id) const -> bool;
				auto get_max_level(game_skill_id skill_id) const -> uint8_t;
				auto get_skill(game_skill_id skill, game_skill_level level) const -> const data::type::skill_level_info * const;
				auto get_mob_skill(game_mob_skill_id skill, game_mob_skill_level level) const -> const data::type::mob_skill_level_info * const;
				auto get_banish_data(game_mob_id mob_id) const -> const data::type::banish_field_info * const;
				auto get_morph_data(game_morph_id morph) const -> const data::type::morph_info * const;
			private:
				auto load_player_skills() -> void;
				auto load_player_skill_levels() -> void;
				auto load_mob_skills() -> void;
				auto load_mob_summons() -> void;
				auto load_banish_data() -> void;
				auto load_morphs() -> void;

				vector<pair<game_mob_skill_id, vector<data::type::mob_skill_level_info>>> m_mob_skills;
				vector<pair<game_skill_id, vector<data::type::skill_level_info>>> m_skill_levels;
				vector<pair<game_skill_id, game_skill_level>> m_skill_max_levels;
				vector<data::type::banish_field_info> m_banish_info;
				vector<data::type::morph_info> m_morph_info;
			};
		}
	}
}