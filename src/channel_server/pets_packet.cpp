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
#include "pets_packet.hpp"
#include "common/file_time.hpp"
#include "common/item.hpp"
#include "common/packet_reader.hpp"
#include "common/session.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/move_path.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace pets {

SPLIT_PACKET_IMPL(pet_summoned, game_player_id player_id, pet *pet, bool kick, int8_t index) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_PET_SPAWN)
		.add<game_player_id>(player_id)
		.add<int8_t>(index != -1 ? index : (pet->is_summoned() ? pet->get_index().get() : -1))
		.add<bool>(pet->is_summoned())
		.add<bool>(kick); // Kick existing pet (only when player doesn't have follow the lead)

	if (pet->is_summoned()) {
		builder.player
			.add<game_item_id>(pet->get_item_id())
			.add<string>(pet->get_name())
			.add<game_pet_id>(pet->get_id())
			.add<point>(pet->get_pos())
			.add<int8_t>(pet->get_stance())
			.add<game_foothold_id>(pet->get_foothold())
			.add<bool>(pet->has_name_tag())
			.add<bool>(pet->has_quote_item());
	}

	builder.map.add_buffer(builder.player);
	return builder;
}

SPLIT_PACKET_IMPL(show_chat, game_player_id player_id, pet *pet, const string &message, int8_t act) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_PET_MESSAGE)
		.add<game_player_id>(player_id)
		.add<int8_t>(pet->is_summoned() ? pet->get_index().get() : -1)
		.unk<int8_t>()
		.add<int8_t>(act)
		.add<string>(message)
		.add<bool>(pet->has_quote_item());
	return builder;
}

SPLIT_PACKET_IMPL(show_movement, game_player_id player_id, pet *pet, const move_path &path) {
	split_packet_builder builder;
	builder.map
		.add<packet_header>(SMSG_PET_MOVEMENT)
		.add<game_player_id>(player_id)
		.add<int8_t>(pet->is_summoned() ? pet->get_index().get() : -1);
	path.write_to_packet(builder.map);
	return builder;
}

PACKET_IMPL(show_animation, game_player_id player_id, pet *pet, int8_t animation) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PET_ANIMATION)
		.add<game_player_id>(player_id)
		.add<int8_t>(pet->is_summoned() ? pet->get_index().get() : -1)
		.add<bool>(animation == 1)
		.add<int8_t>(animation)
		.unk<int8_t>()
		.add<bool>(pet->has_quote_item());
	return builder;
}

PACKET_IMPL(update_pet, pet *pet, item *pet_item) {
	// TODO FIXME remove this and add addInfo packet to addItemInfo and then modify the inventory update packet to support pets
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_INVENTORY_OPERATION)
		.add<int8_t>(0)
		.add<int8_t>(2)
		.add<int8_t>(3)
		.add<int8_t>(5)
		.add<game_inventory_slot>(pet->get_inventory_slot())
		.add<int8_t>(0)
		.add<int8_t>(5)
		.add<game_inventory_slot>(pet->get_inventory_slot())
		.add_buffer(add_info(pet, pet_item));
	return builder;
}

SPLIT_PACKET_IMPL(level_up, game_player_id player_id, pet *pet) {
	split_packet_builder builder;
	packet_builder packet;
	packet
		.add<int16_t>(4)
		.add<int8_t>(pet->is_summoned() ? pet->get_index().get() : -1);

	builder.player
		.add<packet_header>(SMSG_THEATRICS)
		.add_buffer(packet);

	builder.map
		.add<packet_header>(SMSG_SKILL_SHOW)
		.add<game_player_id>(player_id)
		.add_buffer(packet);
	return builder;
}

SPLIT_PACKET_IMPL(change_name, game_player_id player_id, pet *pet) {
	split_packet_builder builder;
	builder.player
		.add<packet_header>(SMSG_PET_NAME_CHANGE)
		.add<game_player_id>(player_id)
		.add<int8_t>(pet->is_summoned() ? pet->get_index().get() : -1)
		.add<string>(pet->get_name())
		.add<bool>(pet->has_name_tag());

	builder.map.add_buffer(builder.player);
	return builder;
}
/* TODO FIXME packet
auto showPet(ref_ptr<player> player, Pet *pet) -> void {
	PacketBuilder builder;
	builder.add<packet_header>(SMSG_PET_SHOW);
	builder.add<game_player_id>(player->get_id());
	builder.add<int8_t>(pet->isSummoned() ? pet->getIndex().get() : -1);
	builder.add<game_pet_id>(pet->get_id());
	builder.add<bool>(pet->hasNameTag());
	player->send(builder);
}*/

PACKET_IMPL(update_summoned_pets, ref_ptr<player> player) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_UPDATE)
		.add<int8_t>(0)
		.add<int16_t>(constant::stat::pet)
		.add<int16_t>(0x18);

	for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
		if (pet *pet = player->get_pets()->get_summoned(i)) {
			builder.add<game_pet_id>(pet->get_id());
		}
		else {
			builder.add<game_pet_id>(0);
		}
	}
	builder.unk<int8_t>();
	return builder;
}

PACKET_IMPL(blank_update) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_PLAYER_UPDATE)
		.add<int8_t>(1)
		.unk<int32_t>();
	return builder;
}

PACKET_IMPL(add_info, pet *pet, item *pet_item) {
	packet_builder builder;
	builder
		.add<int8_t>(3)
		.add<game_item_id>(pet->get_item_id())
		.add<int8_t>(1)
		.add<game_pet_id>(pet->get_id())
		.add<int64_t>(0LL)
		.add<string>(pet->get_name(), 13)
		.add<int8_t>(pet->get_level())
		.add<int16_t>(pet->get_closeness())
		.add<int8_t>(pet->get_fullness())
		.add<file_time>(pet_item->get_expiration_time())
		.unk<int32_t>()
		.add<int32_t>(0); // Time to expire (for trial pet)
	return builder;
}

}
}
}
}