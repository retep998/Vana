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
#include "npc_handler.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/npc.hpp"
#include "common/data/provider/shop.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/npc.hpp"
#include "channel_server/npc_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/storage_packet.hpp"

namespace vana {
namespace channel_server {

namespace shop_opcodes {
	enum opcodes : int8_t {
		buy = 0x00,
		sell = 0x01,
		recharge = 0x02,
		exit_shop = 0x03,
		// Storage
		take_item = 0x04,
		store_item = 0x05,
		meso_transaction = 0x07,
		exit_storage = 0x08
	};
}

auto npc_handler::handle_npc(ref_ptr<player> player, packet_reader &reader) -> void {
	if (player->get_npc() != nullptr) {
		return;
	}
	size_t npc_id = map::make_npc_id(reader.get<game_map_object>());

	if (!player->get_map()->is_valid_npc_index(npc_id)) {
		// Shouldn't ever happen except in edited packets
		return;
	}

	data::type::npc_spawn_info npcs = player->get_map()->get_npc(npc_id);
	if (player->get_npc() == nullptr && npc::has_script(npcs.id, 0, false)) {
		npc *value = new npc{npcs.id, player, npcs.pos};
		value->run();
		return;
	}
	if (player->get_shop() == 0) {
		if (npc_handler::show_shop(player, npcs.id) == result::successful) {
			return;
		}
		if (npc_handler::show_storage(player, npcs.id) == result::successful) {
			return;
		}
		if (npc_handler::show_guild_rank(player, npcs.id) == result::successful) {
			return;
		}
	}
}

auto npc_handler::handle_quest_npc(ref_ptr<player> player, game_npc_id npc_id, bool start, game_quest_id quest_id) -> void {
	if (player->get_npc() != nullptr) {
		return;
	}

	npc *quest_npc = new npc{npc_id, player, quest_id, start};
	quest_npc->run();
}

auto npc_handler::handle_npc_in(ref_ptr<player> player, packet_reader &reader) -> void {
	npc *npc = player->get_npc();
	if (npc == nullptr) {
		return;
	}

	int8_t type = reader.get<int8_t>();
	if (type != npc->get_sent_dialog()) {
		// Hacking
		return;
	}

	if (type == packets::npc::dialogs::quiz || type == packets::npc::dialogs::question) {
		npc->proceed_text(reader.get<string>());
		npc->check_end();
		return;
	}

	int8_t choice = reader.get<int8_t>();

	switch (type) {
		case packets::npc::dialogs::normal:
			switch (choice) {
				case 0: npc->proceed_back(); break;
				case 1:	npc->proceed_next(); break;
				default: npc->end(); break;
			}
			break;
		case packets::npc::dialogs::yes_no:
		case packets::npc::dialogs::accept_decline:
		case packets::npc::dialogs::accept_decline_no_exit:
			switch (choice) {
				case 0: npc->proceed_selection(0); break;
				case 1:	npc->proceed_selection(1); break;
				default: npc->end(); break;
			}
			break;
		case packets::npc::dialogs::get_text:
			if (choice != 0) {
				npc->proceed_text(reader.get<string>());
			}
			else {
				npc->end();
			}
			break;
		case packets::npc::dialogs::get_number:
			if (choice == 1) {
				npc->proceed_number(reader.get<int32_t>());
			}
			else {
				npc->end();
			}
			break;
		case packets::npc::dialogs::simple:
			if (choice == 0) {
				npc->end();
			}
			else {
				npc->proceed_selection(reader.get<uint8_t>());
			}
			break;
		case packets::npc::dialogs::style:
			if (choice == 1) {
				npc->proceed_selection(reader.get<uint8_t>());
			}
			else {
				npc->end();
			}
			break;
		default:
			npc->end();
	}
	npc->check_end();
}

auto npc_handler::handle_npc_animation(ref_ptr<player> player, packet_reader &reader) -> void {
	game_map_object npc_id = reader.get<game_map_object>();
	size_t internal_npc_id = map::make_npc_id(npc_id);
	
	if (!player->get_map()->is_valid_npc_index(internal_npc_id)) {
		// Shouldn't ever happen except in edited packets
		return;
	}
	
	uint8_t action1 = reader.get<uint8_t>();
	uint8_t action2 = reader.get<uint8_t>();

	move_path *path = nullptr;
	if (reader.get_buffer_length() > 0) {
		path = new move_path(reader);
	}

	player->send(packets::npc::animate_npc(npc_id, action1, action2, path));

	if (path != nullptr) {
		delete path;
	}
}

auto npc_handler::use_shop(ref_ptr<player> player, packet_reader &reader) -> void {
	if (player->get_shop() == 0) {
		// Hacking
		return;
	}
	int8_t type = reader.get<int8_t>();
	switch (type) {
		case shop_opcodes::buy: {
			uint16_t item_index = reader.get<uint16_t>();
			reader.skip<game_item_id>(); // No reason to trust this
			game_slot_qty quantity = reader.get<game_slot_qty>();
			reader.skip<game_mesos>(); // Price, don't want to trust this
			auto shop_item = channel_server::get_instance().get_shop_data_provider().get_shop_item(player->get_shop(), item_index);
			if (shop_item == nullptr) {
				// Hacking
				return;
			}

			game_slot_qty amount = shop_item->quantity;
			game_item_id item_id = shop_item->item_id;
			game_mesos price = shop_item->price;
			game_slot_qty total_amount = quantity * amount; // The game doesn't let you purchase more than 1 slot worth of items; if they're grouped, it buys them in single units, if not, it only allows you to go up to max_slot
			game_mesos total_price = quantity * price;
			auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id);

			if (price == 0 || total_amount > item_info->max_slot || total_amount < 0 || player->get_inventory()->get_mesos() < total_price) {
				// Hacking
				player->send(packets::npc::bought(packets::npc::bought_messages::not_enough_mesos));
				return;
			}
			bool have_slot = player->get_inventory()->has_open_slots_for(item_id, total_amount, true);
			if (!have_slot) {
				player->send(packets::npc::bought(packets::npc::bought_messages::no_slots));
				return;
			}
			inventory::add_new_item(player, item_id, total_amount);
			player->get_inventory()->modify_mesos(-total_price);
			player->send(packets::npc::bought(packets::npc::bought_messages::success));
			break;
		}
		case shop_opcodes::sell: {
			game_inventory_slot slot = reader.get<game_inventory_slot>();
			game_item_id item_id = reader.get<game_item_id>();
			game_slot_qty amount = reader.get<game_slot_qty>();
			game_inventory inv = game_logic_utilities::get_inventory(item_id);
			item *item = player->get_inventory()->get_item(inv, slot);
			if (item == nullptr || item->get_id() != item_id || (!game_logic_utilities::is_rechargeable(item_id) && amount > item->get_amount())) {
				// Hacking
				player->send(packets::npc::bought(packets::npc::bought_messages::not_enough_in_stock));
				return;
			}
			game_mesos price = channel_server::get_instance().get_item_data_provider().get_item_info(item_id)->price;

			player->get_inventory()->modify_mesos(price * amount);
			if (game_logic_utilities::is_rechargeable(item_id)) {
				inventory::take_item_slot(player, inv, slot, item->get_amount(), true);
			}
			else {
				inventory::take_item_slot(player, inv, slot, amount, true);
			}
			player->send(packets::npc::bought(packets::npc::bought_messages::success));
			break;
		}
		case shop_opcodes::recharge: {
			game_inventory_slot slot = reader.get<game_inventory_slot>();
			item *item = player->get_inventory()->get_item(constant::inventory::use, slot);
			if (item == nullptr || !game_logic_utilities::is_rechargeable(item->get_id())) {
				// Hacking
				return;
			}

			auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item->get_id());
			game_slot_qty max_slot = item_info->max_slot;
			if (game_logic_utilities::is_rechargeable(item->get_id())) {
				max_slot += player->get_skills()->get_rechargeable_bonus();
			}
			game_mesos modified_mesos = channel_server::get_instance().get_shop_data_provider().get_recharge_cost(player->get_shop(), item->get_id(), max_slot - item->get_amount());
			if (modified_mesos < 0 && player->get_inventory()->get_mesos() > -modified_mesos) {
				player->get_inventory()->modify_mesos(modified_mesos);
				item->set_amount(max_slot);

				vector<inventory_packet_operation> ops;
				ops.emplace_back(packets::inventory::operation_types::modify_quantity, item, slot);
				player->send(packets::inventory::inventory_operation(true, ops));

				player->send(packets::npc::bought(packets::npc::bought_messages::success));
			}
			break;
		}
		case shop_opcodes::exit_shop:
			player->set_shop(0);
			break;
	}
}

