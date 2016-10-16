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

#include "common/data/type/map_info.hpp"
#include "common/types.hpp"
#include "common/util/optional.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace data {
		namespace provider {
			class map {
			public:
				auto load_data() -> void;
				auto get_map(game_map_id map_id) -> ref_ptr<const data::type::map_info>;
				auto get_continent(game_map_id map_id) -> opt_int8_t;
			private:
				auto load_continents() -> void;
				auto load_maps() -> void;

				auto load_map_time_mob(data::type::map_link_info &map) -> void;
				auto load_footholds(data::type::map_link_info &map) -> void;
				auto load_map_life(data::type::map_link_info &map) -> void;
				auto load_portals(data::type::map_link_info &map) -> void;
				auto load_seats(data::type::map_link_info &map) -> void;
				auto load_map(data::type::map_info &map) -> void;

				mutex m_load_mutex;
				vector<ref_ptr<data::type::map_info>> m_maps;
				vector<ref_ptr<data::type::map_link_info>> m_link_info;
				vector<pair<int8_t, int8_t>> m_continents;
			};
		}
	}
}