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
#include "player_packet_helper.hpp"
#include "common/file_time.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/time.hpp"
#include "channel_server/inventory.hpp"
#include "channel_server/pet.hpp"
#include "channel_server/player.hpp"
#include <cmath>

namespace vana {
namespace channel_server {
namespace packets {
namespace helpers {

PACKET_IMPL(add_item_info, game_inventory_slot slot, item *item, bool short_slot) {
	packet_builder builder;
	if (slot != 0) {
		if (short_slot) {
			builder.add<game_inventory_slot>(slot);
		}
		else {
			slot = std::abs(slot);
			if (slot > 100) {
				slot -= 100;
			}
			builder.add<int8_t>(static_cast<int8_t>(slot));
		}
	}
	bool equip = vana::util::game_logic::item::is_equip(item->get_id());
	builder
		.add<int8_t>(equip ? 1 : 2)
		.add<game_item_id>(item->get_id());

	if (false) { //item->get_cash_id() != 0) {
		builder
			.add<int8_t>(1)
			.add<int64_t>(0); //item->get_cash_id());
	}
	else {
		builder.add<int8_t>(0);
	}
	builder.add<file_time>(item->get_expiration_time());
	if (equip) {
		builder
			.add<int8_t>(item->get_slots())
			.add<int8_t>(item->get_scrolls())
			.add<game_stat>(item->get_str())
			.add<game_stat>(item->get_dex())
			.add<game_stat>(item->get_int())
			.add<game_stat>(item->get_luk())
			.add<game_health>(item->get_hp())
			.add<game_health>(item->get_mp())
			.add<game_stat>(item->get_watk())
			.add<game_stat>(item->get_matk())
			.add<game_stat>(item->get_wdef())
			.add<game_stat>(item->get_mdef())
			.add<game_stat>(item->get_accuracy())
			.add<game_stat>(item->get_avoid())
			.add<game_stat>(item->get_hands())
			.add<game_stat>(item->get_speed())
			.add<game_stat>(item->get_jump())
			.add<string>(item->get_name()) // Owner string
			.add<int16_t>(item->get_flags()); // Lock, shoe spikes, cape cold protection, etc.

		if (false) { //item->get_cash_id() != 0) {
			builder
				.add_bytes("91174826F700") // Always the same for cash equips
				.unk<int32_t>();
		}
		else {
			builder
				.unk<int8_t>()
				.unk<int8_t>() // Item level
				.unk<int16_t>()
				.unk<int16_t>() // Item EXP of.. some sort
				.add<int32_t>(item->get_hammers())
				.unk<int64_t>(-1);
		}
		builder
			.add_bytes("0040E0FD3B374F01") // Always the same?
			.unk<int32_t>(-1);
	}
	else {
		builder
			.add<game_slot_qty>(item->get_amount())
			.add<string>(item->get_name()) // Specially made by <IGN>
			.add<int16_t>(item->get_flags());

		if (vana::util::game_logic::item::is_rechargeable(item->get_id())) {
			builder.add<int64_t>(0); // Might be rechargeable ID for internal tracking/duping tracking
		}
	}
	return builder;
}

PACKET_IMPL(add_player_display, ref_ptr<vana::channel_server::player> player) {
	packet_builder builder;
	builder
		.add<game_gender_id>(player->get_gender())
		.add<game_skin_id>(player->get_skin())
		.add<game_face_id>(player->get_face())
		.unk<int8_t>(1)
		.add<game_hair_id>(player->get_hair());

	player->get_inventory()->add_equipped_packet(builder);
	for (int8_t i = 0; i < constant::inventory::max_pet_count; i++) {
		if (pet *pet = player->get_pets()->get_summoned(i)) {
			builder.add<game_item_id>(pet->get_item_id());
		}
		else {
			builder.add<game_item_id>(0);
		}
	}
	return builder;
}

}
}
}
}