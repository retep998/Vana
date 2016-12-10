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
#include "inventory.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/skill.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/randomizer.hpp"
#include "channel_server/buffs.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/map.hpp"
#include "channel_server/monster_book_packet.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/pets_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_monster_book.hpp"

namespace vana {
namespace channel_server {

auto inventory::add_item(ref_ptr<player> player, item *item_value, bool from_drop) -> game_slot_qty {
	game_inventory inv = vana::util::game_logic::inventory::get_inventory(item_value->get_id());
	game_inventory_slot free_slot = 0;
	for (game_inventory_slot s = 1; s <= player->get_inventory()->get_max_slots(inv); s++) {
		item *old_item = player->get_inventory()->get_item(inv, s);
		if (old_item != nullptr) {
			auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_value->get_id());
			game_slot_qty max_slot = item_info->max_slot;
			if (vana::util::game_logic::item::is_stackable(item_value->get_id()) && old_item->get_id() == item_value->get_id() && old_item->get_amount() < max_slot) {
				if (item_value->get_amount() + old_item->get_amount() > max_slot) {
					game_slot_qty amount = max_slot - old_item->get_amount();
					item_value->dec_amount(amount);
					old_item->set_amount(max_slot);

					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::add_item, old_item, s);
					player->send(packets::inventory::inventory_operation(from_drop, ops));
				}
				else {
					item_value->inc_amount(old_item->get_amount());
					player->get_inventory()->delete_item(inv, s);
					player->get_inventory()->add_item(inv, s, item_value);
					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::add_item, item_value, s);
					player->send(packets::inventory::inventory_operation(from_drop, ops));
					return 0;
				}
			}
		}
		else if (!free_slot) {
			free_slot = s;
			if (!vana::util::game_logic::item::is_stackable(item_value->get_id())) {
				break;
			}
		}
	}
	if (free_slot != 0) {
		player->get_inventory()->add_item(inv, free_slot, item_value);

		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::add_item, item_value, free_slot);
		player->send(packets::inventory::inventory_operation(from_drop, ops));

		if (vana::util::game_logic::item::is_pet(item_value->get_id())) {
			pet *value = new pet{player.get(), item_value};
			player->get_pets()->add_pet(value);
			value->set_inventory_slot(static_cast<int8_t>(free_slot));
			player->send(packets::pets::update_pet(value, item_value));
		}

		return 0;
	}
	return item_value->get_amount();
}

auto inventory::add_new_item(ref_ptr<player> player, game_item_id item_id, game_slot_qty amount, stat_variance variance_policy) -> void {
	auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id);
	if (item_info == nullptr) {
		return;
	}

	game_slot_qty max = item_info->max_slot;
	game_slot_qty this_amount = 0;
	if (vana::util::game_logic::item::is_rechargeable(item_id)) {
		this_amount = max + player->get_skills()->get_rechargeable_bonus();
		amount -= 1;
	}
	else if (vana::util::game_logic::item::is_equip(item_id) || vana::util::game_logic::item::is_pet(item_id)) {
		this_amount = 1;
		amount -= 1;
	}
	else if (amount > max) {
		this_amount = max;
		amount -= max;
	}
	else {
		this_amount = amount;
		amount = 0;
	}

	item *value = nullptr;
	if (vana::util::game_logic::item::is_equip(item_id)) {
		value = new item{
			channel_server::get_instance().get_equip_data_provider(),
			item_id,
			variance_policy,
			player->has_gm_benefits()
		};
		if (vana::util::game_logic::item::is_mount(item_id)) {
			player->get_mounts()->add_mount(item_id);
		}
	}
	else {
		value = new item{item_id, this_amount};
	}
	if (add_item(player, value, vana::util::game_logic::item::is_pet(item_id)) == 0 && amount > 0) {
		add_new_item(player, item_id, amount);
	}
}

