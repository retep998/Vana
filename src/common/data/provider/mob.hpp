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

#include "common/data/type/mob_attack_info.hpp"
#include "common/data/type/mob_info.hpp"
#include "common/data/type/mob_skill_info.hpp"
#include "common/types.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace data {
		namespace provider {
			class mob {
			public:
				auto load_data() -> void;

				auto mob_exists(game_mob_id mob_id) const -> bool;
				auto get_mob_info(game_mob_id mob_id) const -> ref_ptr<const data::type::mob_info>;
				auto get_mob_attack(game_mob_id mob_id, uint8_t index) const -> const data::type::mob_attack_info * const;
				auto get_mob_skill(game_mob_id mob_id, uint8_t index) const -> const data::type::mob_skill_info * const;
				auto get_skills(game_mob_id mob_id) const -> const vector<data::type::mob_skill_info> &;
			private:
				auto load_mobs() -> void;
				auto load_attacks() -> void;
				auto load_skills() -> void;
				auto load_summons() -> void;

				vector<ref_ptr<data::type::mob_info>> m_mob_info;
				vector<pair<game_mob_id, vector<data::type::mob_attack_info>>> m_attacks;
				vector<pair<game_mob_id, vector<data::type::mob_skill_info>>> m_skills;
			};
		}
	}
}