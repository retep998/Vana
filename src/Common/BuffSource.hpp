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

#include "Common/BuffSourceType.hpp"
#include "Common/Types.hpp"

namespace vana {
	class item_data_provider;
	class skill_data_provider;
	struct consume_info;
	struct mob_skill_level_info;
	struct skill_level_info;

	class buff_source {
	public:
		buff_source() = default;

		auto get_type() const -> buff_source_type;
		auto get_item_id() const -> game_item_id;
		auto get_skill_id() const -> game_skill_id;
		auto get_skill_level() const -> game_skill_level;
		auto get_mob_skill_id() const -> game_mob_skill_id;
		auto get_mob_skill_level() const -> game_mob_skill_level;
		auto get_id() const -> int32_t;
		auto get_item_data(const item_data_provider &provider) const -> const consume_info * const;
		auto get_skill_data(const skill_data_provider &provider) const -> const skill_level_info * const;
		auto get_mob_skill_data(const skill_data_provider &provider) const -> const mob_skill_level_info * const;

		static auto from_item(game_item_id item_id) -> buff_source;
		static auto from_skill(game_skill_id skill_id, game_skill_level skill_level) -> buff_source;
		static auto from_mob_skill(game_mob_skill_id skill_id, game_mob_skill_level skill_level) -> buff_source;
	private:
		buff_source_type m_type = buff_source_type::none;
		game_skill_id m_skill_id = 0;
		game_skill_level m_skill_level = 0;
		game_mob_skill_id m_mob_skill_id = 0;
		game_mob_skill_level m_mob_skill_level = 0;
		game_item_id m_item_id = 0;
	};
}