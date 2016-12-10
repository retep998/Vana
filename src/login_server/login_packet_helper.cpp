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
#include "login_packet_helper.hpp"
#include "common/constant/equip_slot.hpp"
#include "common/constant/inventory.hpp"
#include "login_server/characters.hpp"

namespace vana {
namespace login_server {
namespace packets {
namespace helpers {

PACKET_IMPL(add_character, const character &charc) {
	packet_builder builder;
	builder
		.add<game_player_id>(charc.id)
		.add<string>(charc.name, 13)
		.add<game_gender_id>(charc.gender)
		.add<game_skin_id>(charc.skin)
		.add<game_face_id>(charc.face)
		.add<game_hair_id>(charc.hair)
		// Pet cash id (3x)
		.add<int64_t>(0)
		.add<int64_t>(0)
		.add<int64_t>(0)
		.add<game_player_level>(charc.level)
		.add<game_job_id>(charc.job)
		.add<game_stat>(charc.str)
		.add<game_stat>(charc.dex)
		.add<game_stat>(charc.intt)
		.add<game_stat>(charc.luk)
		.add<game_health>(charc.hp)
		.add<game_health>(charc.mhp)
		.add<game_health>(charc.mp)
		.add<game_health>(charc.mmp)
		.add<game_stat>(charc.ap)
		.add<game_stat>(charc.sp)
		.add<game_experience>(charc.exp)
		.add<game_fame>(charc.fame)
		.add<int32_t>(0) // Gachapon EXP
		.add<game_map_id>(charc.map)
		.add<int8_t>(charc.pos)
		.unk<int32_t>() // Added in .62
		.add<game_gender_id>(charc.gender)
		.add<game_skin_id>(charc.skin)
		.add<game_face_id>(charc.face)
		.unk<int8_t>(1)
		.add<game_hair_id>(charc.hair);

	game_item_id equips[constant::inventory::equipped_slots][2] = {0};
	for (const auto &equip : charc.equips) {
		game_inventory_slot slot = -equip.slot;
		if (slot > 100) {
			slot -= 100;
		}
		if (equips[slot][0] > 0) {
			if (equip.slot < -100) {
				equips[slot][1] = equips[slot][0];
				equips[slot][0] = equip.id;
			}
			else {
				equips[slot][1] = equip.id;
			}
		}
		else {
			equips[slot][0] = equip.id;
		}
	}
	for (uint8_t i = 0; i < constant::inventory::equipped_slots; i++) {
		// Shown items
		if (equips[i][0] > 0) {
			builder.add<uint8_t>(i);
			if (i == constant::equip_slot::weapon && equips[i][1] > 0) {
				// Normal weapons always here
				builder.add<game_item_id>(equips[i][1]);
			}
			else {
				builder.add<game_item_id>(equips[i][0]);
			}
		}
	}
	builder.add<int8_t>(-1);
	for (uint8_t i = 0; i < constant::inventory::equipped_slots; i++) {
		// Covered items
		if (equips[i][1] > 0 && i != constant::equip_slot::weapon) {
			builder.add<uint8_t>(i);
			builder.add<game_item_id>(equips[i][1]);
		}
	}

	builder
		.add<int8_t>(-1)
		.add<game_item_id>(equips[constant::equip_slot::weapon][0]) // Cash weapon
		// Pet item id (3x)
		.add<game_item_id>(0)
		.add<game_item_id>(0)
		.add<game_item_id>(0)
		// Rankings
		.add<int8_t>(1)
		.add<int32_t>(charc.world_rank)
		.add<int32_t>(charc.world_rank_change)
		.add<int32_t>(charc.job_rank)
		.add<int32_t>(charc.job_rank_change);
	return builder;
}

}
}
}
}