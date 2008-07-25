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
void PlayerInventory::addMaxSlots(char inventory, char rows) { // Useful with .lua
	maxslots[inventory - 1] += (rows * 4);
	if (maxslots[inventory - 1] > 100)
		maxslots[inventory - 1] = 100;
	if (maxslots[inventory - 1] < 24) // Retard.
		maxslots[inventory - 1] = 24;
	InventoryPacket::updateSlots(player, inventory, maxslots[inventory - 1]);
}
