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
#include "Quests.h"
#include "Inventory.h"
#include "Player.h"
#include "QuestDataProvider.h"
#include "PacketReader.h"

bool Quests::giveItem(Player *player, int32_t itemid, int16_t amount) {
	if (amount > 0) {
		Inventory::addNewItem(player, itemid, amount);
	}
	else {
		if (player->getInventory()->getItemAmount(itemid) < amount) { // Player does not have (enough of) what is being taken
			return false;
		}
		Inventory::takeItem(player, itemid, -amount);
	}
	return true;
}

bool Quests::giveMesos(Player *player, int32_t amount) {
	if (amount < 0 && player->getInventory()->getMesos() + amount < 0) { // Do a bit of checking if meso is being taken to see if it's enough
		return false;
	}
	player->getInventory()->modifyMesos(amount);
	return true;
}

void Quests::giveFame(Player *player, int32_t amount) {
	player->getStats()->setFame(player->getStats()->getFame() + static_cast<int16_t>(amount));
}