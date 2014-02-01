/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Characters.hpp"
#include "ItemConstants.hpp"

namespace LoginPacketHelper {

PACKET_IMPL(addCharacter, const Character &charc) {
	PacketBuilder builder;
	builder
		.add<int32_t>(charc.id)
		.addString(charc.name, 13)
		.add<int8_t>(charc.gender)
		.add<int8_t>(charc.skin)
		.add<int32_t>(charc.eyes)
		.add<int32_t>(charc.hair)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<int32_t>(0)
		.add<uint8_t>(charc.level)
		.add<int16_t>(charc.job)
		.add<int16_t>(charc.str)
		.add<int16_t>(charc.dex)
		.add<int16_t>(charc.intt)
		.add<int16_t>(charc.luk)
		.add<int16_t>(charc.hp)
		.add<int16_t>(charc.mhp)
		.add<int16_t>(charc.mp)
		.add<int16_t>(charc.mmp)
		.add<int16_t>(charc.ap)
		.add<int16_t>(charc.sp)
		.add<int32_t>(charc.exp)
		.add<int16_t>(charc.fame)
		.add<int32_t>(0) // Unknown int32 added in .62
		.add<int32_t>(charc.map)
		.add<int8_t>(charc.pos)
		.add<int32_t>(0) // Unknown int32 added in .62
		.add<int8_t>(charc.gender)
		.add<int8_t>(charc.skin)
		.add<int32_t>(charc.eyes)
		.add<int8_t>(1)
		.add<int32_t>(charc.hair);

	int32_t equips[Inventories::EquippedSlots][2] = {0};
	for (const auto &equip : charc.equips) {
		int16_t slot = -equip.slot;
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
				builder.add<int32_t>(equips[i][1]);
			}
			else {
				builder.add<int32_t>(equips[i][0]);
			}
		}
	}
	builder.add<int8_t>(-1);
	for (uint8_t i = 0; i < Inventories::EquippedSlots; i++) {
		// Covered items
		if (equips[i][1] > 0 && i != EquipSlots::Weapon) {
			builder.add<uint8_t>(i);
			builder.add<int32_t>(equips[i][1]);
		}
	}

	builder
		.add<int8_t>(-1)
		.add<int32_t>(equips[EquipSlots::Weapon][0]) // Cash weapon
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