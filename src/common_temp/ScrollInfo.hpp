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

#include "common_temp/Types.hpp"

namespace vana {
	struct scroll_info {
		bool warm_support = false;
		bool rand_stat = false;
		bool prevent_slip = false;
		int8_t recover = 0;
		game_stat str = 0;
		game_stat dex = 0;
		game_stat intl = 0;
		game_stat luk = 0;
		game_health hp = 0;
		game_health mp = 0;
		game_stat watk = 0;
		game_stat matk = 0;
		game_stat wdef = 0;
		game_stat mdef = 0;
		game_stat acc = 0;
		game_stat avo = 0;
		game_stat hand = 0;
		game_stat jump = 0;
		game_stat speed = 0;
		uint16_t success = 0;
		uint16_t cursed = 0;
	};
}