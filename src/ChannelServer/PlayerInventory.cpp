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
#include "PlayerInventory.h"
#include "InventoryPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PacketCreator.h"

PlayerInventory::PlayerInventory(Player *player, unsigned char maxslots[5]) : player(player) {
	memcpy_s(this->maxslots, sizeof(this->maxslots), maxslots, sizeof(this->maxslots));
	memset(this->equipped, 0, sizeof(this->equipped));
}

void PlayerInventory::addMaxSlots(char inventory, char rows) { // Useful with .lua
	inventory -= 1;
	maxslots[inventory] += (rows * 4);
	if (maxslots[inventory] > 100)
		maxslots[inventory] = 100;
	if (maxslots[inventory] < 24) // Retard.
		maxslots[inventory] = 24;
	InventoryPacket::updateSlots(player, inventory, maxslots[inventory - 1]);
}

void PlayerInventory::setMesos(int mesos, bool is) {
	if (mesos < 0)
		mesos = 0;
	this->mesos = mesos;
	PlayerPacket::updateStat(player, 0x40000, mesos, is);
}

void PlayerInventory::addItem(char inv, short slot, Item *item) {
	items[inv-1][slot] = item;
	if (itemamounts.find(item->id) != itemamounts.end())
		itemamounts[item->id] += item->amount;
	else
		itemamounts[item->id] = item->amount;
	if (slot < 0)
		addEquipped(slot, item->id);
}

Item * PlayerInventory::getItem(char inv, short slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot];
	return 0;
}

void PlayerInventory::deleteItem(char inv, short slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end()) {
		itemamounts[items[inv][slot]->id] -= items[inv][slot]->amount;
		if (slot < 0)
			addEquipped(slot, 0);
		delete items[inv][slot];
		items[inv].erase(slot);
	}
}

void PlayerInventory::setItem(char inv, short slot, Item *item) {
	inv -= 1;
	if (item == 0) {
		items[inv].erase(slot);
		if (slot < 0)
			addEquipped(slot, 0);
	}
	else {
		items[inv][slot] = item;
		if (slot < 0)
			addEquipped(slot, item->id);
	}
}

short PlayerInventory::getItemAmountBySlot(char inv, short slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot]->amount;
	return 0;
}

void PlayerInventory::addEquipped(short slot, int itemid) {
	bool cash = false;
	slot = abs(slot);
	if (slot > 100) {
		cash = true;
		slot -= 100;
	}
	if (equipped[slot][0] > 0) {
		if (cash && itemid > 0) {
			equipped[slot][1] = equipped[slot][0];
			equipped[slot][0] = itemid;
		}
		else if (cash) {
			equipped[slot][0] = equipped[slot][1];
			equipped[slot][1] = itemid;
		}
		else if (equipped[slot][1] <= 0) {
			equipped[slot][0] = itemid;
		}
		else {
			equipped[slot][1] = itemid;
		}
	}
	else {
		equipped[slot][0] = itemid;
	}
}

void PlayerInventory::addEquippedPacket(PacketCreator &packet) {
	for (char i = 0; i < 50; i++) { // Shown items
		if (equipped[i][0] > 0) {
			packet.addByte(i);
			if (i == 11 && equipped[i][1] > 0) // Normal weapons always here
				packet.addInt(equipped[i][1]);
			else
				packet.addInt(equipped[i][0]);
		}
	}
	packet.addByte(-1);
	for (char i = 0; i < 50; i++) { // Covered items
		if (equipped[i][1] > 0 && i != 11) {
			packet.addByte(i);
			packet.addInt(equipped[i][1]);
		}
	}
	packet.addByte(-1);
	packet.addInt(equipped[11][0]); // Cash weapon
}

int PlayerInventory::getItemAmount(int itemid) {
	if (itemamounts.find(itemid) != itemamounts.end())
		return itemamounts[itemid];
	return 0;
}
