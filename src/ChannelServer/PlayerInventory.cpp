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
	this->mesos=mesos;
	PlayerPacket::updateStat(player, 0x40000, mesos, is);
}

void PlayerInventory::deleteEquip(int equipid) {
	mysqlpp::Query query = db.query();

	query << "DELETE FROM equip WHERE charid=" << mysqlpp::quote << player->getPlayerid()
		<< " and pos=" << mysqlpp::quote << equips[equipid]->pos;
	query.exec();
	delete equips[equipid];
	equips.erase(equips.begin()+equipid);
}

void PlayerInventory::deleteItem(int itemid) {
	mysqlpp::Query query = db.query();

	query << "DELETE FROM items WHERE charid=" << mysqlpp::quote << player->getPlayerid()
		<< " and inv=" << mysqlpp::quote << (short) items[itemid]->inv
		<< " and pos=" << mysqlpp::quote << items[itemid]->pos;
	query.exec();

	delete items[itemid];
	items.erase(items.begin()+itemid);
}
