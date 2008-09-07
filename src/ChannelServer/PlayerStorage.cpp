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
#include "Inventory.h"
#include "Player.h"
#include "PlayerInventory.h"
#include "StoragePacket.h"
#include "MySQLM.h"

PlayerStorage::PlayerStorage(Player *player) : player(player) {
	load();
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
		if (GETINVENTORY(items[i]->id) == inv)
			itemNum ++;
	}
	return itemNum;
}

void PlayerStorage::changeMesos(int mesos) {
	this->mesos -= mesos;
	StoragePacket::changeMesos(player, this->mesos);
}

void PlayerStorage::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT slots, mesos FROM storage WHERE userid = " << player->getUserId() << " AND world_id = " << (short) player->getWorldId();
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() != 0) {
		slots = (unsigned char) res[0][0];
		mesos = res[0][1];
	}
	else {
		slots = 4;
		mesos = 0;
	}

	query << "SELECT itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, name FROM storageitems WHERE userid = " << mysqlpp::quote << player->getUserId() << " AND world_id = " << (short) player->getWorldId() << " ORDER BY slot ASC";
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		Item *item = new Item;
		item->id = res[i][0];
		item->amount = res[i][1];
		item->slots = (unsigned char) res[i][2];
		item->scrolls = (unsigned char) res[i][3];
		item->istr = res[i][4];
		item->idex = res[i][5];
		item->iint = res[i][6];
		item->iluk = res[i][7];
		item->ihp = res[i][8];
		item->imp = res[i][9];
		item->iwatk = res[i][10];
		item->imatk = res[i][11];
		item->iwdef = res[i][12];
		item->imdef = res[i][13];
		item->iacc = res[i][14];
		item->iavo = res[i][15];
		item->ihand = res[i][16];
		item->ispeed = res[i][17];
		item->ijump = res[i][18];
		res[i][19].to_string(item->name);
		addItem(item);
	}
}

void PlayerStorage::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	// Using MySQL's non-standard ON DUPLICATE KEY UPDATE extension
	query << "INSERT INTO storage VALUES ("
		<< player->getUserId() << ", "
		<< (short) player->getWorldId() << ", "
		<< (short) getSlots() << ", "
		<< getMesos() << ") "
		<< "ON DUPLICATE KEY UPDATE slots = " << (short) getSlots() << ", "
		<< "mesos = " << getMesos();
	query.exec();

	query << "DELETE FROM storageitems WHERE userid = " << player->getUserId() << " AND world_id = " << (short) player->getWorldId();
	query.exec();

	bool firstrun = true;
	for (char i = 0; i < getNumItems(); i++) {
		if (firstrun) {
			query << "INSERT INTO storageitems VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		Item *item = getItem(i);
		query << mysqlpp::quote << player->getUserId() << ","
			<< mysqlpp::quote << (short) player->getWorldId() << ","
			<< mysqlpp::quote << (short) i << ","
			<< mysqlpp::quote << item->id << ","
			<< mysqlpp::quote << item->amount << ","
			<< mysqlpp::quote << (short) item->slots << ","
			<< mysqlpp::quote << (short) item->scrolls << ","
			<< mysqlpp::quote << item->istr << ","
			<< mysqlpp::quote << item->idex << ","
			<< mysqlpp::quote << item->iint << ","
			<< mysqlpp::quote << item->iluk << ","
			<< mysqlpp::quote << item->ihp << ","
			<< mysqlpp::quote << item->imp << ","
			<< mysqlpp::quote << item->iwatk << ","
			<< mysqlpp::quote << item->imatk << ","
			<< mysqlpp::quote << item->iwdef << ","
			<< mysqlpp::quote << item->imdef << ","
			<< mysqlpp::quote << item->iacc << ","
			<< mysqlpp::quote << item->iavo << ","
			<< mysqlpp::quote << item->ihand << ","
			<< mysqlpp::quote << item->ispeed << ","
			<< mysqlpp::quote << item->ijump << ","
			<< mysqlpp::quote << item->name << ")";
	}
	query.exec();
}