auto npc_handler::use_storage(ref_ptr<player> player, packet_reader &reader) -> void {
	if (player->get_shop() == 0) {
		// Hacking
		return;
	}
	int8_t type = reader.get<int8_t>();
	game_mesos cost = channel_server::get_instance().get_npc_data_provider().get_storage_cost(player->get_shop());
	if (cost == 0) {
		// Hacking
		return;
	}
	switch (type) {
		case shop_opcodes::take_item: {
			game_inventory inv = reader.get<game_inventory>();
			game_storage_slot slot = reader.get<game_storage_slot>();
			item *value = player->get_storage()->get_item(slot);
			if (value == nullptr) {
				// Hacking
				return;
			}
			inventory::add_item(player, new item{value});
			player->get_storage()->take_item(slot);
			player->send(packets::storage::take_item(player, inv));
			break;
		}
		case shop_opcodes::store_item: {
			game_inventory_slot slot = reader.get<game_inventory_slot>();
			game_item_id item_id = reader.get<game_item_id>();
			game_slot_qty amount = reader.get<game_slot_qty>();
			if (player->get_inventory()->get_mesos() < cost) {
				// Player doesn't have enough mesos to store this item
				player->send(packets::storage::no_mesos());
				return;
			}
			if (player->get_storage()->is_full()) {
				// Storage is full, so tell the player and abort the mission
				player->send(packets::storage::storage_full());
				return;
			}
			game_inventory inv = game_logic_utilities::get_inventory(item_id);
			item *value = player->get_inventory()->get_item(inv, slot);
			if (value == nullptr) {
				// Hacking
				return;
			}
			if (!game_logic_utilities::is_stackable(item_id)) {
				amount = 1;
			}
			else if (amount <= 0 || amount > value->get_amount()) {
				// Hacking
				return;
			}

			if (value->has_karma()) {
				value->set_karma(false);
			}
			else {
				// TODO FIXME hacking
				// Must validate the Karma state of items here
			}

			player->get_storage()->add_item(
				!game_logic_utilities::is_stackable(item_id) ?
					new item{value} :
					new item{item_id, amount});

			// For equips or rechargeable items (stars/bullets) we create a
			//	new object for storage with the inventory object, and allow
			//	the one in the inventory to go bye bye.
			// Else: For items we just create a new item based on the ID and amount.
			inventory::take_item_slot(
				player,
				inv,
				slot,
				game_logic_utilities::is_rechargeable(item_id) ?
					value->get_amount() :
					amount,
				true,
				true);

			player->get_inventory()->modify_mesos(-cost);
			player->send(packets::storage::add_item(player, inv));
			break;
		}
		case shop_opcodes::meso_transaction: {
			game_mesos mesos = reader.get<game_mesos>();
			// Amount of mesos to remove. Deposits are negative, and withdrawals are positive
			if (mesos < 0) {
				// Transferring from inventory to storage
				if (player->get_inventory()->get_mesos() < -mesos) {
					// Hacking
					return;
				}

				if (player->get_storage()->modify_mesos(-mesos)) {
					player->get_inventory()->modify_mesos(mesos);
				}
				else {
					// TODO FIXME error?
				}
			}
			else {
				// Transferring from storage to inventory
				if (player->get_storage()->get_mesos() < mesos) {
					// Hacking
					return;
				}

				if (player->get_inventory()->modify_mesos(mesos)) {
					player->get_storage()->modify_mesos(-mesos);
				}
				else {
					// TODO FIXME error?
				}
			}

			break;
		}
		case shop_opcodes::exit_storage:
			player->set_shop(0);
			break;
	}
}

auto npc_handler::show_shop(ref_ptr<player> player, game_shop_id shop_id) -> result {
	if (channel_server::get_instance().get_shop_data_provider().is_shop(shop_id)) {
		player->set_shop(shop_id);
		player->send(packets::npc::show_shop(channel_server::get_instance().get_shop_data_provider().get_shop(shop_id), player->get_skills()->get_rechargeable_bonus()));
		return result::successful;
	}
	return result::failure;
}

auto npc_handler::show_storage(ref_ptr<player> player, game_npc_id npc_id) -> result {
	if (channel_server::get_instance().get_npc_data_provider().get_storage_cost(npc_id)) {
		player->set_shop(npc_id);
		player->send(packets::storage::show_storage(player, npc_id));
		return result::successful;
	}
	return result::failure;
}

auto npc_handler::show_guild_rank(ref_ptr<player> player, game_npc_id npc_id) -> result {
	if (channel_server::get_instance().get_npc_data_provider().is_guild_rank(npc_id)) {
		// To be implemented later
	}
	return result::failure;
}

}
}