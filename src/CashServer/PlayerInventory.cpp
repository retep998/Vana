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
#include "PlayerInventory.h"
#include "Database.h"
#include "EquipDataProvider.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "InventoryPacketHelper.h"
#include "ItemDataProvider.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerPacketHelper.h"
#include "TimeUtilities.h"

PlayerInventory::PlayerInventory(Player *player, const boost::array<uint8_t, Inventories::InventoryCount> &maxslots, int32_t mesos) :
m_maxslots(maxslots),
m_mesos(mesos),
m_hammer(-1),
m_player(player)
{
	boost::array<int32_t, 2> g = {0};

	for (size_t i = 0; i < Inventories::EquippedSlots; i++) {
		m_equipped[i] = g;
	}

	load();
}

PlayerInventory::~PlayerInventory() {
	typedef boost::array<ItemInventory, Inventories::InventoryCount> ItemInvArr;
	for (ItemInvArr::iterator iter = m_items.begin(); iter != m_items.end(); iter++) {
		std::for_each(iter->begin(), iter->end(), MiscUtilities::DeleterPairAssoc<ItemInventory::value_type>());
	}
}

void PlayerInventory::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT items.*, pets.* FROM items LEFT JOIN pets ON items.petid = pets.id WHERE charid = " << m_player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	enum TableFields {
		ItemCharId = 0,
		Inv, Slot, ItemId, Amount, Slots,
		Scrolls, iStr, iDex, iInt, iLuk,
		iHp, iMp, iWatk, iMatk, iWdef,
		iMdef, iAcc, iAvo, iHand, iSpeed,
		iJump, Flags, Hammers, PetId, Name,
		CashId, ExpirationTime,
		// End of items, start of pets
		PetsId, Index, PetName, Level, Closeness,
		Fullness
	};

	Item *item;
	string temp;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row &row = res[i];
		item = new Item(row[ItemId]);
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
		item->setPetId(row[PetId]);
		item->setFlags(static_cast<int16_t>(row[Flags]));
		item->setHammers(row[Hammers]);
		item->setCashId(row[CashId]);
		item->setExpirationTime(row[ExpirationTime]);
		row[Name].to_string(temp);
		item->setName(temp);

		addItem((int8_t) row[Inv], row[Slot], item, true);
		if (item->getPetId() != 0) {
			Pet *pet = new Pet(
				m_player,
				item,
				static_cast<int8_t>(row[Index]),
				static_cast<string>(row[PetName]),
				static_cast<uint8_t>(row[Level]),
				static_cast<int16_t>(row[Closeness]),
				static_cast<uint8_t>(row[Fullness]),
				static_cast<uint8_t>(row[Slot])
			);
			m_player->getPets()->addPet(pet);
		}
	}

	query << "SELECT mapindex, mapid FROM teleport_rock_locations WHERE charid = " << m_player->getId();
	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		int8_t index = static_cast<int8_t>(res[i][0]);
		int32_t mapid = res[i][1];

		if (index >= Inventories::TeleportRockMax) {
			m_viplocations.push_back(mapid);
		}
		else {
			m_rocklocations.push_back(mapid);
		}
	}

	query << "SELECT serial FROM character_wishlist WHERE charid = " << m_player->getId();
	res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		addWishListItem(res[i][0]);
	}
}

