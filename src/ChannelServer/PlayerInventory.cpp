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
#include "MySQLM.h"

void PlayerInventory::setMesos(int mesos, bool is) {
	if (mesos < 0)
		mesos = 0;
	this->mesos = mesos;
	PlayerPacket::updateStat(player, 0x40000, mesos, is);
}
void PlayerInventory::addEquip(short slot, Equip *equip) {
	equips[slot] = equip;
	if (itemamounts.find(equip->id) != itemamounts.end())
		itemamounts[equip->id] += 1;
	else
		itemamounts[equip->id] = 1;
}

void PlayerInventory::setEquip(short slot, Equip *equip) {
	if (equip == 0)
		equips.erase(slot);
	else
		equips[slot] = equip;
}

Equip * PlayerInventory::getEquip(short slot) {
	if (equips.find(slot) != equips.end())
		return equips[slot];
	return 0;
}

void PlayerInventory::deleteEquip(short slot) {
	if (equips.find(slot) != equips.end()) {
		itemamounts[equips[slot]->id] -= 1;
		delete equips[slot];
		equips.erase(slot);
	}
}

void PlayerInventory::addItem(char inv, short slot, Item *item) {
	items[inv-2][slot] = item;
	if (itemamounts.find(item->id) != itemamounts.end())
		itemamounts[item->id] += item->amount;
	else
		itemamounts[item->id] = item->amount;
}

Item * PlayerInventory::getItem(char inv, short slot) {
	inv -= 2;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot];
	return 0;
}

void PlayerInventory::deleteItem(char inv, short slot) {
	inv -= 2;
	if (items[inv].find(slot) != items[inv].end()) {
		itemamounts[items[inv][slot]->id] -= items[inv][slot]->amount;
		delete items[inv][slot];
		items[inv].erase(slot);
	}
}

void PlayerInventory::setItem(char inv, short slot, Item *item) {
	inv -= 2;
	if (item == 0)
		items[inv].erase(slot);
	else
		items[inv][slot] = item;
}

short PlayerInventory::getItemAmountBySlot(char inv, short slot) {
	inv -= 2;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot]->amount;
	return 0;
}

int PlayerInventory::getItemAmount(int itemid) {
	if (itemamounts.find(itemid) != itemamounts.end())
		return itemamounts[itemid];
	return 0;
}

void PlayerInventory::addMaxSlots(char inventory, char rows) { // Useful with .lua
	maxslots[inventory - 1] += (rows * 4);
	if (maxslots[inventory - 1] > 100)
		maxslots[inventory - 1] = 100;
	if (maxslots[inventory - 1] < 24) // Retard.
		maxslots[inventory - 1] = 24;
	InventoryPacket::updateSlots(player, inventory, maxslots[inventory - 1]);
}
