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
#include "inventory_packet.hpp"
#include "common/inter_header.hpp"
#include "common/session.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/inventory_packet_helper.hpp"
#include "channel_server/map.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_inventory.hpp"
#include "channel_server/player_packet_helper.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace inventory {

SPLIT_PACKET_IMPL(update_player, ref_ptr<player> player) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_PLAYER_CHANGE_LOOK)
		.add<game_player_id>(player->get_id())
		.add<int8_t>(1)
		.add_buffer(helpers::add_player_display(player))
		.unk<int8_t>()
		.unk<int16_t>();

	builder.map.add_buffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(sit_chair, game_player_id player_id, game_item_id chair_id) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_PLAYER_UPDATE)
		.add<int16_t>(1)
		.unk<int32_t>();

	builder.map
		.add<packet_header>(SMSG_CHAIR_SIT)
		.add<game_player_id>(player_id)
		.add<game_item_id>(chair_id);
	return builder;
}

SPLIT_PACKET_IMPL(stop_chair, game_player_id player_id, bool seat_taken) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_CHAIR)
		.add<int8_t>(0);

	if (seat_taken) {
		return builder;
	}

	builder.map
		.add<packet_header>(SMSG_CHAIR_SIT)
		.add<game_player_id>(player_id)
		.add<game_item_id>(0);
	return builder;
}

SPLIT_PACKET_IMPL(use_scroll, game_player_id player_id, int8_t succeed, bool destroy, bool legendary_spirit) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_SCROLL_USE)
		.add<game_player_id>(player_id)
		.add<int8_t>(succeed)
		.add<bool>(destroy)
		.add<int16_t>(legendary_spirit);

	builder.map.add_buffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(send_chalkboard_update, game_player_id player_id, const string &msg) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_CHALKBOARD)
		.add<game_player_id>(player_id)
		.add<bool>(!msg.empty())
		.add<string>(msg);

	builder.map.add_buffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(use_skillbook, game_player_id player_id, bool is_mastery_book, game_skill_id skill_id, int32_t new_max_level, bool use, bool succeed) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_SKILLBOOK)
		.add<game_player_id>(player_id)
		.add<bool>(is_mastery_book)
		.add<game_skill_id>(skill_id)
		.add<int32_t>(new_max_level)
		.add<bool>(use)
		.add<bool>(succeed);

	if (use) {
		builder.map.add_buffer(builder.player);
	}
	return builder;
}

SPLIT_PACKET_IMPL(use_item_effect, game_player_id player_id, game_item_id item_id) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_ITEM_EFFECT)
		.add<game_player_id>(player_id)
		.add<game_item_id>(item_id);

	builder.map.add_buffer(builder.player);
	return builder;
}

PACKET_IMPL(inventory_operation, bool unk, const vector<inventory_packet_operation> &operations) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_INVENTORY_OPERATION)
		.add<bool>(unk)
		.add<uint8_t>(static_cast<uint8_t>(operations.size()));

	int8_t added_byte = -1;

	for (const auto &operation : operations) {
		builder
			.add<int8_t>(operation.operation_type)
			.add<game_inventory>(vana::util::game_logic::inventory::get_inventory(operation.item->get_id()));

		switch (operation.operation_type) {
			case packets::inventory::operation_types::add_item:
				builder.add_buffer(helpers::add_item_info(operation.current_slot, operation.item, true));
				break;
			case packets::inventory::operation_types::modify_quantity:
				builder
					.add<game_inventory_slot>(operation.current_slot)
					.add<game_slot_qty>(operation.item->get_amount());
				break;
			case packets::inventory::operation_types::modify_slot:
				builder
					.add<game_inventory_slot>(operation.old_slot)
					.add<game_inventory_slot>(operation.current_slot);

				if (added_byte == -1) {
					if (operation.old_slot < 0) {
						added_byte = 1;
					}
					else if (operation.current_slot < 0) {
						added_byte = 2;
					}
				}
				break;
			case packets::inventory::operation_types::remove_item:
				builder.add<game_inventory_slot>(operation.current_slot);
				break;
		}
	}

	if (added_byte != -1) {
		builder.add<int8_t>(added_byte);
	}
	return builder;
}

PACKET_IMPL(sit_map_chair, game_seat_id chair_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CHAIR)
		.add<int8_t>(1)
		.add<game_seat_id>(chair_id);
	return builder;
}

PACKET_IMPL(show_megaphone, const string &msg) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESSAGE)
		.add<int8_t>(2)
		.add<string>(msg);
	return builder;
}

PACKET_IMPL(show_super_megaphone, const string &msg, bool whisper) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESSAGE)
		.add<int8_t>(3)
		.add<string>(msg)
		.add<int8_t>(channel_server::get_instance().get_channel_id())
		.add<bool>(whisper);
	return builder;
}