void PlayerInventory::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "DELETE FROM items WHERE charid = " << m_player->getId();
	query.exec();

	bool firstrun = true;
	for (int8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; i++) {
		ItemInventory &itemsinv = m_items[i - 1];
		for (ItemInventory::iterator iter = itemsinv.begin(); iter != itemsinv.end(); iter++) {
			Item *item = iter->second;
			if (firstrun) {
				query << "INSERT INTO items VALUES (";
				firstrun = false;
			}
			else {
				query << ", (";
			}
			query << m_player->getId() << ","
				<< (int16_t) i << ","
				<< iter->first << ","
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
				<< item->getPetId() << ","
				<< mysqlpp::quote << item->getName() << ","
				<< item->getCashId() << ","
				<< item->getExpirationTime() << ")";
		}
	}
	if (!firstrun)
		query.exec();

	query << "DELETE FROM teleport_rock_locations WHERE charid = " << m_player->getId();
	query.exec();

	for (size_t i = 0; i < m_rocklocations.size(); i++) {
		int32_t mapid = m_rocklocations[i];
		query << "INSERT INTO teleport_rock_locations VALUES ("
				<< m_player->getId() << ","
				<< (uint16_t) i << ","
				<< mapid << ")";
		query.exec();
	}
	for (size_t i = 0; i < m_viplocations.size(); i++) {
		int32_t mapid = m_viplocations[i];
		query << "INSERT INTO teleport_rock_locations VALUES ("
				<< m_player->getId() << ","
				<< (uint16_t) i + Inventories::TeleportRockMax << ","
				<< mapid << ")";
		query.exec();
	}

	query << "DELETE FROM character_wishlist WHERE charid = " << m_player->getId();
	query.exec();

	for (size_t i = 0; i < m_wishlist.size(); i++) {
		int32_t serial = m_wishlist[i];
		query << "INSERT INTO character_wishlist VALUES ("
				<< m_player->getId() << ","
				<< serial << ")";
		query.exec();
	}
}

void PlayerInventory::addMaxSlots(int8_t inventory, int8_t rows) { // Useful with .lua
	inventory -= 1;
	m_maxslots[inventory] += (rows * 4);
	if (m_maxslots[inventory] > 100)
		m_maxslots[inventory] = 100;
	if (m_maxslots[inventory] < 24) // Retard.
		m_maxslots[inventory] = 24;
}

void PlayerInventory::setMesos(int32_t mesos, bool is) {
	if (mesos < 0)
		mesos = 0;
	m_mesos = mesos;
}

bool PlayerInventory::modifyMesos(int32_t mod, bool is) {
	if (mod < 0) {
		if (-mod > m_mesos) {
			return false;
		}
		m_mesos += mod;
	}
	else {
		int32_t mesotest = m_mesos + mod;
		if (mesotest < 0) {
			return false;
		}
		m_mesos = mesotest;
	}
	return true;
}

void PlayerInventory::addItem(int8_t inv, int16_t slot, Item *item, bool isLoading) {
	m_items[inv - 1][slot] = item;
	if (m_itemamounts.find(item->getId()) != m_itemamounts.end())
		m_itemamounts[item->getId()] += item->getAmount();
	else
		m_itemamounts[item->getId()] = item->getAmount();
	if (slot < 0) {
		addEquipped(slot, item->getId());
		m_player->getStats()->setEquip(slot, item, isLoading);
	}
}

Item * PlayerInventory::getItem(int8_t inv, int16_t slot) {
	if (!GameLogicUtilities::isValidInventory(inv))
		return nullptr;
	inv -= 1;
	if (m_items[inv].find(slot) != m_items[inv].end())
		return m_items[inv][slot];
	return nullptr;
}

Item * PlayerInventory::getItem(int8_t inv, int64_t cashid) {
	if (!GameLogicUtilities::isValidInventory(inv)) {
		return nullptr;
	}
	inv -= 1;
	for (int16_t i = 0; i < getMaxSlots(inv + 1); i++) {
		if (m_items[inv].find(i) != m_items[inv].end() && m_items[inv][i]->getCashId() == cashid) {
			return m_items[inv][i];
		}
	}
	return nullptr;
}

void PlayerInventory::deleteItem(int8_t inv, int16_t slot, bool updateAmount) {
	inv -= 1;
	if (m_items[inv].find(slot) != m_items[inv].end()) {
		if (updateAmount)
			m_itemamounts[m_items[inv][slot]->getId()] -= m_items[inv][slot]->getAmount();
		if (slot < 0) {
			addEquipped(slot, 0);
			m_player->getStats()->setEquip(slot, nullptr);
		}
		delete m_items[inv][slot];
		m_items[inv].erase(slot);
	}
}

