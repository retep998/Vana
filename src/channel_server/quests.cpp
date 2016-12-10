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
#include "quests.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/provider/npc.hpp"
#include "common/data/provider/quest.hpp"
#include "common/packet_reader.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/npc_handler.hpp"
#include "channel_server/player.hpp"
#include "channel_server/quests_packet.hpp"

namespace vana {
namespace channel_server {

namespace quest_opcodes {
	enum : int8_t {
		restore_lost_quest_item = 0x00,
		start_quest = 0x01,
		finish_quest = 0x02,
		forfeit_quest = 0x03,
		start_npc_quest_chat = 0x04,
		end_npc_quest_chat = 0x05
	};
}

auto quests::give_item(ref_ptr<player> player, game_item_id item_id, game_slot_qty amount, stat_variance variance_policy) -> result {
	if (amount > 0) {
		if (!player->get_inventory()->has_open_slots_for(item_id, amount)) {
			return result::failure;
		}
		inventory::add_new_item(player, item_id, amount, variance_policy);
	}
	else {
		if (player->get_inventory()->get_item_amount(item_id) < -amount) {
			// Player does not have (enough of) what is being taken
			return result::failure;
		}
		inventory::take_item(player, item_id, -amount);
	}

	player->send(packets::quests::give_item(item_id, amount));
	return result::success;
}

auto quests::give_mesos(ref_ptr<player> player, game_mesos amount) -> result {
	if (player->get_inventory()->modify_mesos(amount).get_result() != stack_result::full) {
		return result::failure;
	}

	player->send(packets::quests::give_mesos(amount));
	return result::success;
}

auto quests::give_fame(ref_ptr<player> player, game_fame amount) -> result {
	player->get_stats()->set_fame(player->get_stats()->get_fame() + amount);
	player->send(packets::quests::give_fame(amount));
	return result::success;
}

auto quests::get_quest(ref_ptr<player> player, packet_reader &reader) -> void {
	int8_t act = reader.get<int8_t>();
	game_quest_id quest_id = reader.get<game_quest_id>();

	if (!channel_server::get_instance().get_quest_data_provider().is_quest(quest_id)) {
		// Hacking
		return;
	}
	if (act == quest_opcodes::forfeit_quest) {
		if (player->get_quests()->is_quest_active(quest_id)) {
			player->get_quests()->remove_quest(quest_id);
		}
		else {
			channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &log) {
				log << "Player (ID: " << player->get_id()
					<< ", Name: " << player->get_name()
					<< ") tried to forfeit a quest that wasn't started yet."
					<< " (Quest ID: " << quest_id << ")";
			});
		}
		return;
	}

	game_npc_id npc_id = reader.get<game_npc_id>();
	if (act != quest_opcodes::start_quest && act != quest_opcodes::start_npc_quest_chat) {
		if (!player->get_quests()->is_quest_active(quest_id)) {
			// Hacking
			channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &log) {
				log << "Player (ID: " << player->get_id()
					<< ", Name: " << player->get_name()
					<< ") tried to perform an action with a non-started quest."
					<< " (Quest ID: " << quest_id << ")";
			});
			return;
		}
	}
	// quest_opcodes::restore_lost_quest_item for some reason appears to use "NPC ID" as a different kind of identifier, maybe quantity?

	if (act != quest_opcodes::restore_lost_quest_item && !channel_server::get_instance().get_npc_data_provider().is_valid_npc_id(npc_id)) {
		channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &log) {
			log << "Player (ID: " << player->get_id()
				<< ", Name: " << player->get_name()
				<< ") tried to do a quest action with an invalid NPC ID."
				<< " (NPC ID: " << npc_id
				<< " (Quest ID: " << quest_id << ")";
		});
		return;
	}

	switch (act) {
		case quest_opcodes::restore_lost_quest_item: {
			game_item_id item_id = reader.get<game_item_id>();
			auto item_info = channel_server::get_instance().get_item_data_provider().get_item_info(item_id);
			if (item_info == nullptr) {
				// Hacking
				return;
			}

			if (item_info->quest) {
				player->send(packets::quests::give_item(item_id, 1));
				inventory::add_new_item(player, item_id, 1);
			}
			else {
				channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &log) {
					log << "Player (ID: " << player->get_id()
						<< ", Name: " << player->get_name()
						<< ") tried to restore a lost quest item which isn't a quest item."
						<< " (Item ID: " << item_id
						<< ", NPC ID: " << npc_id
						<< ", Quest ID: " << quest_id << ")";
				});
			}
			break;
		}
		case quest_opcodes::start_quest:
			if (player->get_quests()->is_quest_active(quest_id)) {
				channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &log) {
					log << "Player (ID: " << player->get_id()
						<< ", Name: " << player->get_name()
						<< ") tried to start an already started quest."
						<< " (NPC ID: " << npc_id
						<< ", Quest ID: " << quest_id << ")";
				});
			}
			else {
				player->get_quests()->add_quest(quest_id, npc_id);
			}
			break;
		case quest_opcodes::finish_quest:
			player->get_quests()->finish_quest(quest_id, npc_id);
			break;
		case quest_opcodes::start_npc_quest_chat:
		case quest_opcodes::end_npc_quest_chat:
			npc_handler::handle_quest_npc(player, npc_id, act == quest_opcodes::start_npc_quest_chat, quest_id);
			break;
	}
}

}
}