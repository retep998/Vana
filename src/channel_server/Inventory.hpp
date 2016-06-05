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

#include "common/item_constants.hpp"
#include "common/types.hpp"

namespace vana {
	class item;
	class packet_reader;

	namespace channel_server {
		class player;

		namespace inventory {
			auto add_item(ref_ptr<player> player, item *item_value, bool from_drop = false) -> game_slot_qty;
			auto add_new_item(ref_ptr<player> player, game_item_id item_id, game_slot_qty amount, items::stat_variance variance_policy = items::stat_variance::none) -> void;
			auto take_item(ref_ptr<player> player, game_item_id item_id, game_slot_qty how_many) -> void;
			auto use_item(ref_ptr<player> player, game_item_id item_id) -> void;
			auto take_item_slot(ref_ptr<player> player, game_inventory inv, game_inventory_slot slot, game_slot_qty amount, bool take_star = false, bool override_gm_benefits = false) -> void;
		}
	}
}