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
#include <queue>

void LoginPacketHelper::addCharacter(PacketCreator &packet, Character charc) {
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
	packet.addInt(charc.map);
	packet.addByte(charc.pos);
	packet.addByte(charc.gender);
	packet.addByte(charc.skin);
	packet.addInt(charc.eyes);
	packet.addByte(1);
	packet.addInt(charc.hair);
	int cashweapon = 0;
	std::queue <int> hiddenEquips;
	for (int j = 0; j < (int) charc.equips.size(); j++) {
		if (charc.equips[j].pos != -111) {
			if (j == 0 || charc.equips[j].type == 11 || charc.equips[j].type != charc.equips[j-1].type) { // Normal weapons always appear here
				addEquip(packet, charc.equips[j]);
			}
			else {
				hiddenEquips.push(j);
			}
		}
		else {
			cashweapon = charc.equips[j].id;
		}
	}
	packet.addByte(-1);
	while (!hiddenEquips.empty()) {
		int j = hiddenEquips.front();
		addEquip(packet, charc.equips[j]);
		hiddenEquips.pop();
	}
	packet.addByte(-1);
	packet.addInt(cashweapon);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addByte(0);
}

inline
void LoginPacketHelper::addEquip(PacketCreator &packet, CharEquip &equip) {
	int pos = -equip.pos;
	if (pos > 100)
		pos -= 100;
	packet.addByte(pos);
	packet.addInt(equip.id);
}
