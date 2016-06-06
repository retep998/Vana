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
#include "mystic_door.hpp"
#include "common/skill_data_provider.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mob.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {

mystic_door::mystic_door(ref_ptr<player> owner, game_map_id town_id, game_portal_id portal_id, const point &map_pos, const point &town_pos, bool is_displacement, seconds door_time) :
	m_map_id{owner->get_map_id()},
	m_map_pos{map_pos},
	m_town_id{town_id},
	m_town_pos{town_pos},
	m_owner_id{owner->get_id()},
	m_portal_id{portal_id},
	m_door_time{door_time}
{
}

mystic_door::mystic_door(game_player_id owner_id, game_map_id map_id, const point &map_pos, game_map_id town_id, game_portal_id portal_id, const point &town_pos, seconds door_time) :
	m_map_id{map_id},
	m_map_pos{map_pos},
	m_town_id{town_id},
	m_town_pos{town_pos},
	m_owner_id{owner_id},
	m_portal_id{portal_id},
	m_door_time{door_time}
{
}

auto mystic_door::get_owner_id() const -> game_player_id {
	return m_owner_id;
}

auto mystic_door::get_portal_id() const -> game_portal_id {
	return m_portal_id;
}

auto mystic_door::get_town_id() const -> game_map_id {
	return m_town_id;
}

auto mystic_door::get_map_id() const -> game_map_id {
	return m_map_id;
}

auto mystic_door::get_town() const -> map * {
	return maps::get_map(m_town_id);
}

auto mystic_door::get_map() const -> map * {
	return maps::get_map(m_map_id);
}

auto mystic_door::get_town_pos() const -> point {
	return m_town_pos;
}

auto mystic_door::get_map_pos() const -> point {
	return m_map_pos;
}

auto mystic_door::get_door_time() const -> seconds {
	return m_door_time;
}

auto mystic_door::with_new_portal(game_portal_id portal_id, const point &town_pos) const -> ref_ptr<mystic_door> {
	return make_ref_ptr<mystic_door>(
		m_owner_id,
		m_map_id,
		m_map_pos,
		m_town_id,
		portal_id,
		town_pos,
		m_door_time);
}

}
}