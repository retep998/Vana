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
			namespace scroll_type {
				enum scroll_type : int16_t {
					helm = 0,
					face = 100,
					eye = 200,
					earring = 300,
					topwear = 400,
					overall = 500,
					bottomwear = 600,
					shoes = 700,
					gloves = 800,
					shield = 900,
					cape = 1000,
					ring = 1100,
					pendant = 1200,
					one_handed_sword = 3000,
					one_handed_axe = 3100,
					one_handed_mace = 3200,
					dagger = 3300,
					wand = 3700,
					staff = 3800,
					two_handed_sword = 4000,
					two_handed_axe = 4100,
					two_handed_mace = 4200,
					spear = 4300,
					polearm = 4400,
					bow = 4500,
					crossbow = 4600,
					claw = 4700,
					knuckle = 4800,
					gun = 4900,
					pet_equip = 8000,
					clean_slate = 9000,
					chaos = 9100
				};
			}
		}
	}
}