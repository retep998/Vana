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

#include "common/data/type/buff.hpp"
#include "common/data/type/buff_info.hpp"
#include "common/data/type/buff_info_by_effect.hpp"
#include "common/types.hpp"
#include <unordered_map>
#include <vector>

namespace vana {
	namespace data {
		namespace type {
			class buff_source;
			struct consume_info;
		}

		namespace provider {
			class buff {
			public:
				auto load_data() -> void;
				auto add_item_info(game_item_id item_id, const data::type::consume_info &cons) -> void;

				auto is_buff(const data::type::buff_source &source) const -> bool;
				auto is_debuff(const data::type::buff_source &source) const -> bool;
				auto get_info(const data::type::buff_source &source) const -> const data::type::buff &;
				auto get_buffs_by_effect() const -> const data::type::buff_info_by_effect &;
			private:
				auto process_skills(data::type::buff value, const init_list<game_skill_id> &skills) -> void;
				vector<pair<game_skill_id, data::type::buff>> m_buffs;
				vector<pair<game_item_id, data::type::buff>> m_items;
				vector<pair<game_mob_skill_id, data::type::buff>> m_mob_skill_info;
				data::type::buff_info_by_effect m_basics;
			};
		}
	}
}