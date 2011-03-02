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
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerPacketHelper.h"

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
	query << "SELECT inv, slot, itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, flags, hammers, petid, items.name, pets.index, pets.name, pets.level, pets.closeness, pets.fullness FROM items LEFT JOIN pets ON items.petid = pets.id WHERE charid = " << m_player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	Item *item;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		item = new Item;
		item->id = res[i][2];
		item->amount = res[i][3];
		item->slots = (int8_t) res[i][4];
		item->scrolls = (int8_t) res[i][5];
		item->istr = res[i][6];
		item->idex = res[i][7];
		item->iint = res[i][8];
		item->iluk = res[i][9];
		item->ihp = res[i][10];
		item->imp = res[i][11];
		item->iwatk = res[i][12];
		item->imatk = res[i][13];
		item->iwdef = res[i][14];
		item->imdef = res[i][15];
		item->iacc = res[i][16];
		item->iavo = res[i][17];
		item->ihand = res[i][18];
		item->ispeed = res[i][19];
		item->ijump = res[i][20];
		item->flags = (int8_t) res[i][21];
		item->hammers = res[i][22];
		item->petid = res[i][23];
		res[i][24].to_string(item->name);
		addItem((int8_t) res[i][0], res[i][1], item, true);
		if (item->petid != 0) {
			Pet *pet = new Pet(
				m_player,
				item, // Item - Gives id and type to pet
				(int8_t) res[i][25], // Index
				(string) res[i][26], // Name
				(uint8_t) res[i][27], // Level
				(int16_t) res[i][28], // Closeness
				(uint8_t) res[i][29], // Fullness
				(uint8_t) res[i][1] // Inventory Slot
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
				query << ",(";
			}
			query << m_player->getId() << ","
				<< (int16_t) i << ","
				<< iter->first << ","
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
				<< item->petid << ","
				<< mysqlpp::quote << item->name << ")";
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
}

void PlayerInventory::addMaxSlots(int8_t inventory, int8_t rows) { // Useful with .lua
	inventory -= 1;
	m_maxslots[inventory] += (rows * 4);
	if (m_maxslots[inventory] > 100)
		m_maxslots[inventory] = 100;
	if (m_maxslots[inventory] < 24) // Retard.
		m_maxslots[inventory] = 24;
	InventoryPacket::updateSlots(m_player, inventory + 1, m_maxslots[inventory]);
}

void PlayerInventory::setMesos(int32_t mesos, bool is) {
	if (mesos < 0)
		mesos = 0;
	m_mesos = mesos;
	PlayerPacket::updateStatInt(m_player, Stats::Mesos, m_mesos, is);
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
	PlayerPacket::updateStatInt(m_player, Stats::Mesos, m_mesos, is);
	return true;
}

void PlayerInventory::addItem(int8_t inv, int16_t slot, Item *item, bool isLoading) {
	m_items[inv - 1][slot] = item;
	if (m_itemamounts.find(item->id) != m_itemamounts.end())
		m_itemamounts[item->id] += item->amount;
	else
		m_itemamounts[item->id] = item->amount;
	if (slot < 0) {
		addEquipped(slot, item->id);
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

void PlayerInventory::deleteItem(int8_t inv, int16_t slot, bool updateAmount) {
	inv -= 1;
	if (m_items[inv].find(slot) != m_items[inv].end()) {
		if (updateAmount)
			m_itemamounts[m_items[inv][slot]->id] -= m_items[inv][slot]->amount;
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
			addEquipped(slot, item->id);
			m_player->getStats()->setEquip(slot, item);
		}
	}
}

int16_t PlayerInventory::getItemAmountBySlot(int8_t inv, int16_t slot) {
	inv -= 1;
	return m_items[inv].find(slot) != m_items[inv].end() ? m_items[inv][slot]->amount : 0;
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
			if (m_equipped[i][1] <= 0 || (i == EquipSlots::Weapon && m_equipped[i][0] > 0)) // Normal weapons always here
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
	packet.add<int32_t>(m_equipped[EquipSlots::Weapon][1]); // Cash weapon
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
		if (getItem(inv, i) == nullptr)
			openslots++;
	}
	return openslots;
}

