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
#include "trade.hpp"
#include "common/data/provider/item.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "common/util/game_logic/item.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/trade_handler.hpp"
#include "channel_server/trades.hpp"

namespace vana {
namespace channel_server {

active_trade::active_trade(ref_ptr<player> sender, ref_ptr<player> receiver, game_trade_id id) :
	m_id{id}
{
	m_sender = make_owned_ptr<trade_info>();
	m_receiver = make_owned_ptr<trade_info>();

	sender->set_trading(true);
	receiver->set_trading(false);
	sender->set_trade_id(id);
	receiver->set_trade_id(id);
	m_sender_id = sender->get_id();
	m_receiver_id = receiver->get_id();
}

auto active_trade::both_can_trade() -> bool {
	if (!can_trade(get_sender(), get_receiver_trade())) {
		return false;
	}
	if (!can_trade(get_receiver(), get_sender_trade())) {
		return false;
	}
	return true;
}

auto active_trade::can_trade(ref_ptr<player> target, trade_info *unit) -> bool {
	bool can_trade =
		target->get_inventory()->can_accept(unit->mesos) == stack_result::full;

	if (can_trade && unit->count > 0) {
		array<game_trade_slot, constant::inventory::count> totals = {0};
		hash_map<game_item_id, game_slot_qty> added;
		for (game_trade_slot i = 0; i < trade_info::trade_size; ++i) {
			// Create item structure to determine needed slots among stackable items
			// Also, determine needed slots for nonstackables
			if (unit->items[i] != nullptr) {
				item *check = unit->items[i];
				game_item_id item_id = check->get_id();
				game_inventory inv = vana::util::game_logic::inventory::get_inventory(item_id);
				if (!vana::util::game_logic::item::is_stackable(item_id)) {
					// No need to clutter unordered map
					totals[inv - 1]++;
				}
				else {
					if (added.find(item_id) != std::end(added)) {
						// Already initialized this item
						added[item_id] += check->get_amount();
					}
					else {
						added[item_id] = check->get_amount();
					}
				}
			}
		}
		for (game_trade_slot i = 0; i < trade_info::trade_size; ++i) {
			// Determine precisely how many slots are needed for stackables
			if (unit->items[i] != nullptr) {
				item *check = unit->items[i];
				game_item_id item_id = check->get_id();
				game_inventory inv = vana::util::game_logic::inventory::get_inventory(item_id);
				if (vana::util::game_logic::item::is_stackable(item_id)) {
					// Already did these
					if (added.find(item_id) == std::end(added)) {
						// Already did this item
						continue;
					}
					auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id);
					game_slot_qty max_slot = item_info->max_slot;
					int32_t current_amount = target->get_inventory()->get_item_amount(item_id);
					int32_t last_slot = (current_amount % max_slot); // Get the number of items in the last slot
					int32_t item_sum = last_slot + added[item_id];
					bool need_slots = false;
					if (last_slot > 0) {
						// Items in the last slot, potential for needing slots
						if (item_sum > max_slot) {
							need_slots = true;
						}
					}
					else {
						// Full in the last slot, for sure need all slots
						need_slots = true;
					}
					if (need_slots) {
						game_trade_slot num_slots = static_cast<game_trade_slot>(item_sum / max_slot);
						game_trade_slot remainder = static_cast<game_trade_slot>(item_sum % max_slot);
						if (remainder > 0) {
							totals[inv - 1]++;
						}
						totals[inv - 1] += num_slots;
					}
					added.erase(item_id);
				}
			}
		}
		for (game_inventory i = 0; i < constant::inventory::count; ++i) {
			// Determine if needed slots are available
			if (totals[i] > 0) {
				game_trade_slot incrementor = 0;
				for (game_inventory_slot_count g = 1; g <= target->get_inventory()->get_max_slots(i + 1); ++g) {
					if (target->get_inventory()->get_item(i + 1, g) == nullptr) {
						incrementor++;
					}
					if (incrementor >= totals[i]) {
						break;
					}
				}
				if (incrementor < totals[i]) {
					can_trade = false;
					break;
				}
			}
		}
	}
	return can_trade;
}

auto active_trade::give_items(ref_ptr<player> player, trade_info *info) -> void {
	if (info->count > 0) {
		for (game_trade_slot i = 0; i < trade_info::trade_size; ++i) {
			if (info->items[i] != nullptr) {
				item *trade_item = info->items[i];
				if (trade_item->has_karma()) {
					trade_item->set_karma(false);
					trade_item->set_trade_block(true);
				}
				inventory::add_item(player, new item{trade_item});
				delete trade_item;
			}
		}
	}
}

auto active_trade::give_mesos(ref_ptr<player> player, trade_info *info, bool traded) -> void {
	if (info->mesos.has_any()) {
		game_mesos apply = info->mesos.get_mesos();
		int32_t tax_level = trade_handler::get_tax_level(apply);
		if (traded && tax_level != 0) {
			int64_t mesos = apply * tax_level / 10000;
			apply -= static_cast<game_mesos>(mesos);
		}
		player->get_inventory()->add_mesos(apply);
	}
}

auto active_trade::return_trade() -> void {
	trade_info *send = get_sender_trade();
	trade_info *recv = get_receiver_trade();
	auto one = get_sender();
	auto two = get_receiver();
	if (one != nullptr) {
		give_items(one, send);
		give_mesos(one, send);
	}
	if (two != nullptr) {
		give_items(two, recv);
		give_mesos(two, recv);
	}
}

auto active_trade::swap_trade() -> void {
	trade_info *send = get_sender_trade();
	trade_info *recv = get_receiver_trade();
	auto one = get_sender();
	auto two = get_receiver();
	give_items(one, recv);
	give_items(two, send);
	give_mesos(one, recv, true);
	give_mesos(two, send, true);
}

auto active_trade::both_accepted() -> bool {
	return get_sender_trade()->accepted && get_receiver_trade()->accepted;
}

auto active_trade::accept(trade_info *unit) -> void {
	unit->accepted = true;
}

auto active_trade::add_mesos(ref_ptr<player> holder, trade_info *unit, game_mesos amount) -> game_mesos {
	unit->mesos.add_mesos(amount);
	holder->get_inventory()->take_mesos(amount, false, true);
	return unit->mesos.get_mesos();
}

auto active_trade::add_item(ref_ptr<player> holder, trade_info *unit, item *value, game_trade_slot trade_slot, game_inventory_slot inventory_slot, game_inventory inventory, game_slot_qty amount) -> item * {
	auto use = new item{value};
	if (amount == value->get_amount() || vana::util::game_logic::item::is_equip(value->get_id())) {
		holder->get_inventory()->set_item(inventory, inventory_slot, nullptr);

		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::modify_slot, value, inventory_slot);
		holder->send(packets::inventory::inventory_operation(true, ops));

		holder->get_inventory()->delete_item(inventory, inventory_slot);
	}
	else {
		value->dec_amount(amount);
		holder->get_inventory()->change_item_amount(value->get_id(), value->get_amount());

		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::modify_quantity, value, inventory_slot);
		holder->send(packets::inventory::inventory_operation(true, ops));

		use->set_amount(amount);
	}
	holder->send(packets::inventory::blank_update()); // Should prevent locking up in .70, don't know why it locks
	unit->count++;
	game_trade_slot index = trade_slot - 1;
	unit->items[index] = use;
	return use;
}

auto active_trade::get_sender() -> ref_ptr<player> {
	return channel_server::get_instance().get_player_data_provider().get_player(m_sender_id);
}

auto active_trade::get_receiver() -> ref_ptr<player> {
	return channel_server::get_instance().get_player_data_provider().get_player(m_receiver_id);
}

}
}