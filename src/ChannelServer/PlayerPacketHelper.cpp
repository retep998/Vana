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
#include "PlayerPacketHelper.h"

void PlayerPacketHelper::addEquip(Packet &packet, Equip* equip, bool pos_is_short) {
	if (pos_is_short)
		packet.addShort(equip->pos);
	else if (equip->pos < 0) {
		char pos = (char) -equip->pos;
		if (pos > 100)
			pos -= 100;
		packet.addByte(pos);
	}
	else
		packet.addByte((char) equip->pos);
	packet.addByte(1);
	packet.addInt(equip->id);
	packet.addShort(0);
	packet.addBytes("8005BB46E61702");
	packet.addByte(equip->slots); // slots
	packet.addByte((char) equip->scrolls); // scrolls
	packet.addShort(equip->istr); // STR
	packet.addShort(equip->idex); // DEX
	packet.addShort(equip->iint); // INT
	packet.addShort(equip->iluk); // LUK
	packet.addShort(equip->ihp); // HP
	packet.addShort(equip->imp); // MP
	packet.addShort(equip->iwatk); // W.Atk
	packet.addShort(equip->imatk); // M.Atk
	packet.addShort(equip->iwdef); // W.def
	packet.addShort(equip->imdef); // M.Def
	packet.addShort(equip->iacc); // Acc		
	packet.addShort(equip->iavo); // Avo		
	packet.addShort(equip->ihand); // Hands		
	packet.addShort(equip->ispeed); // Speed		
	packet.addShort(equip->ijump); // Jump		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);		
	packet.addShort(0);
}

void PlayerPacketHelper::addIP(Packet &packet, char *ip, int port) {
	char *next_token;

	packet.addByte(atoi(strtok_s(ip, ".", &next_token)));
	packet.addByte(atoi(strtok_s(NULL, ".", &next_token)));
	packet.addByte(atoi(strtok_s(NULL, ".", &next_token)));
	packet.addByte(atoi(strtok_s(NULL, ".", &next_token)));

	packet.addShort(port);
}