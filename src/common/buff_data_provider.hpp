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

#include "common/buff_temp.hpp"
#include "common/buff_info.hpp"
#include "common/buff_info_by_effect.hpp"
#include "common/types_temp.hpp"
#include <unordered_map>
#include <vector>

namespace vana {
	class buff_source;
	struct consume_info;

	class buff_data_provider {
	public:
		auto load_data() -> void;
		auto add_item_info(game_item_id item_id, const consume_info &cons) -> void;

		auto is_buff(const buff_source &source) const -> bool;
		auto is_debuff(const buff_source &source) const -> bool;
		auto get_info(const buff_source &source) const -> const buff &;
		auto get_buffs_by_effect() const -> const buff_info_by_effect &;
	private:
		auto process_skills(buff value, const init_list<game_skill_id> &skills) -> void;
		hash_map<game_skill_id, buff> m_buffs;
		hash_map<game_item_id, buff> m_items;
		hash_map<game_mob_skill_id, buff> m_mob_skill_info;
		buff_info_by_effect m_basics;
	};
}