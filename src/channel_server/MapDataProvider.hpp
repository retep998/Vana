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

#include "common/point_temp.hpp"
#include "common/rect_temp.hpp"
#include "common/types_temp.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace channel_server {
		class map;
		class player;

		struct time_mob {
			int8_t start_hour = 0;
			int8_t end_hour = 0;
			game_mob_id id = 0;
			string message;
		};

		struct field_limit {
			bool jump = false;
			bool movement_skills = false;
			bool summoning_bag = false;
			bool mystic_door = false;
			bool channel_switching = false;
			bool regular_exp_loss = false;
			bool vip_rock = false;
			bool minigames = false;
			bool mount = false;
			bool potion_use = false;
			bool drop_down = false;
			bool chalkboard = false;
		};

		struct map_info {
			bool clock = false;
			bool town = false;
			bool swim = false;
			bool fly = false;
			bool everlast = false;
			bool no_leader_pass = false;
			bool shop = false;
			bool scroll_disable = false;
			bool shuffle_reactors = false;
			bool force_map_equip = false;
			int8_t continent = -1;
			int8_t regen_rate = 0;
			int8_t ship_kind = -1;
			game_player_level min_level = 0;
			uint8_t regular_hp_decrease = 0;
			game_map_id return_map = 0;
			game_map_id forced_return = 0;
			game_map_id link = 0;
			int32_t time_limit = 0;
			game_item_id protect_item = 0;
			game_damage damage_per_second = 0;
			double spawn_rate = 0.;
			double traction = 0.;
			string default_music;
			string shuffle_name;
			string message;
			rect dimensions;
			field_limit limitations;
		};

		class map_data_provider {
		public:
			auto load_data() -> void;
			auto get_map(game_map_id map_id) -> map *;
			auto unload_map(game_map_id map_id) -> void;
			auto get_continent(game_map_id map_id) const -> opt_int8_t;
		private:
			auto load_map_data(game_map_id map_id, map *&data) -> game_map_id;
			auto load_map_time_mob(map *map) -> void;
			auto load_footholds(map *map, game_map_id link) -> void;
			auto load_map_life(map *map, game_map_id link) -> void;
			auto load_portals(map *map, game_map_id link) -> void;
			auto load_seats(map *map, game_map_id link) -> void;
			auto load_map(game_map_id map_id, map *&map) -> void;

			mutex m_load_mutex;
			hash_map<game_map_id, map *> m_maps;
			hash_map<int8_t, int8_t> m_continents;
		};
	}
}