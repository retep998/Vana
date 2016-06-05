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
#include "InventoryHandler.hpp"
#include "common/CurseDataProvider.hpp"
#include "common/EquipDataProvider.hpp"
#include "common/GameLogicUtilities.hpp"
#include "common/InterHeader.hpp"
#include "common/ItemConstants.hpp"
#include "common/ItemDataProvider.hpp"
#include "common/MobDataProvider.hpp"
#include "common/NpcDataProvider.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWrapper.hpp"
#include "common/Randomizer.hpp"
#include "common/ScriptDataProvider.hpp"
#include "common/ShopDataProvider.hpp"
#include "common/ValidCharDataProvider.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Drop.hpp"
#include "channel_server/Inventory.hpp"
#include "channel_server/InventoryPacket.hpp"
#include "channel_server/MapleTvs.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Pet.hpp"
#include "channel_server/PetHandler.hpp"
#include "channel_server/PetsPacket.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/ReactorHandler.hpp"

namespace vana {
namespace channel_server {

auto inventory_handler::move_item(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory inv = reader.get<game_inventory>();
	game_inventory_slot slot1 = reader.get<game_inventory_slot>();
	game_inventory_slot slot2 = reader.get<game_inventory_slot>();
	bool dropped = (slot2 == 0);
	bool equipped_slot1 = (slot1 < 0);
	bool equipped_slot2 = (slot2 < 0);
	game_inventory_slot stripped_slot1 = game_logic_utilities::strip_cash_slot(slot1);
	game_inventory_slot stripped_slot2 = game_logic_utilities::strip_cash_slot(slot2);
	auto test_slot = [&slot1, &slot2, &stripped_slot1, &stripped_slot2](game_inventory_slot test_slot) -> bool {
		return (slot1 < 0 && stripped_slot1 == test_slot) || (slot2 < 0 && stripped_slot2 == test_slot);
	};

	if (dropped) {
		item *item1 = player->get_inventory()->get_item(inv, slot1);
		if (item1 == nullptr) {
			return;
		}
		inventory_handler::drop_item(player, reader, item1, slot1, inv);
	}
	else {
		player->get_inventory()->swap_items(inv, slot1, slot2);
	}

	if (equipped_slot1 || equipped_slot2) {
		auto test_pet_slot = [&player, &test_slot](int16_t equip_slot, int32_t pet_index) {
			if (test_slot(equip_slot)) {
				if (pet *pet = player->get_pets()->get_summoned(pet_index)) {
					player->send_map(packets::pets::change_name(player->get_id(), pet));
				}
			}
		};
		// Check if any label ring changed, so we can update the look of the apropos pet
		test_pet_slot(equip_slots::pet_label_ring1, 0);
		test_pet_slot(equip_slots::pet_label_ring2, 1);
		test_pet_slot(equip_slots::pet_label_ring3, 2);

		player->send_map(packets::inventory::update_player(player));
	}
}

auto inventory_handler::drop_item(ref_ptr<player> player, packet_reader &reader, item *item_value, game_inventory_slot slot, game_inventory inv) -> void {
	game_slot_qty amount = reader.get<game_slot_qty>();
	if (!game_logic_utilities::is_stackable(item_value->get_id())) {
		amount = item_value->get_amount();
	}
	else if (amount <= 0 || amount > item_value->get_amount()) {
		// Hacking
		return;
	}
	else if (item_value->has_lock()) {
		// Hacking
		return;
	}
	if (game_logic_utilities::is_gm_equip(item_value->get_id()) || item_value->has_lock()) {
		// We don't allow these to be dropped or traded
		return;
	}

	item dropped_item(item_value);
	dropped_item.set_amount(amount);
	if (item_value->get_amount() == amount) {
		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::modify_slot, item_value, slot);
		player->send(packets::inventory::inventory_operation(true, ops));

		player->get_inventory()->delete_item(inv, slot);
	}
	else {
		item_value->dec_amount(amount);
		player->get_inventory()->change_item_amount(item_value->get_id(), -amount);

		vector<inventory_packet_operation> ops;
		ops.emplace_back(packets::inventory::operation_types::modify_quantity, item_value, slot);
		player->send(packets::inventory::inventory_operation(true, ops));
	}

	auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(dropped_item.get_id());
	bool is_tradeable = dropped_item.has_karma() || !(dropped_item.has_trade_block() || item_info->quest || item_info->no_trade);
	drop *drop_value = new drop{player->get_map_id(), dropped_item, player->get_pos(), player->get_id(), true};
	drop_value->set_time(0);
	drop_value->set_tradeable(is_tradeable);
	drop_value->do_drop(player->get_pos());

	if (is_tradeable) {
		// Drop is deleted otherwise, avoid like plague
		reactor_handler::check_drop(player, drop_value);
	}
}

auto inventory_handler::use_item(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();
	const game_slot_qty amount_used = 1;
	if (player->get_stats()->is_dead() || player->get_inventory()->get_item_amount_by_slot(inventories::use, slot) < amount_used) {
		// Hacking
		return;
	}

	item *item_value = player->get_inventory()->get_item(inventories::use, slot);
	if (item_value == nullptr || item_value->get_id() != item_id) {
		// Hacking
		return;
	}

	if (!player->has_gm_benefits()) {
		inventory::take_item_slot(player, inventories::use, slot, amount_used);
	}

	inventory::use_item(player, item_id);
}

auto inventory_handler::cancel_item(ref_ptr<player> player, packet_reader &reader) -> void {
	game_item_id item_id = reader.get<game_item_id>();
	buffs::end_buff(player, player->get_active_buffs()->translate_to_source(item_id));
}

auto inventory_handler::use_skillbook(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();

	item *item = player->get_inventory()->get_item(inventories::use, slot);
	if (item == nullptr || item->get_id() != item_id) {
		// Hacking
		return;
	}

	auto skillbook_items = channel_server::get_instance().get_item_data_provider().get_item_skills(item_id);
	if (skillbook_items == nullptr) {
		// Hacking
		return;
	}

	game_skill_id skill_id = 0;
	game_skill_level new_max_level = 0;
	bool use = false;
	bool succeed = false;

	for (const auto &s : *skillbook_items) {
		skill_id = s.skill_id;
		new_max_level = s.max_level;
		if (game_logic_utilities::item_skill_matches_job(skill_id, player->get_stats()->get_job())) {
			if (player->get_skills()->get_skill_level(skill_id) >= s.req_level) {
				if (player->get_skills()->get_max_skill_level(skill_id) < new_max_level) {
					if (randomizer::percentage<int8_t>() < s.chance) {
						player->get_skills()->set_max_skill_level(skill_id, new_max_level);
						succeed = true;
					}

					inventory::take_item_slot(player, inventories::use, slot, 1);
					break;
				}
			}
		}
	}

	if (skill_id != 0) {
		player->send_map(packets::inventory::use_skillbook(player->get_id(), skill_id, new_max_level, true, succeed));
	}
}

auto inventory_handler::use_chair(ref_ptr<player> player, packet_reader &reader) -> void {
	game_item_id chair_id = reader.get<game_item_id>();
	player->set_chair(chair_id);
	player->send_map(packets::inventory::sit_chair(player->get_id(), chair_id));
}

auto inventory_handler::handle_chair(ref_ptr<player> player, packet_reader &reader) -> void {
	game_seat_id chair = reader.get<game_seat_id>();
	map *map = player->get_map();
	if (chair == -1) {
		if (player->get_chair() != 0) {
			player->set_chair(0);
		}
		else {
			map->player_seated(player->get_map_chair(), nullptr);
			player->set_map_chair(0);
		}
		map->send(packets::inventory::stop_chair(player->get_id(), false), player);
	}
	else {
		// Map chair
		if (map->seat_occupied(chair)) {
			map->send(packets::inventory::stop_chair(player->get_id(), true), player);
		}
		else {
			map->player_seated(chair, player);
			player->set_map_chair(chair);
			player->send(packets::inventory::sit_map_chair(chair));
		}
	}
}

auto inventory_handler::use_summon_bag(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();

	auto item_info = channel_server::get_instance().get_item_data_provider().get_item_summons(item_id);
	if (item_info == nullptr) {
		// Most likely hacking
		return;
	}

	item *inventory_item = player->get_inventory()->get_item(inventories::use, slot);
	if (inventory_item == nullptr || inventory_item->get_id() != item_id) {
		// Hacking
		return;
	}

	inventory::take_item_slot(player, inventories::use, slot, 1);

	for (const auto &bag : *item_info) {
		if (randomizer::percentage<uint32_t>() < bag.chance) {
			if (channel_server::get_instance().get_mob_data_provider().mob_exists(bag.mob_id)) {
				player->get_map()->spawn_mob(bag.mob_id, player->get_pos());
			}
		}
	}
}

auto inventory_handler::use_return_scroll(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();

	item *item = player->get_inventory()->get_item(inventories::use, slot);
	if (item == nullptr || item->get_id() != item_id) {
		// Hacking
		return;
	}
	auto info = channel_server::get_instance().get_item_data_provider().get_consume_info(item_id);
	if (info == nullptr) {
		// Probably hacking
		return;
	}

	inventory::take_item_slot(player, inventories::use, slot, 1);

	game_map_id map = info->move_to;
	player->set_map(map == vana::maps::no_map ? player->get_map()->get_return_map() : map);
}

auto inventory_handler::use_scroll(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_inventory_slot equip_slot = reader.get<game_inventory_slot>();
	bool white_scroll = (reader.get<int16_t>() == 2);
	bool legendary_spirit = reader.get<bool>();

	item *scroll = player->get_inventory()->get_item(inventories::use, slot);
	item *equip = player->get_inventory()->get_item(inventories::equip, equip_slot);
	if (scroll == nullptr || equip == nullptr) {
		// Most likely hacking
		return;
	}
	if (legendary_spirit && !player->get_skills()->has_legendary_spirit()) {
		// Hacking
		return;
	}
	if (!legendary_spirit && equip_slot > 0) {
		// Hacking
		return;
	}

	game_item_id item_id = scroll->get_id();
	int8_t succeed = -1;
	bool cursed = false;

	if (legendary_spirit && equip->get_slots() == 0) {
		// This is actually allowed by the game for some reason, the server is expected to send an error
	}
	else if (hacking_result::not_hacking != channel_server::get_instance().get_item_data_provider().scroll_item(channel_server::get_instance().get_equip_data_provider(), item_id, equip, white_scroll, player->has_gm_benefits(), succeed, cursed)) {
		return;
	}

	if (succeed != -1) {
		if (white_scroll) {
			inventory::take_item(player, items::white_scroll, 1);
		}

		inventory::take_item_slot(player, inventories::use, slot, 1);
		player->send_map(packets::inventory::use_scroll(player->get_id(), succeed, cursed, legendary_spirit));

		if (!cursed) {
			player->get_stats()->set_equip(equip_slot, equip);

			vector<inventory_packet_operation> ops;
			ops.emplace_back(packets::inventory::operation_types::add_item, equip, equip_slot);
			player->send(packets::inventory::inventory_operation(true, ops));
		}
		else {
			vector<inventory_packet_operation> ops;
			ops.emplace_back(packets::inventory::operation_types::modify_slot, equip, equip_slot);
			player->send(packets::inventory::inventory_operation(true, ops));

			player->get_inventory()->delete_item(inventories::equip, equip_slot);
		}

		player->send_map(packets::inventory::update_player(player));
	}
	else {
		if (legendary_spirit) {
			player->send_map(packets::inventory::use_scroll(player->get_id(), succeed, cursed, legendary_spirit));
		}
		player->send(packets::inventory::blank_update());
	}
}

auto inventory_handler::use_buff_item(ref_ptr<player> player, packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();
	item *item = player->get_inventory()->get_item(inventories::use, slot);
	if (item == nullptr || item->get_id() != item_id) {
		// Hacking
		return;
	}

	string target_name = reader.get<string>();
	if (auto target = channel_server::get_instance().get_player_data_provider().get_player(target_name)) {
		if (target == player) {
			// TODO FIXME packet
			// Need failure packet here (if it is a failure)
		}
		if (player->get_map_id() != target->get_map_id()) {
			// TODO FIXME packet
			// Need failure packet here
		}
		else {
			inventory::use_item(target, item_id);
			inventory::take_item(player, item_id, 1);
		}
	}
	else {
		// TODO FIXME packet
		// Need failure packet here
	}
}

auto inventory_handler::use_cash_item(ref_ptr<player> player, packet_reader &reader) -> void {
	game_inventory_slot item_slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();

	item *test = player->get_inventory()->get_item(inventories::cash, item_slot);
	if (test == nullptr || test->get_id() != item_id) {
		// Hacking
		return;
	}

	auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id);
	bool used = false;
	if (game_logic_utilities::get_item_type(item_id) == items::types::weather_cash) {
		string message = reader.get<string>();
		reader.skip<game_tick_count>();
		if (message.length() <= 35) {
			map *map = player->get_map();
			message = player->get_name() + " 's message : " + message;
			used = map->create_weather(player, false, items::weather_time, item_id, message);
		}
	}
	else if (game_logic_utilities::get_item_type(item_id) == items::types::cash_pet_food) {
		pet *pet = player->get_pets()->get_summoned(0);
		if (pet != nullptr) {
			if (pet->get_fullness() < stats::max_fullness) {
				player->send(packets::pets::show_animation(player->get_id(), pet, 1));
				pet->modify_fullness(stats::max_fullness, false);
				pet->add_closeness(100); // All cash pet food gives 100 closeness
				used = true;
			}
		}
	}
	else {
		switch (item_id) {
			case items::teleport_rock:
			case items::teleport_coke:
			case items::vip_rock: // Only occurs when you actually try to move somewhere
				used = handle_rock_teleport(player, item_id, reader);
				break;
			case items::first_job_sp_reset:
			case items::second_job_sp_reset:
			case items::third_job_sp_reset:
			case items::fourth_job_sp_reset: {
				game_skill_id to_skill = reader.get<int32_t>();
				game_skill_id from_skill = reader.get<int32_t>();
				if (!player->get_skills()->add_skill_level(from_skill, -1, true)) {
					// Hacking
					return;
				}
				if (!player->get_skills()->add_skill_level(to_skill, 1, true)) {
					// Hacking
					return;
				}
				used = true;
				break;
			}
			case items::ap_reset: {
				int32_t to_stat = reader.get<int32_t>();
				int32_t from_stat = reader.get<int32_t>();
				player->get_stats()->add_stat(to_stat, 1, true);
				player->get_stats()->add_stat(from_stat, -1, true);
				used = true;
				break;
			}
			case items::megaphone: {
				string msg = player->get_medal_name() + " : " + reader.get<string>();
				// In global, this sends to everyone on the current channel, not the map
				channel_server::get_instance().get_player_data_provider().send(packets::inventory::show_megaphone(msg));
				used = true;
				break;
			}
			case items::super_megaphone: {
				string msg = player->get_medal_name() + " : " + reader.get<string>();
				bool whisper = reader.get<bool>();
				auto &builder = packets::inventory::show_super_megaphone(msg, whisper);
				channel_server::get_instance().send_world(
					vana::packets::prepend(builder, [](packet_builder &header) {
						header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
					}));
				used = true;
				break;
			}
			case items::diablo_messenger:
			case items::cloud9_messenger:
			case items::loveholic_messenger: {
				string msg1 = reader.get<string>();
				string msg2 = reader.get<string>();
				string msg3 = reader.get<string>();
				string msg4 = reader.get<string>();

				auto &builder = packets::inventory::show_messenger(player->get_name(), msg1, msg2, msg3, msg4, reader.get_buffer(), reader.get_buffer_length(), item_id);
				channel_server::get_instance().send_world(
					vana::packets::prepend(builder, [](packet_builder &header) {
						header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
					}));
				used = true;
				break;
			}
			case items::item_megaphone: {
				string msg = player->get_medal_name() + " : " + reader.get<string>();
				bool whisper = reader.get<bool>();
				item *item = nullptr;
				if (reader.get<bool>()) {
					game_inventory inv = static_cast<game_inventory>(reader.get<int32_t>());
					game_inventory_slot slot = static_cast<game_inventory_slot>(reader.get<int32_t>());
					item = player->get_inventory()->get_item(inv, slot);
					if (item == nullptr) {
						// Hacking
						return;
					}
				}

				auto &builder = packets::inventory::show_item_megaphone(msg, whisper, item);
				channel_server::get_instance().send_world(
					vana::packets::prepend(builder, [](packet_builder &header) {
						header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
					}));
				used = true;
				break;
			}
			case items::art_megaphone: {
				int8_t lines = reader.get<int8_t>();
				if (lines < 1 || lines > 3) {
					// Hacking
					return;
				}
				string text[3];
				for (int8_t i = 0; i < lines; i++) {
					text[i] = player->get_medal_name() + " : " + reader.get<string>();
				}

				bool whisper = reader.get<bool>();
				auto &builder = packets::inventory::show_triple_megaphone(lines, text[0], text[1], text[2], whisper);
				channel_server::get_instance().send_world(
					vana::packets::prepend(builder, [](packet_builder &header) {
						header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
					}));

				used = true;
				break;
			}
			case items::pet_name_tag: {
				string name = reader.get<string>();
				if (channel_server::get_instance().get_valid_char_data_provider().is_forbidden_name(name) ||
					channel_server::get_instance().get_curse_data_provider().is_curse_word(name)) {
					// Don't think it's hacking, but it should be forbidden
					return;
				}
				else {
					pet_handler::change_name(player, name);
					used = true;
				}
				break;
			}
			case items::item_name_tag: {
				game_inventory_slot slot = reader.get<game_inventory_slot>();
				if (slot != 0) {
					item *item = player->get_inventory()->get_item(inventories::equip, slot);
					if (item == nullptr) {
						// Hacking or failure, dunno
						return;
					}
					item->set_name(player->get_name());

					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::add_item, item, slot);
					player->send(packets::inventory::inventory_operation(true, ops));
					used = true;
				}
				break;
			}
			case items::scissors_of_karma:
			case items::item_lock: {
				game_inventory inv = static_cast<game_inventory>(reader.get<int32_t>());
				game_inventory_slot slot = static_cast<game_inventory_slot>(reader.get<int32_t>());
				if (slot != 0) {
					item *item = player->get_inventory()->get_item(inv, slot);
					if (item == nullptr) {
						// Hacking or failure, dunno
						return;
					}
					switch (item_id) {
						case items::item_lock:
							if (item->has_lock()) {
								// Hacking
								return;
							}
							item->set_lock(true);
							break;
						case items::scissors_of_karma: {
							auto equip_info = channel_server::get_instance().get_item_data_provider().get_item_info(item->get_id());

							if (!equip_info->karma_scissors) {
								// Hacking
								return;
							}
							if (item->has_trade_block() || item->has_karma()) {
								// Hacking
								return;
							}
							item->set_karma(true);
							break;
						}
					}

					vector<inventory_packet_operation> ops;
					ops.emplace_back(packets::inventory::operation_types::add_item, item, slot);
					player->send(packets::inventory::inventory_operation(true, ops));
					used = true;
				}
				break;
			}
			case items::maple_tv_messenger:
			case items::megassenger: {
				bool has_receiver = (reader.get<int8_t>() == 3);
				bool show_whisper = (item_id == items::megassenger ? reader.get<bool>() : false);
				auto receiver = channel_server::get_instance().get_player_data_provider().get_player(reader.get<string>());
				int32_t time = 15;

				if ((has_receiver && receiver != nullptr) || (!has_receiver && receiver == nullptr)) {
					string msg1 = reader.get<string>();
					string msg2 = reader.get<string>();
					string msg3 = reader.get<string>();
					string msg4 = reader.get<string>();
					string msg5 = reader.get<string>();
					reader.skip<game_tick_count>();

					channel_server::get_instance().get_maple_tvs().add_message(player, receiver, msg1, msg2, msg3, msg4, msg5, item_id - (item_id == items::megassenger ? 3 : 0), time);

					if (item_id == items::megassenger) {
						auto &builder = packets::inventory::show_super_megaphone(player->get_medal_name() + " : " + msg1 + msg2 + msg3 + msg4 + msg5, show_whisper);
						channel_server::get_instance().send_world(
							vana::packets::prepend(builder, [](packet_builder &header) {
								header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
							}));
					}
					used = true;
				}
				break;
			}
			case items::maple_tv_star_messenger:
			case items::star_megassenger: {
				int32_t time = 30;
				bool show_whisper = (item_id == items::star_megassenger ? reader.get<bool>() : false);
				string msg1 = reader.get<string>();
				string msg2 = reader.get<string>();
				string msg3 = reader.get<string>();
				string msg4 = reader.get<string>();
				string msg5 = reader.get<string>();
				reader.skip<game_tick_count>();

				channel_server::get_instance().get_maple_tvs().add_message(player, nullptr, msg1, msg2, msg3, msg4, msg5, item_id - (item_id == items::star_megassenger ? 3 : 0), time);

				if (item_id == items::star_megassenger) {
					auto &builder = packets::inventory::show_super_megaphone(player->get_medal_name() + " : " + msg1 + msg2 + msg3 + msg4 + msg5, show_whisper);
					channel_server::get_instance().send_world(
						vana::packets::prepend(builder, [](packet_builder &header) {
							header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
						}));
				}
				used = true;
				break;
			}
			case items::maple_tv_heart_messenger:
			case items::heart_megassenger: {
				bool show_whisper = (item_id == items::heart_megassenger ? reader.get<bool>() : false);
				string name = reader.get<string>();
				auto receiver = channel_server::get_instance().get_player_data_provider().get_player(name);
				int32_t time = 60;

				if (receiver != nullptr) {
					string msg1 = reader.get<string>();
					string msg2 = reader.get<string>();
					string msg3 = reader.get<string>();
					string msg4 = reader.get<string>();
					string msg5 = reader.get<string>();
					reader.skip<game_tick_count>();

					channel_server::get_instance().get_maple_tvs().add_message(player, receiver, msg1, msg2, msg3, msg4, msg5, item_id - (item_id == items::heart_megassenger ? 3 : 0), time);

					if (item_id == items::heart_megassenger) {
						auto &builder = packets::inventory::show_super_megaphone(player->get_medal_name() + " : " + msg1 + msg2 + msg3 + msg4 + msg5, show_whisper);
						channel_server::get_instance().send_world(
							vana::packets::prepend(builder, [](packet_builder &header) {
								header.add<packet_header>(IMSG_TO_ALL_PLAYERS);
							}));
					}
					used = true;
				}
				break;
			}
			case items::bronze_sack_of_mesos:
			case items::silver_sack_of_mesos:
			case items::gold_sack_of_mesos: {
				game_mesos mesos = item_info->mesos;
				if (!player->get_inventory()->modify_mesos(mesos)) {
					player->send(packets::inventory::send_mesobag_failed());
				}
				else {
					player->send(packets::inventory::send_mesobag_succeed(mesos));
					used = true;
				}
				break;
			}
			case items::chalkboard:
			case items::chalkboard2: {
				string msg = reader.get<string>();
				player->set_chalkboard(msg);
				player->send_map(packets::inventory::send_chalkboard_update(player->get_id(), msg));
				break;
			}
			case items::fungus_scroll:
			case items::oinker_delight:
			case items::zeta_nightmare:
				inventory::use_item(player, item_id);
				used = true;
				break;
			case items::vicious_hammer: {
				game_inventory inv = static_cast<game_inventory>(reader.get<int32_t>());
				game_inventory_slot slot = static_cast<game_inventory_slot>(reader.get<int32_t>());
				item *item = player->get_inventory()->get_item(inv, slot);
				if (item == nullptr || item->get_hammers() == items::max_hammers || channel_server::get_instance().get_equip_data_provider().get_slots(item->get_id()) == 0) {
					// Hacking, probably
					return;
				}
				item->inc_hammers();
				item->inc_slots();
				player->send(packets::inventory::send_hammer_slots(item->get_hammers()));
				player->get_inventory()->set_hammer_slot(slot);
				used = true;
				break;
			}
			case items::congratulatory_song:
				player->send_map(packets::inventory::play_cash_song(item_id, player->get_name()));
				used = true;
				break;
		}
	}
	if (used) {
		inventory::take_item(player, item_id, 1);
	}
	else {
		player->send(packets::inventory::blank_update());
	}
}

