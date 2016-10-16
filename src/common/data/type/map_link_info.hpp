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

#include "common/data/type/foothold_info.hpp"
#include "common/data/type/portal_info.hpp"
#include "common/data/type/seat_info.hpp"
#include "common/data/type/spawn_info.hpp"
#include "common/data/type/time_mob_info.hpp"
#include "common/rect.hpp"
#include "common/types.hpp"
#include "common/util/optional.hpp"
#include <string>
#include <vector>

namespace vana {
	namespace data {
		namespace type {
			struct map_link_info {
				game_map_id id;
				optional<time_mob_info> time_mob;
				vector<npc_spawn_info> npcs;
				vector<reactor_spawn_info> reactors;
				vector<mob_spawn_info> mobs;
				vector<portal_info> portals;
				vector<foothold_info> footholds;
				vector<seat_info> seats;
			};
		}
	}
}