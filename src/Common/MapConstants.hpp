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

#include "Common/Types.hpp"

namespace vana {
	namespace maps {
		enum : game_map_id {
			gm_map = 180000000,
			origin_of_clock_tower = 220080001,
			sorcerers_room = 270020211,
			no_map = 999999999
		};
	}

	namespace ship_kind {
		enum : int8_t {
			regular = 0x00,
			balrog = 0x01,
		};
	}
}