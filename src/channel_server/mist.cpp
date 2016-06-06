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
#include "mist.hpp"
#include "common/skill_data_provider.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mob.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {

mist::mist(game_map_id map_id, ref_ptr<player> owner, seconds time, const rect &area, game_skill_id skill_id, game_skill_level level, bool is_poison) :
	m_owner_map{map_id},
	m_owner_id{owner->get_id()},
	m_skill{skill_id},
	m_level{level},
	m_area{area},
	m_time{time},
	m_delay{8},
	m_is_mob_mist{false},
	m_poison{is_poison}
{
	maps::get_map(map_id)->add_mist(this);
}

mist::mist(game_map_id map_id, mob *owner, seconds time, const rect &area, game_mob_skill_id skill_id, game_mob_skill_level level) :
	m_owner_map{map_id},
	m_owner_id{owner->get_map_mob_id()},
	m_skill{skill_id},
	m_level{level},
	m_area{area},
	m_time{time}
{
	maps::get_map(map_id)->add_mist(this);
}

auto mist::get_map() const -> map * {
	return maps::get_map(m_owner_map);
}

}
}