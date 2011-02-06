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
#include "CashItem.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerPacketHelper.h"
#include "StringUtilities.h"
#include "TimeUtilities.h"
#include <algorithm>

PlayerStorage::PlayerStorage(Player *player) : player(player) {
	load();
}

PlayerStorage::~PlayerStorage() {
	std::for_each(items.begin(), items.end(), MiscUtilities::DeleterSeq<Item>());
	cashStorage.clear();
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
		if (GameLogicUtilities::getInventory(items[i]->getId()) > inv)
			break;
	}
	items.insert(items.begin() + i, item);
}

void PlayerStorage::takeCashItem(int64_t id) {
	delete cashStorage[id];
	cashStorage.erase(id);
}

void PlayerStorage::addCashItem(CashItem *item) {
	if (item->getId() == -1) {
		// Lets add the item to the database
		mysqlpp::Query query = Database::getCharDB().query();
		query << "INSERT INTO storage_cash VALUES (NULL, "
			<< player->getUserId() << ", "
			<< (int16_t) player->getWorldId() << ", "
			<< item->getItemId() << ", "
			<< item->getAmount() << ", "
			<< mysqlpp::quote << item->getName() << ", "
			<< item->getPetId() << ", "
			<< item->getExpirationTime() << ")";
		query.exec();
		item->setId(static_cast<int32_t>(query.insert_id()));
	}
	cashStorage[item->getId()] = item;
}

void PlayerStorage::removeGift(int64_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "DELETE FROM character_cashshop_gifts WHERE cashid = " << id;
	query.exec();

	delete gifts[id];
	gifts.erase(id);	
}

int8_t PlayerStorage::getNumItems(int8_t inv) {
	int8_t itemNum = 0;
	for (int8_t i = 0; i < (int8_t) items.size(); i++) {
		if (GameLogicUtilities::getInventory(items[i]->getId()) == inv)
			itemNum++;
	}
	return itemNum;
}

void PlayerStorage::changeMesos(int32_t mesos) {
	this->mesos -= mesos;
}

void PlayerStorage::load() {
	cashStorage.clear();
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT slots, char_slots, mesos FROM storage WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId();
	mysqlpp::StoreQueryResult res = query.store();
	if (res.num_rows() != 0) {
		slots = (int8_t) res[0][0];
		charSlots = (int8_t) res[0][1];
		mesos = res[0][2];
	}
	else {
		slots = 4;
		charSlots = 3;
		mesos = 0;
		// Make a row right away...
		query << "INSERT INTO storage (userid, world_id, slots, char_slots, mesos) VALUES ("
			<< player->getUserId() << ", "
			<< (int16_t) player->getWorldId() << ", "
			<< (int16_t) getSlots() << ", "
			<< (int16_t) getCharSlots() << ", "
			<< getMesos() << ")";
		query.exec();
	}

	items.reserve(slots);

	enum TableFields {
		UserId = 0,
		WorldId, Slot, ItemId, Amount, Slots,
		Scrolls, iStr, iDex, iInt, iLuk,
		iHp, iMp, iWatk, iMatk, iWdef,
		iMdef, iAcc, iAvo, iHand, iSpeed,
		iJump, Flags, Hammers, Name
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

	query << "SELECT id, itemid, amount, expires, `from`, userid, petid FROM storage_cash WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId();
	res = query.store();
	CashItem *item;

	for (size_t i = 0; i < res.num_rows(); i++) {
		mysqlpp::Row &row = res[i];
		item = new CashItem();
		item->setId(row[0]);
		item->setItemId(row[1]);
		item->setAmount(row[2]);
		item->setExpirationTime(row[3]);
		row[4].to_string(temp);
		item->setName(temp);
		item->setUserId(row[5]);
		item->setPetId(row[6]);

		addCashItem(item);
	}

	query << "SELECT cashid, itemid, sender, message FROM character_cashshop_gifts WHERE charid = " << player->getId();
	res = query.store();
	CashshopGift *gift;

	for (size_t i = 0; i < res.num_rows(); i++) {
		mysqlpp::Row &row = res[i];
		gift = new CashshopGift();
		gift->cashid = atoi(row[0]);
		gift->itemid = atoi(row[1]);
		row[2].to_string(gift->sender);
		row[3].to_string(gift->message);
		gifts[gift->cashid] = gift;
	}
}
void PlayerStorage::save() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "INSERT INTO storage (userid, world_id, slots, char_slots, mesos) VALUES ("
		<< player->getUserId() << ", "
		<< (int16_t) player->getWorldId() << ", "
		<< (int16_t) getSlots() << ", "
		<< (int16_t) getCharSlots() << ", "
		<< getMesos() << ")"
		<< "ON DUPLICATE KEY UPDATE slots = " << (int16_t) getSlots() << ", "
		<< "char_slots = " << (int16_t) getCharSlots() << ", "
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
	if (!firstrun)
		query.exec();

	query << "DELETE FROM storage_cash WHERE userid = " << player->getUserId() << " AND world_id = " << (int16_t) player->getWorldId();
	query.exec();

	firstrun = true;
	for (unordered_map<int64_t, CashItem *>::iterator iter = cashStorage.begin(); iter != cashStorage.end(); iter++) {
		if (iter->second == nullptr) {
			continue; // Temp fix I guess...
		}
		if (firstrun) {
			query << "INSERT INTO storage_cash VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << iter->first << ", "
			<< iter->second->getUserId() << ", "
			<< (int16_t) player->getWorldId() << ", "
			<< iter->second->getItemId() << ", "
			<< iter->second->getAmount() << ", "
			<< mysqlpp::quote << iter->second->getName() << ", "
			<< iter->second->getPetId() << ", "
			<< iter->second->getExpirationTime() << ")";
	}
	if (!firstrun)
		query.exec();
}

void PlayerStorage::checkExpiredItems() {
	CashItem *item;
	vector<int64_t> expiredItems;
	for (size_t i = 0; i < cashStorage.size(); i++) {
		item = cashStorage[i];
		if (item != nullptr && item->getExpirationTime() != Items::NoExpiration && item->getExpirationTime() <= TimeUtilities::getServerTime()) {
			PlayerPacket::sendItemExpired(player, item->getId());
			expiredItems.push_back(item->getId());
		}
	}
	for (size_t i = 0; i < expiredItems.size(); i++) {
		takeCashItem(expiredItems[i]);
	}
}

void PlayerStorage::cashItemsPacket(PacketCreator &packet) {
	packet.add<int16_t>(cashStorage.size());
	for (unordered_map<int64_t, CashItem *>::iterator iter = cashStorage.begin(); iter != cashStorage.end(); iter++) {
		PlayerPacketHelper::addCashItemInfo(packet, iter->second);
	}
}

void PlayerStorage::giftPacket(PacketCreator &packet) {
	packet.add<int16_t>(gifts.size());
	CashshopGift *gift;
	for (unordered_map<int64_t, CashshopGift *>::iterator iter = gifts.begin(); iter != gifts.end(); iter++) {
		gift = iter->second;
		packet.add<int64_t>(gift->cashid);
		packet.add<int32_t>(gift->itemid);
		packet.addString(gift->sender, 13);
		packet.addString(gift->message, 73);
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