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
#include "PlayerStorage.h"
#include "Player.h"
#include "StoragePacket.h"
#include "Inventory.h"

PlayerStorage::PlayerStorage(Player *player, char slots, int mesos) : slots(slots), mesos(mesos) {
	this->player = player;
}

void PlayerStorage::setSlots(char slots) {
	if (slots < 4) slots = 4;
	else if (slots > 100) slots = 100;
	this->slots = slots;
}

void PlayerStorage::addItem(Item *item) {
	char inv = GETINVENTORY(item->id);
	char i;
	for (i = 0; i < (char) items.size(); i++)
		if (GETINVENTORY(items[i]->id) > inv)
			break;
	items.insert(items.begin() + i, item);
}

char PlayerStorage::getNumItems(char inv) {
	char itemNum = 0;
	for (char i = 0; i < (char) items.size(); i++) {
		if (GETINVENTORY(items[i]->id))
			itemNum ++;
	}
	return itemNum;
}

void PlayerStorage::changeMesos(int mesos) {
	this->mesos -= mesos;
	StoragePacket::changeMesos(player, this->mesos);
}