auto inventory::take_item(ref_ptr<player> player, game_item_id item_id, game_slot_qty how_many) -> void {
	if (player->has_gm_benefits()) {
		player->send(packets::inventory::blank_update());
		return;
	}

	player->get_inventory()->change_item_amount(item_id, -how_many);
	game_inventory inv = vana::util::game_logic::inventory::get_inventory(item_id);
	for (game_inventory_slot i = 1; i <= player->get_inventory()->get_max_slots(inv); i++) {
		item *item = player->get_inventory()->get_item(inv, i);
		if (item == nullptr) {
			continue;
		}
		if (item->get_id() == item_id) {
			if (item->get_amount() >= how_many) {
				item->dec_amount(how_many);
				if (item->get_amount() == 0 && !vana::util::game_logic::item::is_rechargeable(item->get_id())) {
					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::modify_slot, item, i);
					player->send(packets::inventory::inventory_operation(true, ops));

					player->get_inventory()->delete_item(inv, i);
				}
				else {
					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::modify_quantity, item, i);
					player->send(packets::inventory::inventory_operation(true, ops));
				}
				break;
			}
			else if (!vana::util::game_logic::item::is_rechargeable(item->get_id())) {
				how_many -= item->get_amount();
				item->set_amount(0);

				vector<inventory_packet_operation> ops;
				ops.emplace_back(packets::inventory::operation_types::modify_slot, item, i);
				player->send(packets::inventory::inventory_operation(true, ops));

				player->get_inventory()->delete_item(inv, i);
			}
		}
	}
}

auto inventory::take_item_slot(ref_ptr<player> player, game_inventory inv, game_inventory_slot slot, game_slot_qty amount, bool take_star, bool override_gm_benefits) -> void {
	if (!override_gm_benefits && player->has_gm_benefits()) {
		return;
	}

	item *item = player->get_inventory()->get_item(inv, slot);
	if (item == nullptr || item->get_amount() - amount < 0) {
		return;
	}
	item->dec_amount(amount);
	if ((item->get_amount() == 0 && !vana::util::game_logic::item::is_rechargeable(item->get_id())) || (take_star && vana::util::game_logic::item::is_rechargeable(item->get_id()))) {
		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::modify_slot, item, slot);
		player->send(packets::inventory::inventory_operation(true, ops));

		player->get_inventory()->delete_item(inv, slot);
	}
	else {
		player->get_inventory()->change_item_amount(item->get_id(), -amount);

		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::modify_quantity, item, slot);
		player->send(packets::inventory::inventory_operation(true, ops));
	}
}

auto inventory::use_item(ref_ptr<player> player, game_item_id item_id) -> void {
	auto item = channel_server::get_instance().get_item_data_provider().get_consume_info(item_id);
	if (item == nullptr) {
		// Not a consume
		return;
	}

	int16_t potency = 100;
	game_skill_id alchemist = player->get_skills()->get_alchemist();

	if (player->get_skills()->get_skill_level(alchemist) > 0) {
		potency = player->get_skills()->get_skill_info(alchemist)->x;
	}

	auto buffs = player->get_active_buffs();
	if (item->hp > 0) {
		player->get_stats()->modify_hp(item->hp * buffs->get_zombified_potency(potency) / 100);
	}
	if (item->mp > 0) {
		player->get_stats()->modify_mp(item->mp * potency / 100);
	}
	else {
		player->get_stats()->set_mp(player->get_stats()->get_mp(), true);
	}
	if (item->hp_rate != 0) {
		player->get_stats()->modify_hp(item->hp_rate * buffs->get_zombified_potency(player->get_stats()->get_max_hp()) / 100);
	}
	if (item->mp_rate != 0) {
		player->get_stats()->modify_mp(item->mp_rate * player->get_stats()->get_max_mp() / 100);
	}
	if (item->ailment > 0) {
		player->get_active_buffs()->use_debuff_healing_item(item->ailment);
	}
	if (item->buff_time.count() > 0 && item->chance == 0) {
		seconds time{item->buff_time.count() * potency / 100};
		buffs::add_buff(player, item_id, time);
	}
	if (vana::util::game_logic::item::is_monster_card(item_id)) {
		bool is_full = player->get_monster_book()->add_card(item_id); // Has a special buff for being full?
		player->send(packets::monster_book::add_card(item_id, player->get_monster_book()->get_card_level(item_id), is_full));
		if (!is_full) {
			player->send_map(packets::monster_book::add_card_effect(player->get_id()));
		}
		buffs::add_buff(player, item_id, item->buff_time);
	}
}

}
}