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
#include "common/item.hpp"
#include "common/types.hpp"
#include "common/util/meso_inventory.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace vana {
	class item;
	class packet_builder;

	namespace channel_server {
		class player;

		class player_inventory {
			NONCOPYABLE(player_inventory);
			NO_DEFAULT_CONSTRUCTOR(player_inventory);
		public:
			player_inventory(ref_ptr<player> player, const array<game_inventory_slot_count, constant::inventory::count> &max_slots, game_mesos mesos);
			~player_inventory();

			auto load() -> void;
			auto save() -> void;

			auto connect_packet(packet_builder &builder) -> void;
			auto add_equipped_packet(packet_builder &builder) -> void;
			auto rock_packet(packet_builder &builder) -> void;
			auto wishlist_info_packet(packet_builder &builder) -> void;

			auto set_mesos(game_mesos mesos, bool send_packet = false) -> void;
			auto modify_mesos(game_mesos mod, bool allow_partial = false, bool send_packet = false) -> vana::util::meso_modify_result;
			auto add_mesos(game_mesos mod, bool allow_partial = false, bool send_packet = false) -> vana::util::meso_modify_result;
			auto take_mesos(game_mesos mod, bool allow_partial = false, bool send_packet = false) -> vana::util::meso_modify_result;
			auto add_max_slots(game_inventory inventory, game_inventory_slot_count rows) -> void;
			auto add_item(game_inventory inv, game_inventory_slot slot, item *item, bool send_packet_loading = false) -> void;
			auto delete_item(game_inventory inv, game_inventory_slot slot, bool update_amount = true) -> void;
			auto set_item(game_inventory inv, game_inventory_slot slot, item *item) -> void;
			auto change_item_amount(game_item_id item_id, game_slot_qty amount) -> void { m_item_amounts[item_id] += amount; }
			auto set_auto_hp_pot(game_item_id id) -> void { m_auto_hp_pot_id = id; }
			auto set_auto_mp_pot(game_item_id id) -> void { m_auto_mp_pot_id = id; }
			auto swap_items(game_inventory inventory, game_inventory_slot slot1, game_inventory_slot slot2) -> void;
			auto destroy_equipped_item(game_item_id item_id) -> void;

			auto get_max_slots(game_inventory inv) const -> game_inventory_slot_count;
			auto get_mesos() const -> game_mesos;
			auto has_any_mesos() const -> bool;
			auto can_accept(const vana::util::meso_inventory &other) const -> stack_result;
			auto can_modify_mesos(game_mesos mesos) const -> stack_result;
			auto can_add_mesos(game_mesos mesos) const -> stack_result;
			auto can_take_mesos(game_mesos mesos) const -> stack_result;
			auto get_auto_hp_pot() const -> game_item_id;
			auto get_auto_mp_pot() const -> game_item_id;

			auto get_item_amount_by_slot(game_inventory inv, game_inventory_slot slot) -> game_slot_qty;
			auto get_item_amount(game_item_id item_id) -> game_slot_qty;
			auto get_equipped_id(game_inventory_slot slot, bool cash = false) -> game_item_id;
			auto get_item(game_inventory inv, game_inventory_slot slot) -> item *;
			auto is_equipped_item(game_item_id item_id) -> bool;

			auto has_open_slots_for(game_item_id item_id, game_slot_qty amount, bool can_stack = false) -> bool;
			auto get_open_slots_num(game_inventory inv) -> game_inventory_slot_count;
			auto do_shadow_stars() -> game_item_id;

			auto is_hammering() const -> bool { return m_hammer != -1; }
			auto get_hammer_slot() const -> game_inventory_slot { return m_hammer; }
			auto set_hammer_slot(game_inventory_slot hammer) -> void { m_hammer = hammer; }

			auto add_rock_map(game_map_id map_id, int8_t type) -> void;
			auto del_rock_map(game_map_id map_id, int8_t type) -> void;
			auto ensure_rock_destination(game_map_id map_id) -> bool;

			auto add_wish_list_item(game_item_id item_id) -> void;
			auto check_expired_items() -> void;
		private:
			auto add_equipped(game_inventory_slot slot, game_item_id item_id) -> void;
			auto modify_mesos_internal(vana::util::meso_modify_result query, bool send_packet) -> vana::util::meso_modify_result;

			game_inventory_slot m_hammer = -1;
			game_item_id m_auto_hp_pot_id = 0;
			game_item_id m_auto_mp_pot_id = 0;
			view_ptr<player> m_player;
			vana::util::meso_inventory m_mesos;
			array<game_inventory_slot_count, constant::inventory::count> m_max_slots;
			array<array<game_item_id, 2>, constant::inventory::equipped_slots> m_equipped; // Separate sets of slots for regular items and cash items
			array<hash_map<game_inventory_slot, item *>, constant::inventory::count> m_items;
			vector<game_map_id> m_vip_locations;
			vector<game_map_id> m_rock_locations;
			vector<game_item_id> m_wishlist;
			hash_map<game_item_id, game_slot_qty> m_item_amounts;
		};
	}
}