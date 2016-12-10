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
#include "common/constant/item.hpp"
#include "common/types.hpp"
#include "common/util/game_logic/inventory.hpp"
#include <cmath>

namespace vana {
	namespace util {
		namespace game_logic {
			namespace item {
				inline auto get_item_type(game_item_id item_id) -> int32_t { return item_id / 10000; }
				inline auto get_scroll_type(game_item_id item_id) -> int32_t { return (item_id % 10000) - (item_id % 100); }
				inline auto item_type_to_scroll_type(game_item_id item_id) -> int32_t { return (get_item_type(item_id) % 100) * 100; }
				inline auto is_arrow(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_arrow; }
				inline auto is_star(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_star; }
				inline auto is_bullet(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_bullet; }
				inline auto is_projectile(game_item_id item_id) -> bool { return is_bullet(item_id) || is_star(item_id) || is_arrow(item_id); }
				inline auto is_rechargeable(game_item_id item_id) -> bool { return is_bullet(item_id) || is_star(item_id); }
				inline auto is_equip(game_item_id item_id) -> bool { return vana::util::game_logic::inventory::get_inventory(item_id) == constant::inventory::equip; }
				inline auto is_pet(game_item_id item_id) -> bool {	return (item_id / 100 * 100) == 5000000; }
				inline auto is_stackable(game_item_id item_id) -> bool { return !(is_rechargeable(item_id) || is_equip(item_id) || is_pet(item_id)); }
				inline auto is_overall(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_overall; }
				inline auto is_top(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_top; }
				inline auto is_bottom(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_bottom; }
				inline auto is_shield(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_shield; }
				inline auto is2h_weapon(game_item_id item_id) -> bool { return get_item_type(item_id) / 10 == 14; }
				inline auto is1h_weapon(game_item_id item_id) -> bool { return get_item_type(item_id) / 10 == 13; }
				inline auto is_bow(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_bow; }
				inline auto is_crossbow(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_crossbow; }
				inline auto is_sword(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_1h_sword || get_item_type(item_id) == constant::item::type::weapon_2h_sword; }
				inline auto is_mace(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_1h_mace || get_item_type(item_id) == constant::item::type::weapon_2h_mace; }
				inline auto is_mount(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::mount; }
				inline auto is_medal(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::medal; }
				inline auto is_gm_equip(game_item_id item_id) -> bool { return item_id == constant::item::gm_bottom || item_id == constant::item::gm_hat || item_id == constant::item::gm_top || item_id == constant::item::gm_weapon; }
				inline auto is_monster_card(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_monster_card; }
			}
		}
	}
}