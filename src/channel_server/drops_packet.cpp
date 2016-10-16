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
#include "drops_packet.hpp"
#include "common/session.hpp"
#include "common/util/game_logic/inventory.hpp"
#include "channel_server/drop.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/smsg_header.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace drops {

PACKET_IMPL(show_drop, drop *drop, drop_spawn_types type, const point &origin, uint16_t delay) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_ITEM)
		.add<int8_t>((int8_t)type)
		.add<game_map_object>(drop->get_id())
		.add<bool>(drop->is_mesos())
		.add<int32_t>(drop->get_object_id())
		.add<int32_t>(drop->get_owner())
		.add<int8_t>(drop->get_type())
		.add<point>(drop->get_pos())
		.add<int32_t>(drop->get_time());

	if (type != drop_spawn_types::show_existing) {
		// Give the point of origin for things that are just being dropped
		builder
			.add<point>(origin)
			.add<int16_t>(delay);
	}

	if (!drop->is_mesos()) {
		builder.add<file_time>(constant::item::no_expiration);
	}

	builder.add<bool>(!drop->is_player_drop()); // Determines whether pets can pick item up or not
	return builder;
}

PACKET_IMPL(take_drop, game_player_id player_id, game_map_object drop_id, int8_t pet_index) {
	return remove_drop(drop_id, pet_index != -1 ? drop_despawn_types::loot_by_pet : drop_despawn_types::loot_by_user, player_id);
}

PACKET_IMPL(dont_take) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_INVENTORY_OPERATION)
		.add<int16_t>(1);
	return builder;
}

PACKET_IMPL(remove_drop, game_map_object drop_id, drop_despawn_types type, game_map_object looted_by) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_PICKUP)
		.add<int8_t>((int8_t)type)
		.add<game_map_object>(drop_id);

	switch (type) {
	case drop_despawn_types::expire:
	case drop_despawn_types::remove:
		// Expire and remove do not have any additional data
		break;

	case drop_despawn_types::loot_by_mob:
	case drop_despawn_types::loot_by_user:
	case drop_despawn_types::loot_by_pet:
		// Option is either the mob (loot_by_mob) or the user (rest)
		builder.add<game_map_object>(looted_by);
		break;

	case drop_despawn_types::explode:
		throw std::invalid_argument("type cannot be 'explode'; use explode_drop instead");

	default:
		throw std::invalid_argument("Unknown type supplied");
	}
	return builder;
}

PACKET_IMPL(explode_drop, game_map_object drop_id, int16_t delay) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_PICKUP)
		.add<int8_t>((int8_t)drop_despawn_types::explode)
		.add<game_map_object>(drop_id)
		.add<int16_t>(delay);
	return builder;
}

PACKET_IMPL(drop_not_available_for_pickup) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<int8_t>(-2);
	return builder;
}

PACKET_IMPL(cant_get_anymore_items) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<int8_t>(-1);
	return builder;
}

PACKET_IMPL(pickup_drop, game_map_object id, int32_t amount, bool is_mesos, int16_t cafe_bonus) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(0)
		.add<bool>(is_mesos)
		.add<game_map_object>(id);

	if (is_mesos) {
		builder.add<int16_t>(cafe_bonus);
	}
	else if (vana::util::game_logic::inventory::get_inventory(id) != constant::inventory::equip) {
		builder.add<game_slot_qty>(static_cast<game_slot_qty>(amount));
	}
	if (!is_mesos) {
		builder
			.unk<int32_t>()
			.unk<int32_t>();
	}
	return builder;
}

PACKET_IMPL(pickup_drop_special, game_map_object id) {
	packet_builder builder;
	// This builder is used for PQ drops (maybe, got it from the Wing of the Wind item) and monster cards
	builder
		.add<packet_header>(SMSG_NOTICE)
		.add<int8_t>(0)
		.unk<int8_t>(2)
		.add<game_map_object>(id);
	return builder;
}

}
}
}
}