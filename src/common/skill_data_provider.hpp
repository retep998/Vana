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

#include "common/banish_field_info.hpp"
#include "common/mob_skill_level_info.hpp"
#include "common/morph_info.hpp"
#include "common/skill_constants.hpp"
#include "common/skill_level_info.hpp"
#include "common/types_temp.hpp"
#include <unordered_map>

namespace vana {
	class skill_data_provider {
	public:
		auto load_data() -> void;

		auto is_valid_skill(game_skill_id skill_id) const -> bool;
		auto get_max_level(game_skill_id skill_id) const -> uint8_t;
		auto get_skill(game_skill_id skill, game_skill_level level) const -> const skill_level_info * const;
		auto get_mob_skill(game_mob_skill_id skill, game_mob_skill_level level) const -> const mob_skill_level_info * const;
		auto get_banish_data(game_mob_id mob_id) const -> const banish_field_info * const;
		auto get_morph_data(game_morph_id morph) const -> const morph_info * const;
	private:
		auto load_player_skills() -> void;
		auto load_player_skill_levels() -> void;
		auto load_mob_skills() -> void;
		auto load_mob_summons() -> void;
		auto load_banish_data() -> void;
		auto load_morphs() -> void;

		hash_map<game_mob_skill_id, hash_map<game_mob_skill_level, mob_skill_level_info>> m_mob_skills;
		hash_map<game_skill_id, hash_map<game_skill_level, skill_level_info>> m_skill_levels;
		hash_map<game_skill_id, game_skill_level> m_skill_max_levels;
		hash_map<game_skill_id, banish_field_info> m_banish_info;
		hash_map<game_morph_id, morph_info> m_morph_info;
	};
}