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
#include "reactor_temp.hpp"
#include "channel_server/drop_handler.hpp"
#include "channel_server/maps_temp.hpp"
#include "channel_server/player_temp.hpp"
#include "channel_server/reactor_packet.hpp"

namespace vana {
namespace channel_server {

reactor::reactor(game_map_id map_id, game_reactor_id reactor_id, const point &pos, bool faces_left) :
	m_reactor_id{reactor_id},
	m_map_id{map_id},
	m_pos{pos},
	m_faces_left{faces_left}
{
	maps::get_map(map_id)->add_reactor(this);
}

auto reactor::set_state(int8_t state, bool send_packet) -> void {
	m_state = state;
	if (send_packet) {
		get_map()->send(packets::trigger_reactor(this));
	}
}

auto reactor::restore() -> void {
	revive();
	set_state(0, false);
	get_map()->send(packets::spawn_reactor(this));
}

auto reactor::drop(ref_ptr<player> player) -> void {
	drop_handler::do_drops(player->get_id(), m_map_id, 0, m_reactor_id, m_pos, false, false);
}

auto reactor::get_map() const -> map * {
	return maps::get_map(m_map_id);
}

}
}