PACKET_IMPL(show_messenger, const string &player_name, const string &msg1, const string &msg2, const string &msg3, const string &msg4, unsigned char *display_info, int32_t display_info_size, game_item_id item_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_AVATAR_MEGAPHONE)
		.add<game_item_id>(item_id)
		.add<string>(player_name)
		.add<string>(msg1)
		.add<string>(msg2)
		.add<string>(msg3)
		.add<string>(msg4)
		.add<int32_t>(channel_server::get_instance().get_channel_id())
		.add_buffer(display_info, display_info_size);
	return builder;
}

PACKET_IMPL(show_item_megaphone, const string &msg, bool whisper, item *item) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESSAGE)
		.add<int8_t>(8)
		.add<string>(msg)
		.add<int8_t>(channel_server::get_instance().get_channel_id())
		.add<bool>(whisper);

	if (item == nullptr) {
		builder.add<int8_t>(0);
	}
	else {
		builder.add_buffer(helpers::add_item_info(1, item));
	}
	return builder;
}

PACKET_IMPL(show_triple_megaphone, int8_t lines, const string &line1, const string &line2, const string &line3, bool whisper) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESSAGE)
		.add<int8_t>(0x0A)
		.add<string>(line1)
		.add<int8_t>(lines);
	if (lines > 1) {
		builder.add<string>(line2);
	}
	if (lines > 2) {
		builder.add<string>(line3);
	}
	builder
		.add<int8_t>(channel_server::get_instance().get_channel_id())
		.add<bool>(whisper);
	return builder;
}

PACKET_IMPL(update_slots, game_inventory inventory, game_inventory_slot_count slots) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_INVENTORY_SLOT_UPDATE)
		.add<game_inventory>(inventory)
		.add<game_inventory_slot_count>(slots);
	return builder;
}

PACKET_IMPL(blank_update) {
	packet_builder builder;
	vector<inventory_packet_operation> ops;
	builder.add_buffer(inventory_operation(true, ops));
	return builder;
}

PACKET_IMPL(send_rock_update, int8_t mode, int8_t type, const vector<game_map_id> &maps) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_TELEPORT_ROCK)
		.add<int8_t>(mode)
		.add<int8_t>(type)
		.add_buffer(helpers::fill_rock_packet(maps, (type == rock_types::regular ? constant::inventory::teleport_rock_max : constant::inventory::vip_rock_max)));
	return builder;
}

PACKET_IMPL(send_rock_error, int8_t code, int8_t type) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_TELEPORT_ROCK)
		.add<int8_t>(code)
		.add<int8_t>(type);
	return builder;
}

PACKET_IMPL(send_mesobag_succeed, game_mesos mesos) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_MESOBAG_SUCCESS)
		.add<game_mesos>(mesos);
	return builder;
}

PACKET_IMPL(send_mesobag_failed) {
	packet_builder builder;
	builder.add<packet_header>(SMSG_MESOBAG_FAILURE);
	return builder;
}

PACKET_IMPL(use_charm, uint8_t charms_left, uint8_t days_left) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_THEATRICS)
		.add<int8_t>(0x06)
		.add<int8_t>(0x01)
		.add<uint8_t>(charms_left)
		.add<uint8_t>(days_left);
	return builder;
}

PACKET_IMPL(send_hammer_slots, int32_t slots) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_HAMMER)
		.add<int8_t>(0x34) // No idea... mode of some sort, I think
		.add<int32_t>(0x00)
		.add<int32_t>(slots);
	return builder;
}

PACKET_IMPL(send_hulk_smash, game_inventory_slot slot, item *hammered) {
	packet_builder builder;
	vector<inventory_packet_operation> ops;
	ops.emplace_back(operation_types::remove_item, hammered, slot);
	ops.emplace_back(operation_types::add_item, hammered, slot);
	builder.add_buffer(inventory_operation(false, ops));
	return builder;
}

PACKET_IMPL(send_hammer_update) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_HAMMER)
		.add<int8_t>(0x38) // No idea... mode of some sort, I think
		.add<int32_t>(0x00);
	return builder;
}

PACKET_IMPL(play_cash_song, game_item_id item_id, const string &player_name) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_CASH_SONG)
		.add<game_item_id>(item_id)
		.add<string>(player_name);
	return builder;
}

SPLIT_PACKET_IMPL(send_reward_item_animation, game_player_id player_id, game_item_id item_id, const string &effect) {
	split_packet_builder builder;
	packet_builder buffer;
	buffer
		.add<int8_t>(0x0E)
		.add<game_item_id>(item_id)
		.add<int8_t>(1) // Unk...?
		.add<string>(effect);

	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add_buffer(buffer);

	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add_buffer(buffer);
	return builder;
}

PACKET_IMPL(send_item_expired, const vector<game_item_id> &items) {
	packet_builder builder;
	builder
		.add<int16_t>(SMSG_NOTICE)
		.add<int8_t>(0x08)
		.add<uint8_t>(static_cast<uint8_t>(items.size()));

	for (const auto &item_id : items) {
		builder.add<game_item_id>(item_id);
	}
	return builder;
}

}
}
}
}