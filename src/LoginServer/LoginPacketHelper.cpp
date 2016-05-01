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
#include "LoginPacketHelper.hpp"
#include "Common/ItemConstants.hpp"
#include "LoginServer/Characters.hpp"

namespace Vana {
namespace LoginServer {
namespace Packets {
namespace Helpers {

PACKET_IMPL(addCharacter, const Character &charc) {
	PacketBuilder builder;
	builder
		.add<player_id_t>(charc.id)
		.add<string_t>(charc.name, 13)
		.add<gender_id_t>(charc.gender)
		.add<skin_id_t>(charc.skin)
		.add<face_id_t>(charc.face)
		.add<hair_id_t>(charc.hair)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<player_level_t>(charc.level)
		.add<job_id_t>(charc.job)
		.add<stat_t>(charc.str)
		.add<stat_t>(charc.dex)
		.add<stat_t>(charc.intt)
		.add<stat_t>(charc.luk)
		.add<health_t>(charc.hp)
		.add<health_t>(charc.mhp)
		.add<health_t>(charc.mp)
		.add<health_t>(charc.mmp)
		.add<stat_t>(charc.ap)
		.add<stat_t>(charc.sp)
		.add<experience_t>(charc.exp)
		.add<fame_t>(charc.fame)
		.add<int32_t>(0) // Unknown int32 added in .62
		.add<map_id_t>(charc.map)
		.add<int8_t>(charc.pos)
		.add<int32_t>(0) // Unknown int32 added in .62
		.add<gender_id_t>(charc.gender)
		.add<skin_id_t>(charc.skin)
		.add<face_id_t>(charc.face)
		.add<int8_t>(1)
		.add<hair_id_t>(charc.hair);

	item_id_t equips[Inventories::EquippedSlots][2] = {0};
	for (const auto &equip : charc.equips) {
		inventory_slot_t slot = -equip.slot;
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
	for (uint8_t i = 0; i < Inventories::EquippedSlots; i++) {
		// Shown items
		if (equips[i][0] > 0) {
			builder.add<uint8_t>(i);
			if (i == EquipSlots::Weapon && equips[i][1] > 0) {
				// Normal weapons always here
				builder.add<item_id_t>(equips[i][1]);
			}
			else {
				builder.add<item_id_t>(equips[i][0]);
			}
		}
	}
	builder.add<int8_t>(-1);
	for (uint8_t i = 0; i < Inventories::EquippedSlots; i++) {
		// Covered items
		if (equips[i][1] > 0 && i != EquipSlots::Weapon) {
			builder.add<uint8_t>(i);
			builder.add<item_id_t>(equips[i][1]);
		}
	}

	builder
		.add<int8_t>(-1)
		.add<item_id_t>(equips[EquipSlots::Weapon][0]) // Cash weapon
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		// Rankings
		.add<int8_t>(1)
		.add<int32_t>(charc.worldRank)
		.add<int32_t>(charc.worldRankChange)
		.add<int32_t>(charc.jobRank)
		.add<int32_t>(charc.jobRankChange);
	return builder;
}

}
}
}
}