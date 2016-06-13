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

#include "common/data/type/shop_info.hpp"
#include "common/data/type/shop_item_info.hpp"
#include "common/shop_data.hpp"
#include "common/types.hpp"
#include <map>
#include <unordered_map>
#include <vector>

namespace vana {
	namespace data {
		namespace provider {
			class shop {
			public:
				auto load_data() -> void;

				auto is_shop(game_shop_id id) const -> bool;
				auto get_shop(game_shop_id id) const -> shop_data;
				auto get_shop_item(game_shop_id shop_id, uint16_t shop_index) const -> const data::type::shop_item_info * const;
				auto get_recharge_cost(game_shop_id shop_id, game_item_id item_id, game_slot_qty amount) const -> game_mesos;
			private:
				auto load_shops() -> void;
				auto load_user_shops() -> void;
				auto load_recharge_tiers() -> void;

				hash_map<game_shop_id, data::type::shop_info> m_shops;
				hash_map<int8_t, ord_map<game_item_id, double>> m_recharge_costs;
			};
		}
	}
}