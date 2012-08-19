/*
Copyright (C) 2008-2012 Vana Development Team

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

	sql.once << "SELECT s.slots, s.mesos, s.char_slots FROM storage s WHERE s.user_id = :user AND s.world_id = :world LIMIT 1",
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
		sql.once << "INSERT INTO storage (user_id, world_id, slots, mesos, char_slots) " <<
					"VALUES (:user, :world, :slots, :mesos, :chars)",
					soci::use(userId, "user"),
					soci::use(worldId, "world"),
					soci::use(m_slots, "slots"),
					soci::use(m_mesos, "mesos"),
					soci::use(m_charSlots, "chars");
	}

	m_items.reserve(m_slots);

	string location = "storage";

	soci::rowset<> rs = (sql.prepare << "SELECT i.* FROM items i " <<
										"WHERE i.location = :location AND i.user_id = :user AND i.world_id = :world " <<
										"ORDER BY i.slot ASC",
										soci::use(location, "location"),
										soci::use(userId, "user"),
										soci::use(worldId, "world"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		Item *item = new Item(row.get<int32_t>("item_id"));
		item->setAmount(row.get<int16_t>("amount"));
		item->setSlots(row.get<int8_t>("slots"));
		item->setScrolls(row.get<int8_t>("scrolls"));
		item->setStr(row.get<int16_t>("istr"));
		item->setDex(row.get<int16_t>("idex"));
		item->setInt(row.get<int16_t>("iint"));
		item->setLuk(row.get<int16_t>("iluk"));
		item->setHp(row.get<int16_t>("ihp"));
		item->setMp(row.get<int16_t>("imp"));
		item->setWatk(row.get<int16_t>("iwatk"));
		item->setMatk(row.get<int16_t>("imatk"));
		item->setWdef(row.get<int16_t>("iwdef"));
		item->setMdef(row.get<int16_t>("imdef"));
		item->setAccuracy(row.get<int16_t>("iacc"));
		item->setAvoid(row.get<int16_t>("iavo"));
		item->setHands(row.get<int16_t>("ihand"));
		item->setSpeed(row.get<int16_t>("ispeed"));
		item->setJump(row.get<int16_t>("ijump"));
		item->setFlags(row.get<int16_t>("flags"));
		item->setHammers(row.get<int8_t>("hammers"));
		item->setExpirationTime(row.get<int64_t>("expiration"));
		item->setName(row.get<string>("name"));
		item->setPetId(row.get<int32_t>("pet_id"));
		addItem(item);
	}
}

void PlayerStorage::save() {
	using namespace soci;
	int8_t worldId = m_player->getWorldId();
	int32_t userId = m_player->getUserId();
	int32_t playerId = m_player->getId();
	string location = "storage";

	session &sql = Database::getCharDb();
	sql.once << "UPDATE storage " <<
				"SET slots = :slots, mesos = :mesos, char_slots = :chars " <<
				"WHERE user_id = :user AND world_id = :world",
				use(userId, "user"),
				use(worldId, "world"),
				use(m_slots, "slots"),
				use(m_mesos, "mesos"),
				use(m_charSlots, "chars");

	sql.once << "DELETE FROM items WHERE location = :location AND user_id = :user AND world_id = :world",
				use(location, "location"),
				use(userId, "user"),
				use(worldId, "world");

	uint8_t max = getNumItems();

	if (max > 0) {
		uint8_t i = 0;
		int8_t slots = 0;
		int8_t scrolls = 0;
		uint8_t inv = 0;
		int16_t amount = 0;
		int32_t itemId = 0;
		int16_t iStr = 0;
		int16_t iDex = 0;
		int16_t iInt = 0;
		int16_t iLuk = 0;
		int16_t iHp = 0;
		int16_t iMp = 0;
		int16_t iWatk = 0;
		int16_t iMatk = 0;
		int16_t iWdef = 0;
		int16_t iMdef = 0;
		int16_t iAcc = 0;
		int16_t iAvo = 0;
		int16_t iHands = 0;
		int16_t iSpeed = 0;
		int16_t iJump = 0;
		int16_t flags = 0;
		int32_t hammers = 0;
		int64_t petId = 0;
		int64_t expiration = 0;
		string name = "";

		statement st = (sql.prepare << "INSERT INTO items VALUES (" <<
										":player, :inv, :i, :location, :user, " <<
										":world, :item, :amount, :slots, :scrolls, " <<
										":str, :dex, :int, :luk, :hp, " <<
										":mp, :watk, :matk, :wdef, :mdef, " <<
										":acc, :avo, :hands, :speed, :jump, " <<
										":flags, :hammers, :pet, :name, :expiration" <<
										")",
										use(playerId, "player"),
										use(inv, "inv"),
										use(i, "i"),
										use(location, "location"),
										use(userId, "user"),
										use(worldId, "world"),
										use(itemId, "item"),
										use(amount, "amount"),
										use(slots, "slots"),
										use(scrolls, "scrolls"),
										use(iStr, "str"),
										use(iDex, "dex"),
										use(iInt, "int"),
										use(iLuk, "luk"),
										use(iHp, "hp"),
										use(iMp, "mp"),
										use(iWatk, "watk"),
										use(iMatk, "matk"),
										use(iWdef, "wdef"),
										use(iMdef, "mdef"),
										use(iAcc, "acc"),
										use(iAvo, "avo"),
										use(iHands, "hands"),
										use(iSpeed, "speed"),
										use(iJump, "jump"),
										use(flags, "flags"),
										use(hammers, "hammers"),
										use(petId, "pet"),
										use(name, "name"),
										use(expiration, "expiration"));

		for (i = 0; i < max; ++i) {
			Item *item = getItem(i);

			inv = GameLogicUtilities::getInventory(item->getId());
			itemId = item->getId();
			amount = item->getAmount();
			slots = item->getSlots();
			scrolls = item->getScrolls();
			iStr = item->getStr();
			iDex = item->getDex();
			iInt = item->getInt();
			iLuk = item->getLuk();
			iHp = item->getHp();
			iMp = item->getMp();
			iWatk = item->getWatk();
			iMatk = item->getMatk();
			iWdef = item->getWdef();
			iMdef = item->getMdef();
			iAcc = item->getAccuracy();
			iAvo = item->getAvoid();
			iHands = item->getHands();
			iSpeed = item->getSpeed();
			iJump = item->getJump();
			flags = item->getFlags();
			hammers = item->getHammers();
			petId = item->getPetId();
			name = item->getName();
			expiration = item->getExpirationTime();

			st.execute(true);
		}
	}
}