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

#include "common/file_time.hpp"
#include "common/types.hpp"
#include <limits>

namespace vana {
	namespace constant {
		namespace item {
			namespace max_stat {
				const game_stat str = std::numeric_limits<int16_t>::max();
				const game_stat dex = std::numeric_limits<int16_t>::max();
				const game_stat intl = std::numeric_limits<int16_t>::max();
				const game_stat luk = std::numeric_limits<int16_t>::max();
				const game_health hp = std::numeric_limits<int16_t>::max();
				const game_health mp = std::numeric_limits<int16_t>::max();
				const game_stat watk = 1999;
				const game_stat wdef = 255;
				const game_stat matk = 1999;
				const game_stat mdef = 255;
				const game_stat acc = 103;
				const game_stat avoid = 103;
				const game_stat hands = std::numeric_limits<int16_t>::max();
				const game_stat speed = 40;
				const game_stat jump = 23;
			}
		}
	}
}