int32_t PlayerInventory::doShadowStars() {
	for (int16_t s = 1; s <= getMaxSlots(Inventories::UseInventory); s++) {
		Item *item = getItem(Inventories::UseInventory, s);
		if (item == nullptr)
			continue;
		if (GameLogicUtilities::isStar(item->id) && item->amount >= 200) {
			Inventory::takeItemSlot(m_player, Inventories::UseInventory, s, 200);
			return item->id;
		}
	}
	return 0;
}
void PlayerInventory::addRockMap(int32_t mapid, int8_t type) {
	const int8_t mode = 03;
	if (type == 0) { // Regular Teleport Rock
		if (m_rocklocations.size() < Inventories::TeleportRockMax) {
			m_rocklocations.push_back(mapid);
		}
		InventoryPacket::sendRockUpdate(m_player, mode, type, m_rocklocations);
	}
	else if (type == 1) { // VIP Teleport Rock
		if (m_viplocations.size() < Inventories::VipRockMax) {
			m_viplocations.push_back(mapid);
			// Want packet
		}
		InventoryPacket::sendRockUpdate(m_player, mode, type, m_viplocations);
	}
}

void PlayerInventory::delRockMap(int32_t mapid, int8_t type) {
	const int8_t mode = 02;
	if (type == 0) {
		for (size_t k = 0; k < m_rocklocations.size(); k++) {
			if (m_rocklocations[k] == mapid) {
				m_rocklocations.erase(m_rocklocations.begin() + k);
				InventoryPacket::sendRockUpdate(m_player, mode, type, m_rocklocations);
				break;
			}
		}
	}
	else if (type == 1) {
		for (size_t k = 0; k < m_viplocations.size(); k++) {
			if (m_viplocations[k] == mapid) {
				m_viplocations.erase(m_viplocations.begin() + k);
				InventoryPacket::sendRockUpdate(m_player, mode, type, m_viplocations);
				break;
			}
		}
	}
}

bool PlayerInventory::ensureRockDestination(int32_t mapid) {
	bool valid = false;
	for (size_t k = 0; k < m_rocklocations.size(); k++) {
		if (m_rocklocations[k] == mapid) {
			valid = true;
			break;
		}
	}
	for (size_t k = 0; k < m_viplocations.size(); k++) {
		if (m_viplocations[k] == mapid) {
			valid = true;
			break;
		}
	}
	return valid;
}

void PlayerInventory::addWishListItem(int32_t itemid) {
	m_wishlist.push_back(itemid);
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
			if (item->petid == 0) {
				PlayerPacketHelper::addItemInfo(packet, s, item);
			}
			else {
				Pet *pet = m_player->getPets()->getPet(item->petid);
				packet.add<int8_t>((int8_t) s);
				PetsPacket::addInfo(packet, pet);
			}
		}
		packet.add<int8_t>(0);
	}
}

void PlayerInventory::rockPacket(PacketCreator &packet) {
	size_t remaining;
	for (remaining = 1; remaining <= m_rocklocations.size(); remaining++) {
		int32_t mapid = m_rocklocations[remaining - 1];
		packet.add<int32_t>(mapid);
	}
	for (; remaining <= Inventories::TeleportRockMax; remaining++) {
		packet.add<int32_t>(Maps::NoMap);
	}
	for (remaining = 1; remaining <= m_viplocations.size(); remaining++) {
		int32_t mapid = m_viplocations[remaining - 1];
		packet.add<int32_t>(mapid);
	}
	for (; remaining <= Inventories::VipRockMax; remaining++) {
		packet.add<int32_t>(Maps::NoMap);
	}
}

void PlayerInventory::wishListPacket(PacketCreator &packet) {
	packet.add<uint8_t>(m_wishlist.size());
	for (size_t i = 0; i < m_wishlist.size(); i++) {
		packet.add<int32_t>(m_wishlist[i]);
	}
}