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

#include "Common/Point.hpp"
#include "Common/Types.hpp"
#include <string>

namespace vana {
	struct spawn_info {
		auto set_spawn_info(const spawn_info &rhs) -> void {
			id = rhs.id;
			time = rhs.time;
			foothold = rhs.foothold;
			pos = rhs.pos;
			faces_left = rhs.faces_left;
			spawned = rhs.spawned;
		}

		bool faces_left = false;
		bool spawned = false;
		int32_t id = 0;
		int32_t time = 0;
		game_foothold_id foothold = 0;
		point pos;
	};

	struct npc_spawn_info : public spawn_info {
		game_coord rx0 = 0;
		game_coord rx1 = 0;
	};

	struct mob_spawn_info : public spawn_info {
		game_mob_id link = 0;
	};

	struct reactor_spawn_info : public spawn_info {
		string name;
	};
}