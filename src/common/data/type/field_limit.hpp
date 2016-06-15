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
#include <string>

namespace vana {
	namespace data {
		namespace type {
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
		}
	}
}