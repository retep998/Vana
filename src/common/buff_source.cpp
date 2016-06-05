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
#include "buff_source.hpp"
#include "common/item_data_provider.hpp"
#include "common/skill_data_provider.hpp"

namespace vana {

auto buff_source::from_item(game_item_id item_id) -> buff_source {
	buff_source ret;
	ret.m_type = buff_source_type::item;
	ret.m_item_id = item_id;
	ret.m_skill_level = 1;
	return ret;
}

auto buff_source::from_skill(game_skill_id skillId, game_skill_level skill_level) -> buff_source {
	buff_source ret;
	ret.m_type = buff_source_type::skill;
	ret.m_skill_id = skillId;
	ret.m_skill_level = skill_level;
	return ret;
}

auto buff_source::from_mob_skill(game_mob_skill_id skillId, game_mob_skill_level skill_level) -> buff_source {
	buff_source ret;
	ret.m_type = buff_source_type::mob_skill;
	ret.m_mob_skill_id = skillId;
	ret.m_mob_skill_level = skill_level;
	return ret;
}

auto buff_source::get_type() const -> buff_source_type {
	return m_type;
}

auto buff_source::get_item_id() const -> game_item_id {
	return m_item_id;
}

auto buff_source::get_skill_id() const -> game_skill_id {
	return m_skill_id;
}

auto buff_source::get_skill_level() const -> game_skill_level {
	return m_skill_level;
}

auto buff_source::get_mob_skill_id() const -> game_mob_skill_id {
	return m_mob_skill_id;
}

auto buff_source::get_mob_skill_level() const -> game_mob_skill_level {
	return m_mob_skill_level;
}

auto buff_source::get_id() const -> int32_t {
	switch (m_type) {
		case buff_source_type::item: return m_item_id;
		case buff_source_type::skill: return m_skill_id;
		case buff_source_type::mob_skill: return m_mob_skill_id;
	}
	throw not_implemented_exception{"BuffSourceType"};
}

auto buff_source::get_item_data(const item_data_provider &provider) const -> const consume_info * const {
	return provider.get_consume_info(m_item_id);
}

auto buff_source::get_skill_data(const skill_data_provider &provider) const -> const skill_level_info * const {
	return provider.get_skill(m_skill_id, m_skill_level);
}

auto buff_source::get_mob_skill_data(const skill_data_provider &provider) const -> const mob_skill_level_info * const {
	return provider.get_mob_skill(m_mob_skill_id, m_mob_skill_level);
}

}