void PlayerInventory::setItem(int8_t inv, int16_t slot, Item *item) {
	inv -= 1;
	if (item == nullptr) {
		m_items[inv].erase(slot);
		if (slot < 0) {
			addEquipped(slot, 0);
			m_player->getStats()->setEquip(slot, nullptr);
		}
	}
	else {
		m_items[inv][slot] = item;
		if (slot < 0) {
			addEquipped(slot, item->getId());
			m_player->getStats()->setEquip(slot, item);
		}
	}
}

int16_t PlayerInventory::getItemAmountBySlot(int8_t inv, int16_t slot) {
	inv -= 1;
	return m_items[inv].find(slot) != m_items[inv].end() ? m_items[inv][slot]->getAmount() : 0;
}

int16_t PlayerInventory::getSlotByCashId(int8_t inv, int64_t cashid) {
	if (!GameLogicUtilities::isValidInventory(inv)) {
		return -1;
	}
	inv -= 1;
	for (int16_t i = 0; i < getMaxSlots(inv + 1); i++) {
		if (m_items[inv].find(i) != m_items[inv].end() && m_items[inv][i]->getCashId() == cashid) {
			return i;
		}
	}
	return -1;
}

void PlayerInventory::addEquipped(int16_t slot, int32_t itemid) {
	slot = abs(slot);
	if (slot == EquipSlots::Mount)
		m_player->getMounts()->setCurrentMount(itemid);

	if (slot > 100) // Cash items
		m_equipped[slot - 100][1] = itemid;
	else // Normal items
		m_equipped[slot][0] = itemid;
}

int32_t PlayerInventory::getEquippedId(int16_t slot) {
	return m_equipped[slot][0];
}

void PlayerInventory::addEquippedPacket(PacketCreator &packet) {
	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) { // Shown items
		if (m_equipped[i][0] > 0 || m_equipped[i][1] > 0) {
			packet.add<int8_t>(i);
			if (m_equipped[i][1] <= 0 || (i == 11 && m_equipped[i][0] > 0)) // Normal weapons always here
				packet.add<int32_t>(m_equipped[i][0]);
			else
				packet.add<int32_t>(m_equipped[i][1]);
		}
	}
	packet.add<int8_t>(-1);
	for (int8_t i = 0; i < Inventories::EquippedSlots; i++) { // Covered items
		if (m_equipped[i][1] > 0 && m_equipped[i][0] > 0 && i != 11) {
			packet.add<int8_t>(i);
			packet.add<int32_t>(m_equipped[i][0]);
		}
	}
	packet.add<int8_t>(-1);
	packet.add<int32_t>(m_equipped[11][1]); // Cash weapon
}

uint16_t PlayerInventory::getItemAmount(int32_t itemid) {
	return m_itemamounts.find(itemid) != m_itemamounts.end() ? m_itemamounts[itemid] : 0;
}

bool PlayerInventory::isEquippedItem(int32_t itemid) {
	ItemInventory &equips = m_items[Inventories::EquipInventory - 1];
	bool has = false;
	for (ItemInventory::iterator iter = equips.begin(); iter != equips.end(); iter++) {
		if (iter->first == itemid) {
			has = true;
			break;
		}
	}
	return has;
}

bool PlayerInventory::hasOpenSlotsFor(int32_t itemid, int16_t amount, bool canStack) {
	int16_t required = 0;
	int8_t inv = GameLogicUtilities::getInventory(itemid);
	if (inv == Inventories::EquipInventory || GameLogicUtilities::isRechargeable(itemid))
		required = amount; // These aren't stackable
	else {
		int16_t maxslot = ItemDataProvider::Instance()->getMaxSlot(itemid);
		uint16_t existing = getItemAmount(itemid) % maxslot;
		// Bug in global:
		// It doesn't matter if you already have a slot with a partial stack or not, non-shops require at least 1 empty slot
		if (canStack && existing > 0) { // If not, calculate how many slots necessary
			existing += amount;
			if (existing > maxslot) { // Only have to bother with required slots if it would put us over the limit of a slot
				required = (int16_t) (existing / maxslot);
				if ((existing % maxslot) > 0)
					required += 1;
			}
		}
		else { // If it is, treat it as though no items exist at all
			required = (int16_t) (amount / maxslot);
			if ((amount % maxslot) > 0)
				required += 1;
		}
	}
	return getOpenSlotsNum(inv) >= required;
}

