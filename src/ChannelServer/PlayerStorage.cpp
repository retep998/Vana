/*
Copyright (C) 2008-2010 Vana Development Team

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
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "MiscUtilities.h"
#include "Player.h"
#include "StoragePacket.h"
#include <algorithm>

PlayerStorage::PlayerStorage(Player *player) : player(player) {
	load();
}

PlayerStorage::~PlayerStorage() {
	std::for_each(items.begin(), items.end(), MiscUtilities::DeleterSeq<Item>());
}

void PlayerStorage::takeItem(int8_t slot) {
	vector<Item *>::iterator iter = items.begin() + slot;
	delete *iter;
	items.erase(iter);
}

void PlayerStorage::setSlots(int8_t slots) {
	if (slots < 4) slots = 4;
	else if (slots > 100) slots = 100;
	this->slots = slots;
}

void PlayerStorage::addItem(Item *item) {
	int8_t inv = GameLogicUtilities::getInventory(item->id);
	int8_t i;
	for (i = 0; i < (int8_t) items.size(); i++) {
		if (GameLogicUtilities::getInventory(items[i]->id) > inv)
			break;
	}
	items.insert(items.begin() + i, item);
}

int8_t PlayerStorage::getNumItems(int8_t inv) {
	int8_t itemNum = 0;
	for (int8_t i = 0; i < (int8_t) items.size(); i++) {
		if (GameLogicUtilities::getInventory(items[i]->id) == inv)
			itemNum++;
	}
	return itemNum;
}

void PlayerStorage::changeMesos(int32_t mesos) {
	this->mesos -= mesos;
	StoragePacket::changeMesos(player, this->mesos);
}

void PlayerStorage::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT slots, mesos FROM storage WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId();
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() != 0) {
		slots = (uint8_t) res[0][0];
		mesos = res[0][1];
	}
	else {
		slots = 4;
		mesos = 0;
	}

	items.reserve(slots);

	query << "SELECT itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, flags, hammers, name FROM storageitems WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId() << " ORDER BY slot ASC";
	res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		Item *item = new Item;
		item->id = res[i][0];
		item->amount = res[i][1];
		item->slots = (int8_t) res[i][2];
		item->scrolls = (int8_t) res[i][3];
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
		item->flags = (int8_t) res[i][19];
		item->hammers = res[i][20];
		res[i][21].to_string(item->name);
		addItem(item);
	}
}

void PlayerStorage::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	// Using MySQL's non-standard ON DUPLICATE KEY UPDATE extension
	query << "INSERT INTO storage (userid, world_id, slots, mesos) VALUES ("
		<< player->getUserId() << ", "
		<< (int16_t) player->getWorldId() << ", "
		<< (int16_t) getSlots() << ", "
		<< getMesos() << ") "
		<< "ON DUPLICATE KEY UPDATE slots = " << (int16_t) getSlots() << ", "
		<< "mesos = " << getMesos();
	query.exec();

	query << "DELETE FROM storageitems WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId();
	query.exec();

	bool firstrun = true;
	for (int8_t i = 0; i < getNumItems(); i++) {
		if (firstrun) {
			query << "INSERT INTO storageitems VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		Item *item = getItem(i);
		query << player->getUserId() << ","
			<< (int16_t) player->getWorldId() << ","
			<< (int16_t) i << ","
			<< item->id << ","
			<< item->amount << ","
			<< (int16_t) item->slots << ","
			<< (int16_t) item->scrolls << ","
			<< item->istr << ","
			<< item->idex << ","
			<< item->iint << ","
			<< item->iluk << ","
			<< item->ihp << ","
			<< item->imp << ","
			<< item->iwatk << ","
			<< item->imatk << ","
			<< item->iwdef << ","
			<< item->imdef << ","
			<< item->iacc << ","
			<< item->iavo << ","
			<< item->ihand << ","
			<< item->ispeed << ","
			<< item->ijump << ","
			<< (int16_t) item->flags << ","
			<< item->hammers << ","
			<< mysqlpp::quote << item->name << ")";
	}
	if (!firstrun)
		query.exec();
}