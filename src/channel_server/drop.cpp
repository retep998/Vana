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
#include "drop.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/drops_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"

namespace vana {
namespace channel_server {

drop::drop(game_map_id map_id, game_mesos mesos, const point &pos, game_player_id owner, bool player_drop) :
	m_owner{owner},
	m_map_id{map_id},
	m_mesos{mesos},
	m_player_drop{player_drop},
	m_pos{pos}
{
}

drop::drop(game_map_id map_id, const item &item, const point &pos, game_player_id owner, bool player_drop) :
	m_owner{owner},
	m_map_id{map_id},
	m_player_drop{player_drop},
	m_pos{pos},
	m_item{item}
{
}

auto drop::get_object_id() -> int32_t {
	return m_mesos > 0 ? m_mesos : m_item.get_id();
}

auto drop::get_amount() -> game_slot_qty {
	return m_item.get_amount();
}

auto drop::do_drop(const point &origin) -> void {
	set_dropped_at_time(vana::util::time::get_now());
	map *map = get_map();
	map->add_drop(this);

	if (!is_quest()) {
		if (!is_tradeable()) {
			map->send(packets::drops::show_drop(this, packets::drops::drop_spawn_types::disappear_during_drop, origin));
			this->remove_drop(false);
		}
		else {
			map->send(packets::drops::show_drop(this, packets::drops::drop_spawn_types::drop_animation, origin));
			map->send(packets::drops::show_drop(this, packets::drops::drop_spawn_types::show_drop, origin));
		}
	}
	else if (m_owner != 0) {
		if (auto player = channel_server::get_instance().get_player_data_provider().get_player(m_owner)) {
			if (player->get_map_id() == m_map_id) {
				player->send(packets::drops::show_drop(this, packets::drops::drop_spawn_types::drop_animation, origin));
				player->send(packets::drops::show_drop(this, packets::drops::drop_spawn_types::show_drop, origin));
			}
		}
	}
}

auto drop::show_drop(ref_ptr<player> player) -> void {
	if (is_quest() && player->get_id() != m_owner) {
		return;
	}
	player->send(packets::drops::show_drop(this, packets::drops::drop_spawn_types::show_existing, point{}));
}

auto drop::take_drop(ref_ptr<player> player, game_pet_id pet_id) -> void {
	map *map = get_map();
	map->remove_drop(m_id);

	if (pet_id == 0) {
		auto &packet = packets::drops::take_drop(player->get_id(), get_id());
		if (is_quest()) {
			player->send(packet);
		}
		else {
			map->send(packet);
		}
	}
	else {
		pet *pet = player->get_pets()->get_pet(pet_id);
		if (pet == nullptr || !pet->is_summoned()) {
			// nullptr = definitely hacking. Otherwise may be lag.
			return;
		}

		auto &packet = packets::drops::take_drop(player->get_id(), get_id(), pet->get_index().get());
		if (is_quest()) {
			player->send(packet);
		}
		else {
			map->send(packet);
		}

	}
	delete this;
}

auto drop::remove_drop(bool show_packet) -> void {
	map *map = get_map();
	map->remove_drop(m_id);
	if (show_packet) {
		map->send(packets::drops::remove_drop(get_id(), packets::drops::drop_despawn_types::expire, 0));
	}
	delete this;
}

auto drop::get_map() const -> map * {
	return maps::get_map(m_map_id);
}

}
}