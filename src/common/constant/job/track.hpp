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

#include "common/types.hpp"
#include <initializer_list>

namespace vana {
	namespace constant {
		namespace job {
			namespace track {
				enum tracks : int8_t {
					beginner = 0,
					warrior = 1,
					magician = 2,
					bowman = 3,
					thief = 4,
					pirate = 5,

					// Not sure if junior_gm is correct
					junior_gm = 8,
					gm = 9,

					noblesse = 10,
					dawn_warrior = 11,
					blaze_wizard = 12,
					wind_archer = 13,
					night_walker = 14,
					thunder_breaker = 15,

					legend = 20,
					aran = 21,
					evan = 22,
					mercedes = 23,
					phantom = 24,
					luminous = 27,

					citizen = 30,
					demon_slayer = 31,
					battle_mage = 32,
					wild_hunter = 33,
					mechanic = 35,
					xenon = 36,

					mihile = 51,

					nova = 60,
					kaiser = 61,
					angelic = 65,
				};

				const init_list<int8_t> all = {
					beginner, warrior, magician, bowman, thief, pirate,
					noblesse, dawn_warrior, blaze_wizard, wind_archer, night_walker, thunder_breaker,
					legend, aran, evan, mercedes, phantom, luminous,
					citizen, demon_slayer, battle_mage, wild_hunter, mechanic, xenon,
					mihile,
					nova, kaiser, angelic,
				};
			}
		}
	}
}