int16_t PlayerInventory::getOpenSlotsNum(int8_t inv) {
	int16_t openslots = 0;
	for (int16_t i = 1; i <= getMaxSlots(inv); i++) {
		if (getItem(inv, i) == nullptr) {
			openslots++;
		}
	}
	return openslots;
}

int32_t PlayerInventory::doShadowStars() {
	for (int16_t s = 1; s <= getMaxSlots(Inventories::UseInventory); s++) {
		Item *item = getItem(Inventories::UseInventory, s);
		if (item == nullptr)
			continue;
		if (GameLogicUtilities::isStar(item->getId()) && item->getAmount() >= 200) {
			Inventory::takeItemSlot(m_player, Inventories::UseInventory, s, 200);
			return item->getId();
		}
	}
	return 0;
}

void PlayerInventory::addWishListItem(int32_t itemid) {
	m_wishlist.push_back(itemid);
}

void PlayerInventory::clearWishList() {
	m_wishlist.clear();
}

void PlayerInventory::connectData(PacketCreator &packet) {
	packet.add<int32_t>(m_mesos);

	for (uint8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; i++)
		packet.add<int8_t>(getMaxSlots(i));

	// Go through equips
	ItemInventory &equips = m_items[Inventories::EquipInventory - 1];
	ItemInventory::iterator iter;
	for (iter = equips.begin(); iter != equips.end(); ++iter) {
		if (iter->first < 0 && iter->first > -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.add<int8_t>(0);
	for (iter = equips.begin(); iter != equips.end(); ++iter) {
		if (iter->first < -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.add<int8_t>(0);
	for (iter = equips.begin(); iter != equips.end(); ++iter) {
		if (iter->first > 0) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.add<int8_t>(0);

	// Equips done, do rest of user's items starting with Use
	for (int8_t i = Inventories::UseInventory; i <= Inventories::InventoryCount; i++) {
		for (int16_t s = 1; s <= getMaxSlots(i); s++) {
			Item *item = getItem(i, s);
			if (item == nullptr)
				continue;
			if (item->getPetId() == 0) {
				PlayerPacketHelper::addItemInfo(packet, s, item);
			}
			else {
				Pet *pet = m_player->getPets()->getPet(item->getPetId());
				packet.add<int8_t>((int8_t) s);
				PetsPacket::addInfo(packet, pet);
			}
		}
		packet.add<int8_t>(0);
	}
}

void PlayerInventory::rockPacket(PacketCreator &packet) {
	InventoryPacketHelper::fillRockPacket(packet, m_rocklocations, Inventories::TeleportRockMax);
	InventoryPacketHelper::fillRockPacket(packet, m_viplocations, Inventories::VipRockMax);
}

void PlayerInventory::wishListPacket(PacketCreator &packet) {
	size_t remaining = 10 - m_wishlist.size();
	for (size_t i = 0; i < m_wishlist.size(); i++) {
		packet.add<int32_t>(m_wishlist[i]);
	}

	for (size_t i = 0; i < remaining; i++) {
		packet.add<int32_t>(0);
	}
}

void PlayerInventory::checkExpiredItems() {
	for (int8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; i++) {
		for (int16_t s = 1; s <= getMaxSlots(i); s++) {
			Item *item = getItem(i, s);

			if (item != nullptr && item->getExpirationTime() != Items::NoExpiration && item->getExpirationTime() <= TimeUtilities::getServerTime()) {
				PlayerPacket::sendItemExpired(m_player, item->getCashId());
				Inventory::takeItemSlot(m_player, i, s, item->getAmount());
			}
		}
	}
}
