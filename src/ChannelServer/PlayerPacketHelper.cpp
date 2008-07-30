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
#include "PacketCreator.h"
#include "Player.h"
#include "Inventory.h"

void PlayerPacketHelper::addItemInfo(PacketCreator &packet, short slot, Item *item, bool shortSlot) {
	if (slot != 0) {
		if (shortSlot)
			packet.addShort(slot);
		else if (slot < 0) {
			char charslot = (char) -slot;
			if (charslot > 100)
				charslot -= 100;
			packet.addByte(charslot);
		}
		else
			packet.addByte((char) slot);
	}
	packet.addByte(!ISEQUIP(item->id) + 1);
	packet.addInt(item->id);
	packet.addShort(0);
	packet.addBytes("8005BB46E61702");
	if (ISEQUIP(item->id)) {
		packet.addByte(item->slots); // Slots
		packet.addByte(item->scrolls); // Scrolls
		packet.addShort(item->istr); // STR
		packet.addShort(item->idex); // DEX
		packet.addShort(item->iint); // INT
		packet.addShort(item->iluk); // LUK
		packet.addShort(item->ihp); // HP
		packet.addShort(item->imp); // MP
		packet.addShort(item->iwatk); // W.Atk
		packet.addShort(item->imatk); // M.Atk
		packet.addShort(item->iwdef); // W.Def
		packet.addShort(item->imdef); // M.Def
		packet.addShort(item->iacc); // Acc
		packet.addShort(item->iavo); // Avo
		packet.addShort(item->ihand); // Hands
		packet.addShort(item->ispeed); // Speed
		packet.addShort(item->ijump); // Jump
		packet.addShort(0); // Owner string goes here
		packet.addShort(0);
		packet.addShort(0);
		packet.addShort(0);
		packet.addShort(0);
		packet.addShort(0);
	}
	else {
		packet.addShort(item->amount); // Amount
		packet.addInt(0);
		if (ISRECHARGEABLE(item->id)) {
			packet.addInt(2);
			packet.addShort(0x54);
			packet.addByte(0);
			packet.addByte(0x34);
		}
	}
}

void PlayerPacketHelper::addPlayerDisplay(PacketCreator &packet, Player *player) {
	packet.addByte(player->getGender());
	packet.addByte(player->getSkin());
	packet.addInt(player->getEyes());
	packet.addByte(1);
	packet.addInt(player->getHair());
	int equips[35][2] = {0};
	iteminventory *playerequips = player->inv->getItems(1);
	for (iteminventory::iterator iter = playerequips->begin(); iter != playerequips->end(); iter++) { // Sort equips
		Item *equip = iter->second;
		if (iter->first < 0) {
			if (equips[equip->type][0] > 0) {
				if (Inventory::isCash(equip->id)) {
					equips[equip->type][1] = equips[equip->type][0];
					equips[equip->type][0] = equip->id;
				}
				else {
					equips[equip->type][1] = equip->id;
				}
			}
			else {
				equips[equip->type][0] = equip->id;
			}
		}
	}
	for (int i = 0; i < 35; i++) { // Shown items
		if (equips[i][0] > 0) {
			packet.addByte(i);
			if (i == 11 && equips[i][1] > 0) // Normal weapons always here
				packet.addInt(equips[i][1]);
			else
				packet.addInt(equips[i][0]);
		}
	}
	packet.addByte(-1);
	for (int i = 0; i < 35; i++) { // Covered items
		if (equips[i][1] > 0 && i != 11) {
			packet.addByte(i);
			packet.addInt(equips[i][1]);
		}
	}
	packet.addByte(-1);
	if (equips[11][1] > 0) // Cash weapon
		packet.addInt(equips[11][0]);
	else
		packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
	packet.addInt(0);
}
