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
#include "reactor_handler.hpp"
#include "common/file_utilities.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/packet_reader.hpp"
#include "common/point.hpp"
#include "common/reactor_data_provider.hpp"
#include "common/script_data_provider.hpp"
#include "common/time_utilities.hpp"
#include "common/timer.hpp"
#include "common/timer_thread.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/drop.hpp"
#include "channel_server/lua_reactor.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/reactor.hpp"
#include "channel_server/reactor_packet.hpp"
#include <functional>
#include <iostream>
#include <sstream>

namespace vana {
namespace channel_server {

auto reactor_handler::hit_reactor(ref_ptr<player> player, packet_reader &reader) -> void {
	game_map_object id = map::make_reactor_id(reader.get<game_map_object>());

	map *map = player->get_map();
	reactor *reactor = map->get_reactor(id);

	if (reactor != nullptr && reactor->is_alive()) {
		auto &data = channel_server::get_instance().get_reactor_data_provider().get_reactor_data(reactor->get_reactor_id(), true);
		if (reactor->get_state() < data.max_states - 1) {
			const auto &reactor_event = data.states.at(reactor->get_state())[0]; // There's only one way to hit something
			if (reactor_event.next_state < data.max_states - 1) {
				if (reactor_event.type == 100) {
					return;
				}
				map->send(packets::trigger_reactor(reactor));
				reactor->set_state(reactor_event.next_state, true);
				return;
			}
			else {
				auto &channel = channel_server::get_instance();
				string filename = channel.get_script_data_provider().get_script(&channel, reactor->get_reactor_id(), script_types::reactor);

				if (utilities::file::exists(filename)) {
					lua_reactor{filename, player->get_id(), id, reactor->get_map_id()};
				}
				else {
					// Default action of dropping an item
					reactor->drop(player);
				}

				reactor->set_state(reactor_event.next_state, false);
				reactor->kill();
				map->remove_reactor(id);
				map->send(packets::destroy_reactor(reactor));
			}
		}
	}
}

auto reactor_handler::touch_reactor(ref_ptr<player> player, packet_reader &reader) -> void {
	size_t id = map::make_reactor_id(reader.get<game_map_object>());
	bool is_touching = reader.get<bool>();

	map *map = player->get_map();
	reactor *reactor = map->get_reactor(id);

	if (reactor != nullptr && reactor->is_alive()) {
		int8_t new_state = reactor->get_state() + (is_touching ? 1 : -1);
		map->send(packets::trigger_reactor(reactor));
		reactor->set_state(new_state, true);
	}
}

struct reaction {
	auto operator()(const time_point &now) -> void {
		reactor->set_state(state, true);
		drop->remove_drop();
		auto &channel = channel_server::get_instance();
		string filename = channel.get_script_data_provider().get_script(&channel, reactor->get_reactor_id(), script_types::reactor);
		// TODO FIXME reactor
		// Not sure if this reactor identifier dispatch is correct
		lua_reactor{filename, player->get_id(), static_cast<game_map_object>(map::make_reactor_id(reactor->get_id())), reactor->get_map_id()};
	}

	reactor *reactor = nullptr;
	drop *drop = nullptr;
	ref_ptr<player> player = nullptr;
	int8_t state = 0;
};

auto reactor_handler::check_drop(ref_ptr<player> player, drop *drop) -> void {
	reactor *reactor;
	map *map = drop->get_map();
	for (size_t i = 0; i < map->get_num_reactors(); ++i) {
		reactor = map->get_reactor(i);
		auto &data = channel_server::get_instance().get_reactor_data_provider().get_reactor_data(reactor->get_reactor_id(), true);
		if (reactor->get_state() < data.max_states - 1) {
			for (const auto &reactor_event : data.states.at(reactor->get_state())) {
				if (reactor_event.type == 100 && drop->get_object_id() == reactor_event.item_id) {
					if (reactor_event.dimensions.move(reactor->get_pos()).contains(drop->get_pos())) {
						reaction reaction;
						reaction.reactor = reactor;
						reaction.drop = drop;
						reaction.player = player;
						reaction.state = reactor_event.next_state;

						vana::timer::id id{timer_type::reaction_timer, drop->get_id()};
						vana::timer::timer::create(reaction, id, nullptr, seconds{3});
					}
					return;
				}
			}
		}
	}
}

auto reactor_handler::check_loot(drop *drop) -> void {
	vana::timer::id id{timer_type::reaction_timer, drop->get_id()};
	vana::timer::timer_thread::get_instance().get_timer_container()->remove_timer(id);
}

}
}