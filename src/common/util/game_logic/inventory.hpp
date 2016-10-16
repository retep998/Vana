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

#include "common/constant/inventory.hpp"
#include "common/types.hpp"
#include <cmath>

namespace vana {
	namespace util {
		namespace game_logic {
			namespace inventory {
				inline auto get_inventory(game_item_id item_id) -> game_inventory { return static_cast<game_inventory>(item_id / 1000000); }
				inline auto is_valid_inventory(game_inventory inv) -> bool { return inv > 0 && inv <= constant::inventory::count; }
				inline auto is_cash_slot(game_inventory_slot slot) -> bool { return std::abs(slot) > 100; }
				inline auto strip_cash_slot(game_inventory_slot slot) -> game_inventory_slot { return static_cast<game_inventory_slot>(is_cash_slot(slot) ? std::abs(slot) - 100 : std::abs(slot)); }
			}
		}
	}
}