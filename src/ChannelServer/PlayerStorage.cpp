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
	int8_t inv = GameLogicUtilities::getInventory(item->getId());
	int8_t i;
	for (i = 0; i < (int8_t) items.size(); i++) {
		if (GameLogicUtilities::getInventory(items[i]->getId()) > inv) {
			break;
		}
	}
	items.insert(items.begin() + i, item);
}

int8_t PlayerStorage::getNumItems(int8_t inv) {
	int8_t itemNum = 0;
	for (int8_t i = 0; i < (int8_t) items.size(); i++) {
		if (GameLogicUtilities::getInventory(items[i]->getId()) == inv) {
			itemNum++;
		}
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
		// Make a row right away...
		query << "INSERT INTO storage (userid, world_id, slots, mesos) VALUES ("
			<< player->getUserId() << ", "
			<< (int16_t) player->getWorldId() << ", "
			<< (int16_t) getSlots() << ", "
			<< getMesos() << ") ";
		query.exec();
	}

	items.reserve(slots);

	enum TableFields {
		UserId = 0,
		WorldId, Slot, ItemId, Amount, Slots,
		Scrolls, iStr, iDex, iInt, iLuk,
		iHp, iMp, iWatk, iMatk, iWdef,
		iMdef, iAcc, iAvo, iHand, iSpeed,
		iJump, Flags, Hammers, Name, CashID
	};

	query << "SELECT * FROM storage_items WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId() << " ORDER BY slot ASC";
	res = query.store();
	string temp;
	for (size_t i = 0; i < res.num_rows(); i++) {
		mysqlpp::Row &row = res[i];
		Item *item = new Item(row[ItemId]);
		item->setAmount(row[Amount]);
		item->setSlots(static_cast<int8_t>(row[Slots]));
		item->setScrolls(static_cast<int8_t>(row[Scrolls]));
		item->setStr(row[iStr]);
		item->setDex(row[iDex]);
		item->setInt(row[iInt]);
		item->setLuk(row[iLuk]);
		item->setHp(row[iHp]);
		item->setMp(row[iMp]);
		item->setWatk(row[iWatk]);
		item->setMatk(row[iMatk]);
		item->setWdef(row[iWdef]);
		item->setMdef(row[iMdef]);
		item->setAccuracy(row[iAcc]);
		item->setAvoid(row[iAvo]);
		item->setHands(row[iHand]);
		item->setSpeed(row[iSpeed]);
		item->setJump(row[iJump]);
		item->setFlags(static_cast<int16_t>(row[Flags]));
		item->setHammers(row[Hammers]);
		row[Name].to_string(temp);
		item->setName(temp);
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

	query << "DELETE FROM storage_items WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId();
	query.exec();

	bool firstrun = true;
	for (int8_t i = 0; i < getNumItems(); i++) {
		if (firstrun) {
			query << "INSERT INTO storage_items VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		Item *item = getItem(i);
		query << player->getUserId() << ","
			<< (int16_t) player->getWorldId() << ","
			<< (int16_t) i << ","
			<< item->getId() << ","
			<< item->getAmount() << ","
			<< (int16_t) item->getSlots() << ","
			<< (int16_t) item->getScrolls() << ","
			<< item->getStr() << ","
			<< item->getDex() << ","
			<< item->getInt() << ","
			<< item->getLuk() << ","
			<< item->getHp() << ","
			<< item->getMp() << ","
			<< item->getWatk() << ","
			<< item->getMatk() << ","
			<< item->getWdef() << ","
			<< item->getMdef() << ","
			<< item->getAccuracy() << ","
			<< item->getAvoid() << ","
			<< item->getHands() << ","
			<< item->getSpeed() << ","
			<< item->getJump() << ","
			<< item->getFlags() << ","
			<< item->getHammers() << ","
			<< mysqlpp::quote << item->getName() << ")";
	}
	if (!firstrun) {
		query.exec();
	}
}

void PlayerStorage::changeNxCredit(int32_t val) {
	if (val < 0) {
		if (-val > getNxCredit()) {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET credit_nx = 0 WHERE userid = " << player->getUserId();
			query.exec();
		}
		else {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET credit_nx = credit_nx + " << val << " WHERE userid = " << player->getUserId();
			query.exec();
		}
	}
	else {
		if (getNxCredit() + val < 0) {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET credit_nx = " << INT_MAX << " WHERE userid = " << player->getUserId();
			query.exec();
		}
		else {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET credit_nx = credit_nx + " << val << " WHERE userid = " << player->getUserId();
			query.exec();
		}
	}
}

void PlayerStorage::changeNxPrepaid(int32_t val) {
	if (val < 0) {
		if (-val > getNxPrepaid()) {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET prepaid_nx = 0 WHERE userid = " << player->getUserId();
			query.exec();
		}
		else {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET prepaid_nx = prepaid_nx + " << val << " WHERE userid = " << player->getUserId();
			query.exec();
		}
	}
	else {
		if (getNxPrepaid() + val < 0) {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET prepaid_nx = " << INT_MAX << " WHERE userid = " << player->getUserId();
			query.exec();
		}
		else {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET prepaid_nx = prepaid_nx + " << val << " WHERE userid = " << player->getUserId();
			query.exec();
		}
	}
}

void PlayerStorage::changeMaplePoints(int32_t val) {
	if (val < 0) {
		if (-val > getMaplePoints()) {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET maplepoints = 0 WHERE userid = " << player->getUserId();
			query.exec();
		}
		else {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET maplepoints = maplepoints + " << val << " WHERE userid = " << player->getUserId();
			query.exec();
		}
	}
	else {
		if (getMaplePoints() + val < 0) {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET maplepoints = " << INT_MAX << " WHERE userid = " << player->getUserId();
			query.exec();
		}
		else {
			mysqlpp::Query query = Database::getCharDB().query();
			query << "UPDATE storage SET maplepoints = maplepoints + " << val << " WHERE userid = " << player->getUserId();
			query.exec();
		}
	}
}

int32_t PlayerStorage::getNxCredit() {
	mysqlpp::Query query = Database::getCharDB().query();	
	query << "SELECT credit_nx FROM storage WHERE userid = " << player->getUserId();
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() == 0) {
		return 0;
	}
	else {
		return atoi(res[0][0]);
	}
}

int32_t PlayerStorage::getNxPrepaid() {
	mysqlpp::Query query = Database::getCharDB().query();	
	query << "SELECT prepaid_nx FROM storage WHERE userid = " << player->getUserId();
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() == 0) {
		return 0;
	}
	else {
		return atoi(res[0][0]);
	}
}

int32_t PlayerStorage::getMaplePoints() {
	mysqlpp::Query query = Database::getCharDB().query();	
	query << "SELECT maplepoints FROM storage WHERE userid = " << player->getUserId();
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() == 0) {
		return 0;
	}
	else {
		return atoi(res[0][0]);
	}
}