auto inventory_handler::use_item_effect(ref_ptr<player> player, packet_reader &reader) -> void {
	game_item_id item_id = reader.get<game_item_id>();
	if (player->get_inventory()->get_item_amount(item_id) == 0) {
		// Hacking
		return;
	}
	player->set_item_effect(item_id);
	player->send_map(packets::inventory::use_item_effect(player->get_id(), item_id));
}

auto inventory_handler::handle_rock_functions(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t mode = reader.get<int8_t>();
	int8_t type = reader.get<int8_t>();

	enum modes : int8_t {
		remove = 0x00,
		add = 0x01
	};

	if (mode == remove) {
		game_map_id map = reader.get<game_map_id>();
		player->get_inventory()->del_rock_map(map, type);
	}
	else if (mode == add) {
		game_map_id map_id = player->get_map_id();
		map *m = maps::get_map(map_id);
		if (m->can_vip() && m->get_continent() != 0) {
			player->get_inventory()->add_rock_map(map_id, type);
		}
		else {
			// Hacking, the client doesn't allow this to occur
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::cannot_save_map, type));
		}
	}
}

auto inventory_handler::handle_rock_teleport(ref_ptr<player> player, game_item_id item_id, packet_reader &reader) -> bool {
	if (item_id == items::special_teleport_rock) {
		game_inventory_slot slot = reader.get<game_inventory_slot>();
		game_item_id posted_item_id = reader.get<game_item_id>();

		item *item = player->get_inventory()->get_item(inventories::use, slot);
		if (item == nullptr || item->get_id() != item_id || item_id != posted_item_id) {
			// Hacking
			return false;
		}
	}

	int8_t type = item_id != items::vip_rock ?
		packets::inventory::rock_types::regular :
		packets::inventory::rock_types::vip;

	bool used = false;
	int8_t mode = reader.get<int8_t>();
	game_map_id target_map_id = -1;

	enum modes : int8_t {
		preset_map = 0x00,
		ign = 0x01
	};
	if (mode == preset_map) {
		target_map_id = reader.get<game_map_id>();
		if (!player->get_inventory()->ensure_rock_destination(target_map_id)) {
			// Hacking
			return false;
		}
	}
	else if (mode == ign) {
		string target_name = reader.get<string>();
		auto target = channel_server::get_instance().get_player_data_provider().get_player(target_name);
		if (target != nullptr && target != player) {
			target_map_id = target->get_map_id();
		}
		else if (target == nullptr) {
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::difficult_to_locate, type));
		}
		else if (target == player) {
			// Hacking
			return false;
		}
	}
	if (target_map_id != -1) {
		map *destination = maps::get_map(target_map_id);
		map *origin = player->get_map();
		if (!destination->can_vip()) {
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::cannot_go, type));
		}
		else if (!origin->can_vip()) {
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::cannot_go, type));
		}
		else if (player->get_map_id() == target_map_id) {
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::already_there, type));
		}
		else if (type == 0 && destination->get_continent() != origin->get_continent()) {
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::cannot_go, type));
		}
		else if (player->get_stats()->get_level() < 7 && origin->get_continent() == 0 && destination->get_continent() != 0) {
			player->send(packets::inventory::send_rock_error(packets::inventory::rock_errors::noobs_cannot_leave_maple_island, type));
		}
		else {
			player->set_map(target_map_id);
			used = true;
		}
	}

	reader.skip<game_tick_count>();

	if (item_id == items::special_teleport_rock) {
		if (used) {
			inventory::take_item(player, item_id, 1);
		}
		else {
			player->send(packets::inventory::blank_update());
		}
	}
	return used;
}

