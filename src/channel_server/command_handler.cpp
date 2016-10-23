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
#include "command_handler.hpp"
#include "common/algorithm.hpp"
#include "common/data/provider/mob.hpp"
#include "common/database.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/inter_header.hpp"
#include "common/packet_wrapper.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/buffs.hpp"
#include "channel_server/chat_handler_functions.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/gm_packet.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/map.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_inventory.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/players_packet.hpp"
#include "channel_server/skills.hpp"
#include "channel_server/world_server_packet.hpp"
#include <string>

namespace vana {
namespace channel_server {

namespace command_opcodes {
	enum opcodes : int8_t {
		find_player = 0x05,
		whisper = 0x06
	};
}

namespace admin_opcodes {
	enum opcodes : int8_t {
		create_item = 0x00,
		destroy_first_item = 0x01,
		give_exp = 0x02,
		ban = 0x03,
		block = 0x04,
		var_set_get = 0x09,
		hide = 0x10,
		show_message_map = 0x11,
		send = 0x12,
		summon = 0x17,
		snow = 0x1C,
		warn = 0x1D,
		log = 0x1E,
		set_obj_state = 0x22,
	};
	/*
		Opcode syntax:
		DestroyFirstItem = /d (inv)
		GiveExp = /exp (amount)
		Ban = /ban (character name)
		Block = /block (character name) (duration) (sort)
		Hide = /h (0 = off, 1 = on)
		Log = /log (character name) (0 = off, 1 = on)
		Send = /send (character name) (mapId)
		Summon = /summon (mobId) (amount)
		VarSetGet = /varset (charactername) (variable name) (variable value)
					/varget (charactername) (variable name)
		Warn = /w (character name) (message)
		Snow = /snow (time in seconds, minimum: 30, maximum: 600)
	*/
}

auto command_handler::handle_command(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t type = reader.get<int8_t>();
	game_chat name = reader.get<game_chat>();
	auto receiver = channel_server::get_instance().get_player_data_provider().get_player(name);
	// If this player doesn't exist, connect to the world server to see if they're on any other channel
	switch (type) {
		case command_opcodes::find_player: {
			bool found = false;
			if (receiver != nullptr) {
				if (!receiver->is_using_gm_hide() || player->is_gm() || player->is_admin()) {
					player->send(packets::players::find_player(receiver->get_name(), receiver->get_map_id()));
					found = true;
				}
			}
			else {
				auto target_data = channel_server::get_instance().get_player_data_provider().get_player_data_by_name(name);
				if (target_data != nullptr && target_data->channel.is_initialized()) {
					player->send(packets::players::find_player(target_data->name, target_data->channel.get(), 1, true));
					found = true;
				}
			}
			if (!found) {
				player->send(packets::players::find_player(name, opt_int32_t{}, 0));
			}
			break;
		}
		case command_opcodes::whisper: {
			game_chat chat = reader.get<game_chat>();
			bool found = false;
			if (receiver != nullptr) {
				receiver->send(packets::players::whisper_player(player->get_name(), channel_server::get_instance().get_channel_id(), chat));
				player->send(packets::players::find_player(receiver->get_name(), opt_int32_t{}, 1));
				found = true;
			}
			else {
				auto target_data = channel_server::get_instance().get_player_data_provider().get_player_data_by_name(name);
				if (target_data != nullptr && target_data->channel.is_initialized()) {
					player->send(packets::players::find_player(target_data->name, opt_int32_t{}, 1));
					channel_server::get_instance().send_world(
						vana::packets::prepend(packets::players::whisper_player(player->get_name(), channel_server::get_instance().get_channel_id(), chat), [target_data](packet_builder &builder) {
							builder.add<packet_header>(IMSG_TO_CHANNEL);
							builder.add<game_channel_id>(target_data->channel.get());
							builder.add<packet_header>(IMSG_TO_PLAYER);
							builder.add<game_player_id>(target_data->id);
						}));
					found = true;
				}
			}
			if (!found) {
				player->send(packets::players::find_player(name, opt_int32_t{}, 0));
			}
			break;
		}
	}
}

auto command_handler::handle_admin_command(ref_ptr<player> player, packet_reader &reader) -> void {
	if (!player->is_admin()) {
		// Hacking
		return;
	}

	int8_t type = reader.get<int8_t>();
	switch (type) {
		case admin_opcodes::hide: {
			bool hide = reader.get<bool>();
			if (hide) {
				auto result = buffs::add_buff(
					player,
					constant::skill::super_gm::hide,
					1,
					0);

				if (result == result::successful) {
					player->send(packets::gm::begin_hide());
					player->get_map()->gm_hide_change(player);
				}
			}
			else {
				skills::stop_skill(
					player,
					data::type::buff_source::from_skill(
						constant::skill::super_gm::hide,
						1));
			}
			break;
		}
		case admin_opcodes::send: {
			game_chat name = reader.get<game_chat>();
			game_map_id map_id = reader.get<game_map_id>();

			if (auto receiver = channel_server::get_instance().get_player_data_provider().get_player(name)) {
				receiver->set_map(map_id);
			}
			else {
				player->send(packets::gm::invalid_character_name());
			}

			break;
		}
		case admin_opcodes::summon: {
			game_mob_id mob_id = reader.get<game_mob_id>();
			int32_t count = reader.get<int32_t>();
			if (channel_server::get_instance().get_mob_data_provider().mob_exists(mob_id)) {
				count = ext::constrain_range(count, 1, 1000);
				for (int32_t i = 0; i < count; ++i) {
					player->get_map()->spawn_mob(mob_id, player->get_pos());
				}
			}
			else {
				chat_handler_functions::show_error(player, "invalid mob: " + std::to_string(mob_id));
			}
			break;
		}
		case admin_opcodes::create_item: {
			game_item_id item_id = reader.get<game_item_id>();
			inventory::add_new_item(player, item_id, 1);
			break;
		}
		case admin_opcodes::destroy_first_item: {
			game_inventory inv = reader.get<game_inventory>();
			if (!game_logic_utilities::is_valid_inventory(inv)) {
				return;
			}
			game_inventory_slot_count slots = player->get_inventory()->get_max_slots(inv);
			for (game_inventory_slot_count i = 0; i < slots; ++i) {
				if (item *item = player->get_inventory()->get_item(inv, i)) {
					inventory::take_item_slot(player, inv, i, player->get_inventory()->get_item_amount_by_slot(inv, i));
					break;
				}
			}
			break;
		}
		case admin_opcodes::give_exp: {
			game_experience amount = reader.get<game_experience>();
			player->get_stats()->give_exp(amount);
			break;
		}
		case admin_opcodes::ban: {
			game_chat victim = reader.get<game_chat>();
			if (auto receiver = channel_server::get_instance().get_player_data_provider().get_player(victim)) {
				receiver->disconnect();
			}
			else {
				player->send(packets::gm::invalid_character_name());
			}
			break;
		}
		case admin_opcodes::block: {
			game_chat victim = reader.get<game_chat>();
			int8_t reason = reader.get<int8_t>();
			int32_t length = reader.get<int32_t>();
			game_chat reason_message = reader.get<game_chat>();
			if (auto receiver = channel_server::get_instance().get_player_data_provider().get_player(victim)) {
				auto &db = database::get_char_db();
				auto &sql = db.get_session();
				sql.once
					<< "UPDATE " << db.make_table("accounts") << " u "
					<< "INNER JOIN " << db.make_table("characters") << " c ON u.account_id = c.account_id "
					<< "SET "
					<< "	u.ban_expire = DATE_ADD(NOW(), INTERVAL :expire DAY),"
					<< "	u.ban_reason = :reason,"
					<< "	u.ban_reasonMessage = :reasonMessage "
					<< "WHERE c.name = :name ",
					soci::use(victim, "name"),
					soci::use(length, "expire"),
					soci::use(reason, "reason"),
					soci::use(reason_message, "reason_message");

				player->send(packets::gm::block());
				game_chat ban_message = victim + " has been banned" + chat_handler_functions::get_ban_string(reason);
				channel_server::get_instance().get_player_data_provider().send(packets::player::show_message(ban_message, packets::player::notice_types::notice));
			}
			else {
				player->send(packets::gm::invalid_character_name());
			}
			break;
		}
		case admin_opcodes::show_message_map:
			player->send(packets::player::show_message(player->get_map()->get_player_names(), packets::player::notice_types::notice));
			break;
		case admin_opcodes::snow:
			player->get_map()->create_weather(player, true, reader.get<int32_t>(), constant::item::snowy_snow, "");
			break;
		case admin_opcodes::var_set_get: {
			int8_t type = reader.get<int8_t>();
			game_chat player_name = reader.get<game_chat>();
			if (auto victim = channel_server::get_instance().get_player_data_provider().get_player(player_name)) {
				game_chat variable_name = reader.get<game_chat>();
				if (type == 0x0a) {
					game_chat variable_value = reader.get<game_chat>();
					victim->get_variables()->set_variable(variable_name, variable_value);
				}
				else {
					player->send(packets::gm::set_get_var_result(player_name, variable_name, victim->get_variables()->get_variable(variable_name)));
				}
			}
			else {
				player->send(packets::gm::invalid_character_name());
			}
			break;
		}
		case admin_opcodes::warn: {
			game_chat victim = reader.get<game_chat>();
			game_chat message = reader.get<game_chat>();
			if (auto receiver = channel_server::get_instance().get_player_data_provider().get_player(victim)) {
				receiver->send(packets::player::show_message(message, packets::player::notice_types::box));
				player->send(packets::gm::warning(true));
			}
			else {
				player->send(packets::gm::warning(false));
			}
			break;
		}

		default:
			reader.reset();
			std::cout << "Unknown GM command: " << reader << std::endl;

			break;
	}
}

}
}