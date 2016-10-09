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

#include "common/point.hpp"
#include "common/rect.hpp"
#include "common/types.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace channel_server {
		class map;
		class player;

		class map_factory {
		public:
			auto get_map(game_map_id map_id) -> map *;
			auto unload_map(game_map_id map_id) -> void;
		private:
			mutex m_load_mutex;
			vector<map *> m_maps;
		};
	}
}