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
#include "storage_packet.hpp"
#include "common/session.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_packet_helper.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace storage {

PACKET_IMPL(show_storage, ref_ptr<player> player, game_npc_id npc_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_STORAGE)
		.add<int8_t>(0x16) // Type of storage action
		.add<game_npc_id>(npc_id)
		.add<game_storage_slot>(player->get_storage()->get_slots())
		.add<int32_t>(0x7e)
		.unk<int32_t>()
		.add<game_mesos>(player->get_storage()->get_mesos())
		.unk<int16_t>()
		.add<game_storage_slot>(player->get_storage()->get_num_items());

	for (game_storage_slot i = 0; i < player->get_storage()->get_num_items(); i++) {
		builder.add_buffer(helpers::add_item_info(0, player->get_storage()->get_item(i)));
	}

	builder
		.unk<int16_t>()
		.unk<int8_t>();
	return builder;
}

PACKET_IMPL(add_item, ref_ptr<player> player, game_inventory inv) {
	packet_builder builder;
	int8_t type = static_cast<int8_t>(std::pow(2.f, static_cast<int32_t>(inv))) * 2; // Gotta work some magic on type, which starts as inventory
	builder
		.add<packet_header>(SMSG_STORAGE)
		.add<int8_t>(0x0d)
		.add<game_storage_slot>(player->get_storage()->get_slots())
		.add<int32_t>(type)
		.unk<int32_t>()
		.add<game_storage_slot>(player->get_storage()->get_num_items(inv));

	for (game_storage_slot i = 0; i < player->get_storage()->get_num_items(); i++) {
		item *item = player->get_storage()->get_item(i);
		if (vana::util::game_logic::inventory::get_inventory(item->get_id()) == inv) {
			builder.add_buffer(helpers::add_item_info(0, item));
		}
	}
	return builder;
}

PACKET_IMPL(take_item, ref_ptr<player> player, int8_t inv) {
	packet_builder builder;
	int8_t type = static_cast<int8_t>(std::pow(2.f, static_cast<int32_t>(inv))) * 2;
	builder
		.add<packet_header>(SMSG_STORAGE)
		.add<int8_t>(0x09)
		.add<game_storage_slot>(player->get_storage()->get_slots())
		.add<int32_t>(type)
		.unk<int32_t>()
		.add<game_storage_slot>(player->get_storage()->get_num_items(inv));

	for (game_storage_slot i = 0; i < player->get_storage()->get_num_items(); i++) {
		item *item = player->get_storage()->get_item(i);
		if (vana::util::game_logic::inventory::get_inventory(item->get_id()) == inv) {
			builder.add_buffer(helpers::add_item_info(0, item));
		}
	}
	return builder;
}

PACKET_IMPL(change_mesos, game_storage_slot slot_count, game_mesos mesos) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_STORAGE)
		.add<int8_t>(0x13)
		.add<game_storage_slot>(slot_count)
		.unk<int16_t>(2)
		.unk<int16_t>()
		.unk<int32_t>()
		.add<game_mesos>(mesos);
	return builder;
}

PACKET_IMPL(storage_full) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_STORAGE)
		.add<int8_t>(0x11);
	return builder;
}

PACKET_IMPL(no_mesos) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_STORAGE)
		.add<int8_t>(0x10);
	return builder;
}

}
}
}
}