/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "ChannelServer.h"
#include "Database.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "MiscUtilities.h"
#include "Player.h"
#include "StoragePacket.h"
#include <algorithm>

PlayerStorage::PlayerStorage(Player *player) :
	m_player(player)
{
	load();
}

PlayerStorage::~PlayerStorage() {
	std::for_each(m_items.begin(), m_items.end(), MiscUtilities::DeleterSeq<Item>());
}

void PlayerStorage::takeItem(uint8_t slot) {
	vector<Item *>::iterator iter = m_items.begin() + slot;
	delete *iter;
	m_items.erase(iter);
}

void PlayerStorage::setSlots(uint8_t slots) {
	m_slots = MiscUtilities::constrainToRange(slots, Inventories::MinSlotsStorage, Inventories::MaxSlotsStorage);
}

void PlayerStorage::addItem(Item *item) {
	uint8_t inv = GameLogicUtilities::getInventory(item->getId());
	uint8_t i;
	for (i = 0; i < m_items.size(); ++i) {
		if (GameLogicUtilities::getInventory(m_items[i]->getId()) > inv) {
			break;
		}
	}
	m_items.insert(m_items.begin() + i, item);
}

uint8_t PlayerStorage::getNumItems(uint8_t inv) {
	int8_t itemNum = 0;
	for (uint8_t i = 0; i < m_items.size(); ++i) {
		if (GameLogicUtilities::getInventory(m_items[i]->getId()) == inv) {
			itemNum++;
		}
	}
	return itemNum;
}

void PlayerStorage::changeMesos(int32_t mesos) {
	m_mesos -= mesos;
	StoragePacket::changeMesos(m_player, m_mesos);
}

void PlayerStorage::load() {
	soci::session &sql = Database::getCharDb();
	soci::row row;
	int32_t userId = m_player->getUserId();
	int8_t worldId = m_player->getWorldId();

	sql.once
		<< "SELECT s.slots, s.mesos, s.char_slots "
		<< "FROM storage s "
		<< "WHERE s.user_id = :user AND s.world_id = :world "
		<< "LIMIT 1",
		soci::use(userId, "user"),
		soci::use(worldId, "world"),
		soci::into(row);

	if (sql.got_data()) {
		m_slots = row.get<uint8_t>("slots");
		m_mesos = row.get<int32_t>("mesos");
		m_charSlots = row.get<int32_t>("char_slots");
	}
	else {
		m_slots = ChannelServer::Instance()->getDefaultStorageSlots();
		m_mesos = 0;
		m_charSlots = ChannelServer::Instance()->getDefaultChars();
		sql.once
			<< "INSERT INTO storage (user_id, world_id, slots, mesos, char_slots) "
			<< "VALUES (:user, :world, :slots, :mesos, :chars)",
			soci::use(userId, "user"),
			soci::use(worldId, "world"),
			soci::use(m_slots, "slots"),
			soci::use(m_mesos, "mesos"),
			soci::use(m_charSlots, "chars");
	}

	m_items.reserve(m_slots);

	string location = "storage";

	soci::rowset<> rs = (sql.prepare
		<< "SELECT i.* "
		<< "FROM items i "
		<< "WHERE i.location = :location AND i.user_id = :user AND i.world_id = :world "
		<< "ORDER BY i.slot ASC",
		soci::use(location, "location"),
		soci::use(userId, "user"),
		soci::use(worldId, "world"));

	for (const auto &row : rs) {
		Item *item = new Item(row);
		addItem(item);
	}
}

void PlayerStorage::save() {
	using namespace soci;
	int8_t worldId = m_player->getWorldId();
	int32_t userId = m_player->getUserId();
	int32_t playerId = m_player->getId();

	session &sql = Database::getCharDb();
	sql.once
		<< "UPDATE storage "
		<< "SET slots = :slots, mesos = :mesos, char_slots = :chars "
		<< "WHERE user_id = :user AND world_id = :world",
		use(userId, "user"),
		use(worldId, "world"),
		use(m_slots, "slots"),
		use(m_mesos, "mesos"),
		use(m_charSlots, "chars");

	sql.once
		<< "DELETE FROM items "
		<< "WHERE location = :location AND user_id = :user AND world_id = :world",
		use(Item::Storage, "location"),
		use(userId, "user"),
		use(worldId, "world");

	uint8_t max = getNumItems();

	if (max > 0) {
		vector<ItemDbRecord> v;
		for (uint8_t i = 0; i < max; ++i) {
			ItemDbRecord rec(i, playerId, userId, worldId, Item::Storage, getItem(i));
			v.push_back(rec);
		}
		Item::databaseInsert(sql, v);
	}
}