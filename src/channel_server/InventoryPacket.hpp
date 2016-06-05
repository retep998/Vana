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

#include "common/packet_builder.hpp"
#include "common/split_packet_builder.hpp"
#include "common/types_temp.hpp"
#include <string>
#include <vector>

namespace vana {
	class item;

	namespace channel_server {
		class player;

		struct inventory_packet_operation {
			inventory_packet_operation() = delete;
			inventory_packet_operation(int8_t operation_type, item *item, int16_t current_slot, int16_t old_slot = 0) :
				operation_type{operation_type},
				item{item},
				current_slot{current_slot},
				old_slot{old_slot}
			{
			}

			int8_t operation_type = 0;
			item *item = nullptr;
			game_inventory_slot old_slot = 0;
			game_inventory_slot current_slot = 0;
		};

		namespace packets {
			namespace inventory {
				namespace rock_modes {
					enum modes : int8_t {
						remove = 0x02,
						add = 0x03
					};
				}
				namespace rock_errors {
					enum errors : int8_t {
						unk = 0x02, // Causes error 38 with current data
						unk2 = 0x03, // Causes error 38 with current data
						cannot_go2 = 0x05, // This is unused
						difficult_to_locate = 0x06,
						difficult_to_locate2 = 0x07, // This is unused
						cannot_go = 0x08,
						already_there = 0x09,
						cannot_save_map = 0x0A,
						noobs_cannot_leave_maple_island = 0x0B, // "Users below level 7 are not allowed to go out from Maple Island."
					};
				}
				namespace rock_types {
					enum types : int8_t {
						regular = 0x00,
						vip = 0x01,
					};
				}
				namespace operation_types {
					enum types : int8_t {
						add_item = 0x00,
						modify_quantity = 0x01,
						modify_slot = 0x02,
						remove_item = 0x03,
					};
				}

				SPLIT_PACKET(update_player, ref_ptr<player> player);
				SPLIT_PACKET(sit_chair, game_player_id player_id, game_item_id chair_id);
				SPLIT_PACKET(stop_chair, game_player_id player_id, bool seat_taken);
				SPLIT_PACKET(use_scroll, game_player_id player_id, int8_t succeed, bool destroy, bool legendary_spirit);
				SPLIT_PACKET(send_chalkboard_update, game_player_id player_id, const string &msg);
				SPLIT_PACKET(use_skillbook, game_player_id player_id, game_skill_id skill_id, int32_t new_max_level, bool use, bool succeed);
				SPLIT_PACKET(use_item_effect, game_player_id player_id, game_item_id item_id);
				SPLIT_PACKET(send_reward_item_animation, game_player_id player_id, game_item_id item_id, const string &effect);
				PACKET(inventory_operation, bool unk, const vector<inventory_packet_operation> &operations);
				PACKET(sit_map_chair, game_seat_id chair_id);
				PACKET(show_megaphone, const string &msg);
				PACKET(show_super_megaphone, const string &msg, bool whisper = false);
				PACKET(show_messenger, const string &player_name, const string &msg1, const string &msg2, const string &msg3, const string &msg4, unsigned char *display_info, int32_t display_info_size, game_item_id item_id);
				PACKET(show_item_megaphone, const string &msg, bool whisper = false, item *item = nullptr);
				PACKET(show_triple_megaphone, int8_t lines, const string &line1, const string &line2, const string &line3, bool whisper);
				PACKET(update_slots, game_inventory inventory, game_inventory_slot_count slots);
				PACKET(send_rock_update, int8_t mode, int8_t type, const vector<game_map_id> &maps);
				PACKET(send_rock_error, int8_t code, int8_t type);
				PACKET(use_charm, uint8_t charms_left, uint8_t days_left = 99);
				PACKET(send_mesobag_succeed, game_mesos mesos);
				PACKET(send_mesobag_failed);
				PACKET(send_hammer_slots, int32_t slots);
				PACKET(send_hulk_smash, game_inventory_slot slot, item *hammered);
				PACKET(send_hammer_update);
				PACKET(play_cash_song, game_item_id item_id, const string &player_name);
				PACKET(blank_update);
				PACKET(send_item_expired, const vector<game_item_id> &items);
			}
		}
	}
}