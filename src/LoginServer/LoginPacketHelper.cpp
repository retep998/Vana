/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "LoginPacketHelper.h"
#include "Characters.h"
#include "ItemConstants.h"
#include "PacketCreator.h"

void LoginPacketHelper::addCharacter(PacketCreator &packet, const Character &charc) {
	packet.add<int32_t>(charc.id);
	packet.addString(charc.name, 13);
	packet.add<int8_t>(charc.gender);
	packet.add<int8_t>(charc.skin);
	packet.add<int32_t>(charc.eyes);
	packet.add<int32_t>(charc.hair);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int8_t>(charc.level);
	packet.add<int16_t>(charc.job);
	packet.add<int16_t>(charc.str);
	packet.add<int16_t>(charc.dex);
	packet.add<int16_t>(charc.intt);
	packet.add<int16_t>(charc.luk);
	packet.add<int16_t>(charc.hp);
	packet.add<int16_t>(charc.mhp);
	packet.add<int16_t>(charc.mp);
	packet.add<int16_t>(charc.mmp);
	packet.add<int16_t>(charc.ap);
	packet.add<int16_t>(charc.sp);
	packet.add<int32_t>(charc.exp);
	packet.add<int16_t>(charc.fame);
	packet.add<int32_t>(0); // Unknown int32 added in .62
	packet.add<int32_t>(charc.map);
	packet.add<int8_t>(charc.pos);
	packet.add<int32_t>(0); // Unknown int32 added in .62
	packet.add<int8_t>(charc.gender);
	packet.add<int8_t>(charc.skin);
	packet.add<int32_t>(charc.eyes);
	packet.add<int8_t>(1);
	packet.add<int32_t>(charc.hair);
	int32_t equips[Inventories::EquippedSlots][2] = {0};
	for (int16_t i = 0; i < (int16_t) charc.equips.size(); i++) {
		int16_t slot = -charc.equips[i].slot;
		if (slot > 100)
			slot -= 100;
		if (equips[slot][0] > 0) {
			if (charc.equips[i].slot < -100) {
				equips[slot][1] = equips[slot][0];
				equips[slot][0] = charc.equips[i].id;
			}
			else {
				equips[slot][1] = charc.equips[i].id;
			}
		}
		else {
			equips[slot][0] = charc.equips[i].id;
		}
	}
	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) { // Shown items
		if (equips[i][0] > 0) {
			packet.add<int8_t>(i);
			if (i == EquipSlots::Weapon && equips[i][1] > 0) { // Normal weapons always here
				packet.add<int32_t>(equips[i][1]);
			}
			else {
				packet.add<int32_t>(equips[i][0]);
			}
		}
	}
	packet.add<int8_t>(-1);
	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) { // Covered items
		if (equips[i][1] > 0 && i != EquipSlots::Weapon) {
			packet.add<int8_t>(i);
			packet.add<int32_t>(equips[i][1]);
		}
	}
	packet.add<int8_t>(-1);
	packet.add<int32_t>(equips[EquipSlots::Weapon][0]); // Cash weapon
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	packet.add<int32_t>(0);
	// Rankings
	packet.add<int8_t>(1);
	packet.add<int32_t>(charc.w_rank);
	packet.add<int32_t>(charc.w_rankmove);
	packet.add<int32_t>(charc.j_rank);
	packet.add<int32_t>(charc.j_rankmove);
}
