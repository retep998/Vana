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
#include "DropsPacket.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/item_constants.hpp"
#include "common/session_temp.hpp"
#include "channel_server/Drop.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/SmsgHeader.hpp"

namespace vana {
namespace channel_server {
namespace packets {
namespace drops {

PACKET_IMPL(show_drop, drop *drop, int8_t type, const point &origin) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_ITEM)
		.add<int8_t>(type)
		.add<game_map_object>(drop->get_id())
		.add<bool>(drop->is_mesos())
		.add<int32_t>(drop->get_object_id())
		.add<int32_t>(drop->get_owner())
		.add<int8_t>(drop->get_type())
		.add<point>(drop->get_pos())
		.add<int32_t>(drop->get_time());

	if (type != drop_types::show_existing) {
		// Give the point of origin for things that are just being dropped
		builder
			.add<point>(origin)
			.unk<int16_t>();
	}
	if (!drop->is_mesos()) {
		builder.add<file_time>(items::no_expiration);
	}
	builder.add<bool>(!drop->is_player_drop()); // Determines whether pets can pick item up or not
	return builder;
}

PACKET_IMPL(take_drop, game_player_id player_id, game_map_object drop_id, int8_t pet_index) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_PICKUP)
		.add<int8_t>(pet_index != -1 ? 5 : 2)
		.add<game_map_object>(drop_id)
		.add<game_player_id>(player_id);
	return builder;
}

PACKET_IMPL(dont_take) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_INVENTORY_OPERATION)
		.add<int16_t>(1);
	return builder;
}

PACKET_IMPL(remove_drop, game_map_object drop_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_PICKUP)
		.add<int8_t>(0)
		.add<game_map_object>(drop_id);
	return builder;
}

PACKET_IMPL(explode_drop, game_map_object drop_id) {
	packet_builder builder;
	builder
		.add<packet_header>(SMSG_DROP_PICKUP)
		.add<int8_t>(4)
		.add<game_map_object>(drop_id)
		.add<int16_t>(655);
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
	else if (game_logic_utilities::get_inventory(id) != inventories::equip) {
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