auto inventory_handler::handle_hammer_time(ref_ptr<player> player) -> void {
	if (!player->get_inventory()->is_hammering()) {
		// Hacking
		return;
	}
	game_inventory_slot hammer_slot = player->get_inventory()->get_hammer_slot();
	item *item = player->get_inventory()->get_item(inventories::equip, hammer_slot);
	player->send(packets::inventory::send_hammer_update());
	player->send(packets::inventory::send_hulk_smash(hammer_slot, item));
	player->get_inventory()->set_hammer_slot(-1);
}

auto inventory_handler::handle_reward_item(ref_ptr<player> player, packet_reader &reader) -> void {
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();
	item *reward_item_cons = player->get_inventory()->get_item(inventories::use, slot);
	if (reward_item_cons == nullptr || reward_item_cons->get_id() != item_id) {
		// Hacking or hacking failure
		player->send(packets::inventory::blank_update());
		return;
	}

	auto rewards = channel_server::get_instance().get_item_data_provider().get_item_rewards(item_id);
	if (rewards == nullptr) {
		// Hacking or no information in the database
		player->send(packets::inventory::blank_update());
		return;
	}

	auto reward = randomizer::select(rewards);
	inventory::take_item(player, item_id, 1);
	item *reward_item = new item{reward->reward_id, reward->quantity};
	inventory::add_item(player, reward_item, true);
	player->send_map(packets::inventory::send_reward_item_animation(player->get_id(), item_id, reward->effect));
}

