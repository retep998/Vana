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
#include "pet_handler.hpp"
#include "common/data/provider/item.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/util/randomizer.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/map.hpp"
#include "channel_server/move_path.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/pets_packet.hpp"
#include "channel_server/player.hpp"

namespace vana {
namespace channel_server {

auto pet_handler::handle_movement(ref_ptr<player> player, packet_reader &reader) -> void {
	game_pet_id pet_id = reader.get<game_pet_id>();
	pet *pet = player->get_pets()->get_pet(pet_id);
	if (pet == nullptr) {
		// Hacking
		return;
	}

	move_path path(reader);
	pet->reset_from_move_path(path);
	player->send_map(packets::pets::show_movement(player->get_id(), pet, path));
}

auto pet_handler::handle_chat(ref_ptr<player> player, packet_reader &reader) -> void {
	game_pet_id pet_id = reader.get<game_pet_id>();
	if (player->get_pets()->get_pet(pet_id) == nullptr) {
		// Hacking
		return;
	}
	reader.unk<uint8_t>();
	int8_t act = reader.get<int8_t>();
	string message = reader.get<string>();
	player->send_map(packets::pets::show_chat(player->get_id(), player->get_pets()->get_pet(pet_id), message, act));
}

auto pet_handler::handle_summon(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	bool master = reader.get<int8_t>() == 1; // Might possibly fit under get_bool criteria
	bool multipet = player->get_skills()->has_follow_the_lead();
	pet *pet_value = player->get_pets()->get_pet(player->get_inventory()->get_item(constant::inventory::cash, slot)->get_pet_id());

	if (pet_value == nullptr) {
		// Hacking
		return;
	}

	if (pet_value->is_summoned()) {
		// Removing a pet
		int8_t index = pet_value->get_index().get();
		player->get_pets()->set_summoned(index, 0);
		if (index == 0) {
			vana::timer::id id{vana::timer::type::pet_timer, index};
			player->get_timer_container()->remove_timer(id);
		}
		if (multipet) {
			for (int8_t i = index; i < constant::inventory::max_pet_count; ++i) {
				// Shift around pets if using multipet
				if (pet *move = player->get_pets()->get_summoned(i)) {
					move->summon(i - 1);
					int8_t move_index = move->get_index().get();
					player->get_pets()->set_summoned(move_index, move->get_id());
					player->get_pets()->set_summoned(i, 0);
					if (move_index == 0) {
						move->start_timer();
					}
				}
			}
		}

		pet_value->desummon();
		player->send_map(packets::pets::pet_summoned(player->get_id(), pet_value, false, index));
	}
	else {
		// Summoning a Pet
		pet_value->set_pos(player->get_pos());
		if (!multipet || master) {
			pet_value->summon(0);
			if (multipet) {
				for (int8_t i = constant::inventory::max_pet_count - 1; i > 0; --i) {
					if (player->get_pets()->get_summoned(i - 1) && !player->get_pets()->get_summoned(i)) {
						pet *move = player->get_pets()->get_summoned(i - 1);
						player->get_pets()->set_summoned(i, move->get_id());
						player->get_pets()->set_summoned(i - 1, 0);
						move->summon(i);
					}
				}
				player->send_map(packets::pets::pet_summoned(player->get_id(), pet_value));
			}
			else if (pet *kicked = player->get_pets()->get_summoned(0)) {
				vana::timer::id id{vana::timer::type::pet_timer, kicked->get_index().get()};
				player->get_timer_container()->remove_timer(id);
				kicked->desummon();
				player->send_map(packets::pets::pet_summoned(player->get_id(), pet_value, true));
			}
			else {
				player->send_map(packets::pets::pet_summoned(player->get_id(), pet_value));
			}
			player->get_pets()->set_summoned(0, pet_value->get_id());
			pet_value->start_timer();
		}
		else {
			for (int8_t i = 0; i < constant::inventory::max_pet_count; ++i) {
				if (!player->get_pets()->get_summoned(i)) {
					player->get_pets()->set_summoned(i, pet_value->get_id());
					pet_value->summon(i);
					player->send_map(packets::pets::pet_summoned(player->get_id(), pet_value));
					pet_value->start_timer();
					break;
				}
			}
		}
	}
	player->send(packets::pets::blank_update());
}

auto pet_handler::handle_feed(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();
	item *item = player->get_inventory()->get_item(constant::inventory::use, slot);
	pet *pet = player->get_pets()->get_summoned(0);
	if (pet != nullptr && item != nullptr && item->get_id() == item_id) {
		inventory::take_item(player, item_id, 1);

		bool success = (pet->get_fullness() < constant::stat::max_fullness);
		if (success) {
			player->send(packets::pets::show_animation(player->get_id(), pet, 1));

			pet->modify_fullness(constant::stat::pet_feed_fullness, false);
			if (vana::util::randomizer::percentage() < 60) {
				// 60% chance for feed to add closeness
				pet->add_closeness(1);
			}
		}
	}
	else {
		player->send(packets::inventory::blank_update());
	}
}

auto pet_handler::handle_command(ref_ptr<player> player, packet_reader &reader) -> void {
	game_pet_id pet_id = reader.get<game_pet_id>();
	pet *pet = player->get_pets()->get_pet(pet_id);
	if (pet == nullptr) {
		// Hacking
		return;
	}
	reader.unk<uint8_t>();
	int8_t act = reader.get<int8_t>();
	auto action = channel_server::get_instance().get_item_data_provider().get_interaction(pet->get_item_id(), act);
	if (action == nullptr) {
		// Hacking or no action info available
		return;
	}

	if (vana::util::randomizer::percentage<uint32_t>() < action->prob) {
		pet->add_closeness(action->increase);
	}

	player->send(packets::pets::show_animation(player->get_id(), pet, act));
}

auto pet_handler::handle_consume_potion(ref_ptr<player> player, packet_reader &reader) -> void {
	game_pet_id pet_id = reader.get<game_pet_id>();
	pet *pet = player->get_pets()->get_pet(pet_id);
	if (pet == nullptr || !pet->is_summoned() || player->get_stats()->is_dead()) {
		// Hacking
		return;
	}
	reader.unk<uint8_t>(); // It MIGHT be some flag for Meso/Power/Magic Guard...?
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();
	item *item = player->get_inventory()->get_item(constant::inventory::use, slot);
	auto info = channel_server::get_instance().get_item_data_provider().get_consume_info(item_id);
	if (item == nullptr || item->get_id() != item_id) {
		// Hacking
		return;
	}

	// Check if the MP potion is an MP potion set
	if ((info->mp != 0 || info->mp_rate != 0) && player->get_inventory()->get_auto_mp_pot() != item_id) {
		// Hacking
		return;
	}

	// Check if the HP potion is an HP potion set
	if ((info->hp != 0 || info->hp_rate != 0) && player->get_inventory()->get_auto_hp_pot() != item_id) {
		// Hacking
		return;
	}

	inventory::use_item(player, item_id);
	inventory::take_item_slot(player, constant::inventory::use, slot, 1);
}

auto pet_handler::change_name(ref_ptr<player> player, const string &name) -> void {
	if (pet *pet = player->get_pets()->get_summoned(0)) {
		pet->set_name(name);
	}
}

auto pet_handler::show_pets(ref_ptr<player> player) -> void {
	for (int8_t i = 0; i < constant::inventory::max_pet_count; ++i) {
		if (pet *pet = player->get_pets()->get_summoned(i)) {
			pet->set_pos(player->get_pos());
			player->send(packets::pets::pet_summoned(player->get_id(), pet));
		}
	}
	player->send(packets::pets::update_summoned_pets(player));
}

}
}