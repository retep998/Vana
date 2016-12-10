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
#include "map_factory.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/string.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maple_tvs.hpp"
#include <utility>

namespace vana {
namespace channel_server {

auto map_factory::get_map(game_map_id map_id) -> map * {
	owned_lock<mutex> l{m_load_mutex};
	for (auto &map : m_maps) {
		if (map->get_id() == map_id) {
			return map;
		}
	}

	auto info = channel_server::get_instance().get_map_data_provider().get_map(map_id);
	map *map = new vana::channel_server::map{info, map_id};
	m_maps.push_back(map);
	return map;
}

auto map_factory::unload_map(game_map_id map_id) -> void {
	owned_lock<mutex> l{m_load_mutex};
	for (size_t i = 0; i < m_maps.size(); i++) {
		auto map = m_maps[i];
		if (map->get_id() == map_id) {
			// We could run into a situation where unload_map has been called while a lock is out on get_map
			// Reasons for this might be: Starting an instance, adding a player
			// Once the code here advances, we have to ensure that we're doing the right thing, otherwise there could be a serious problem
			if (map->get_num_players() == 0 && map->get_instance() == nullptr) {
				delete map;
				m_maps.erase(std::begin(m_maps) + i);
			}
			break;
		}
	}
}

}
}