auto inventory_handler::handle_script_item(ref_ptr<player> player, packet_reader &reader) -> void {
	if (player->get_npc() != nullptr || player->get_shop() != 0 || player->get_trade_id() != 0) {
		// Hacking
		player->send(packets::inventory::blank_update());
		return;
	}

	reader.skip<game_tick_count>();
	game_inventory_slot slot = reader.get<game_inventory_slot>();
	game_item_id item_id = reader.get<game_item_id>();

	item *item = player->get_inventory()->get_item(inventories::use, slot);
	if (item == nullptr || item->get_id() != item_id) {
		// Hacking or hacking failure
		player->send(packets::inventory::blank_update());
		return;
	}

	auto &channel = channel_server::get_instance();
	string script_name = channel.get_script_data_provider().get_script(&channel, item_id, script_types::item);
	if (script_name.empty()) {
		// Hacking or no script for item found
		player->send(packets::inventory::blank_update());
		return;
	}

	game_npc_id npc_id = channel.get_item_data_provider().get_item_info(item_id)->npc;

	// Let's run the NPC
	npc *script_npc = new npc{npc_id, player, script_name};
	if (!script_npc->check_end()) {
		// NPC is running/script found
		// Delete the item used
		inventory::take_item(player, item_id, 1);
		script_npc->run();
	}
	else {
		// NPC didn't run/no script found
		player->send(packets::inventory::blank_update());
	}
}

}
}