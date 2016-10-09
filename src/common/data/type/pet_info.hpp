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
			struct pet_info {
				bool no_revive = false;
				bool no_storing_in_cash_shop = false;
				bool auto_react = false;
				int8_t evolve_level = 0;
				int32_t hunger = 0;
				int32_t life = 0;
				int32_t limited_life = 0;
				game_item_id evolve_item = 0;
				game_item_id item_id = 0;
				string name;
			};
		}
	}
}