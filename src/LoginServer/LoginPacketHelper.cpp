/*
Copyright (C) 2008 Vana Development Team

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
#include "PacketCreator.h"

void LoginPacketHelper::addCharacter(PacketCreator &packet, const Character &charc) {
	packet.addInt(charc.id);
	packet.addString(charc.name, 12);
	packet.addByte(0);
	packet.addByte(charc.gender);
	packet.addByte(charc.skin);
	packet.addInt(charc.eyes);
	packet.addInt(charc.hair);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(charc.level);
	packet.addShort(charc.job);
	packet.addShort(charc.str);
	packet.addShort(charc.dex);
	packet.addShort(charc.intt);
	packet.addShort(charc.luk);
	packet.addShort(charc.hp);
	packet.addShort(charc.mhp);
	packet.addShort(charc.mp);
	packet.addShort(charc.mmp);
	packet.addShort(charc.ap);
	packet.addShort(charc.sp);
	packet.addInt(charc.exp);
	packet.addShort(charc.fame);
	packet.addInt(0); // Unknown int32 added in .62
	packet.addInt(charc.map);
	packet.addByte(charc.pos);
	packet.addInt(0); // Unknown int32 added in .62
	packet.addByte(charc.gender);
	packet.addByte(charc.skin);
	packet.addInt(charc.eyes);
	packet.addByte(1);
	packet.addInt(charc.hair);
	int32_t equips[50][2] = {0};
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
	for (int8_t i = 0; i < 50; i++) { // Shown items
		if (equips[i][0] > 0) {
			packet.addByte(i);
			if (i == 11 && equips[i][1] > 0) // Normal weapons always here
				packet.addInt(equips[i][1]);
			else
				packet.addInt(equips[i][0]);
		}
	}
	packet.addByte(-1);
	for (int8_t i = 0; i < 50; i++) { // Covered items
		if (equips[i][1] > 0 && i != 11) {
			packet.addByte(i);
			packet.addInt(equips[i][1]);
		}
	}
	packet.addByte(-1);
	packet.addInt(equips[11][0]); // Cash weapon
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
}
