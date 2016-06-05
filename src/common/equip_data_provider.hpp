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

#include "common/item_temp.hpp"
#include "common/item_constants.hpp"
#include "common/equip_info.hpp"
#include "common/types_temp.hpp"
#include <unordered_map>

namespace vana {
	class equip_data_provider {
	public:
		auto load_data() -> void;

		auto set_equip_stats(item *equip, items::stat_variance policy, bool is_gm, bool is_item_initialization) const -> void;
		auto can_equip(game_item_id item_id, game_gender_id gender, game_job_id job, game_stat str, game_stat dex, game_stat intt, game_stat luk, game_fame fame) const -> bool;
		auto is_valid_slot(game_item_id equip_id, game_inventory_slot target) const -> bool;
		auto get_slots(game_item_id equip_id) const -> int8_t;
		auto get_equip_info(game_item_id equip_id) const -> const equip_info &;
	private:
		auto load_equips() -> void;

		hash_map<game_item_id, equip_info> m_equip_info;
	};
}