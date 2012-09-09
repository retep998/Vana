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
#include "PlayerInventory.h"
#include "Database.h"
#include "EquipDataProvider.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "InventoryPacketHelper.h"
#include "ItemConstants.h"
#include "ItemDataProvider.h"
#include "Map.h"
#include "MiscUtilities.h"
#include "PacketCreator.h"
#include "Pet.h"
#include "PetsPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerPacketHelper.h"
#include "TimeUtilities.h"

PlayerInventory::PlayerInventory(Player *player, const std::array<uint8_t, Inventories::InventoryCount> &maxSlots, int32_t mesos) :
	m_maxSlots(maxSlots),
	m_mesos(mesos),
	m_hammer(-1),
	m_player(player)
{
	std::array<int32_t, 2> g = {0};

	for (size_t i = 0; i < Inventories::EquippedSlots; ++i) {
		m_equipped[i] = g;
	}

	load();
}

PlayerInventory::~PlayerInventory() {
	typedef std::array<ItemInventory, Inventories::InventoryCount> ItemInvArr;
	for (ItemInvArr::iterator iter = m_items.begin(); iter != m_items.end(); ++iter) {
		std::for_each(iter->begin(), iter->end(), MiscUtilities::DeleterPairAssoc<ItemInventory::value_type>());
	}
}

void PlayerInventory::load() {
	using namespace soci;
	session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	string location = "inventory";

	soci::rowset<> rs = (sql.prepare
		<< "SELECT i.*, p.index, p.name AS pet_name, p.level, p.closeness, p.fullness "
		<< "FROM items i "
		<< "LEFT OUTER JOIN pets p ON i.pet_id = p.pet_id "
		<< "WHERE i.location = :location AND i.character_id = :char",
		soci::use(m_player->getId(), "char"),
		soci::use(location, "location"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		Item *item = new Item(row);
		addItem(row.get<int8_t>("inv"), row.get<int16_t>("slot"), item, true);

		if (item->getPetId() != 0) {
			Pet *pet = new Pet(m_player, item, row);
			m_player->getPets()->addPet(pet);
		}
	}

	rs = (sql.prepare << "SELECT t.map_index, t.map_id FROM teleport_rock_locations t WHERE t.character_id = :char", soci::use(m_player->getId(), "char"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		int8_t index = row.get<int8_t>("map_index");
		int32_t mapId = row.get<int32_t>("map_id");

		if (index >= Inventories::TeleportRockMax) {
			m_vipLocations.push_back(mapId);
		}
		else {
			m_rockLocations.push_back(mapId);
		}

	}
}

void PlayerInventory::save() {
	using namespace soci;
	session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();

	sql.once << "DELETE FROM teleport_rock_locations WHERE character_id = :char", use(charId, "char");
	if (m_rockLocations.size() > 0 || m_vipLocations.size() > 0) {
		int32_t mapId = 0;
		size_t i = 0;

		statement st = (sql.prepare
			<< "INSERT INTO teleport_rock_locations "
			<< "VALUES (:char, :i, :map)",
			use(charId, "char"),
			use(mapId, "map"),
			use(i, "i"));

		for (i = 0; i < m_rockLocations.size(); ++i) {
			mapId = m_rockLocations[i];
			st.execute(true);
		}

		i = Inventories::TeleportRockMax;
		for (size_t j = 0; j < m_vipLocations.size(); ++j) {
			mapId = m_vipLocations[i];
			st.execute(true);
			++i;
		}
	}

	sql.once
		<< "DELETE FROM items "
		<< "WHERE location = :inv AND character_id = :char",
		use(charId, "char"),
		use(Item::Inventory, "inv");

	vector<ItemDbRecord> v;
	for (int8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		ItemInventory &itemsInv = m_items[i - 1];
		for (ItemInventory::iterator iter = itemsInv.begin(); iter != itemsInv.end(); ++iter) {
			ItemDbRecord rec(iter->first, charId, m_player->getUserId(), m_player->getWorldId(), Item::Inventory, iter->second);
			v.push_back(rec);
		}
	}

	Item::databaseInsert(sql, v);
}

void PlayerInventory::addMaxSlots(int8_t inventory, int8_t rows) {
	inventory -= 1;

	uint8_t &inv = m_maxSlots[inventory];
	inv += (rows * 4);

	inv = MiscUtilities::constrainToRange(inv, Inventories::MinSlotsPerInventory, Inventories::MaxSlotsPerInventory);
	InventoryPacket::updateSlots(m_player, inventory + 1, inv);
}

void PlayerInventory::setMesos(int32_t mesos, bool sendPacket) {
	if (mesos < 0) {
		mesos = 0;
	}
	m_mesos = mesos;
	PlayerPacket::updateStat(m_player, Stats::Mesos, m_mesos, sendPacket);
}

bool PlayerInventory::modifyMesos(int32_t mod, bool sendPacket) {
	if (mod < 0) {
		if (-mod > m_mesos) {
			return false;
		}
		m_mesos += mod;
	}
	else {
		int32_t mesoTest = m_mesos + mod;
		if (mesoTest < 0) {
			return false;
		}
		m_mesos = mesoTest;
	}
	PlayerPacket::updateStat(m_player, Stats::Mesos, m_mesos, sendPacket);
	return true;
}

void PlayerInventory::addItem(int8_t inv, int16_t slot, Item *item, bool isLoading) {
	m_items[inv - 1][slot] = item;
	int32_t itemId = item->getId();
	if (m_itemAmounts.find(itemId) != m_itemAmounts.end()) {
		m_itemAmounts[itemId] += item->getAmount();
	}
	else {
		m_itemAmounts[itemId] = item->getAmount();
	}
	if (slot < 0) {
		addEquipped(slot, itemId);
		m_player->getStats()->setEquip(slot, item, isLoading);
	}
}

Item * PlayerInventory::getItem(int8_t inv, int16_t slot) {
	if (!GameLogicUtilities::isValidInventory(inv)) {
		return nullptr;
	}
	inv -= 1;
	if (m_items[inv].find(slot) != m_items[inv].end()) {
		return m_items[inv][slot];
	}
	return nullptr;
}

void PlayerInventory::deleteItem(int8_t inv, int16_t slot, bool updateAmount) {
	inv -= 1;
	if (m_items[inv].find(slot) != m_items[inv].end()) {
		if (updateAmount) {
			Item *x = m_items[inv][slot];
			m_itemAmounts[x->getId()] -= x->getAmount();
		}
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
			m_player->getMap()->checkPlayerEquip(m_player);
		}
	}
	else {
		m_items[inv][slot] = item;
		if (slot < 0) {
			addEquipped(slot, item->getId());
			m_player->getStats()->setEquip(slot, item);
			m_player->getMap()->checkPlayerEquip(m_player);
		}
	}
}

void PlayerInventory::destroyEquippedItem(int32_t itemId) {
	int8_t inv = Inventories::EquipInventory;
	const ItemInventory &equips = m_items[inv - 1];
	for (auto iter = equips.begin(); iter != equips.end(); ++iter) {
		if (iter->first < 0 && iter->second->getId() == itemId) {
			InventoryPacket::moveItem(m_player, inv, iter->first, 0);
			deleteItem(inv, iter->first, false);
			break;
		}
	}
}

int16_t PlayerInventory::getItemAmountBySlot(int8_t inv, int16_t slot) {
	inv -= 1;
	return (m_items[inv].find(slot) != m_items[inv].end() ? m_items[inv][slot]->getAmount() : 0);
}

void PlayerInventory::addEquipped(int16_t slot, int32_t itemId) {
	if (abs(slot) == EquipSlots::Mount) {
		m_player->getMounts()->setCurrentMount(itemId);
	}

	int8_t cash = GameLogicUtilities::isCashSlot(slot) ? 1 : 0;
	m_equipped[GameLogicUtilities::stripCashSlot(slot)][cash] = itemId;
}

int32_t PlayerInventory::getEquippedId(int16_t slot, bool cash) {
	return m_equipped[slot][(cash ? 1 : 0)];
}

void PlayerInventory::addEquippedPacket(PacketCreator &packet) {
	for (int8_t i = 0; i < Inventories::EquippedSlots; ++i) {
		// Shown items
		if (m_equipped[i][0] > 0 || m_equipped[i][1] > 0) {
			packet.add<int8_t>(i);
			if (m_equipped[i][1] <= 0 || (i == EquipSlots::Weapon && m_equipped[i][0] > 0)) {
				// Normal weapons always here
				packet.add<int32_t>(m_equipped[i][0]);
			}
			else {
				packet.add<int32_t>(m_equipped[i][1]);
			}
		}
	}
	packet.add<int8_t>(-1);
	for (int8_t i = 0; i < Inventories::EquippedSlots; ++i) {
		// Covered items
		if (m_equipped[i][1] > 0 && m_equipped[i][0] > 0 && i != EquipSlots::Weapon) {
			packet.add<int8_t>(i);
			packet.add<int32_t>(m_equipped[i][0]);
		}
	}
	packet.add<int8_t>(-1);
	packet.add<int32_t>(m_equipped[EquipSlots::Weapon][1]); // Cash weapon
}

uint16_t PlayerInventory::getItemAmount(int32_t itemId) {
	return m_itemAmounts.find(itemId) != m_itemAmounts.end() ? m_itemAmounts[itemId] : 0;
}

bool PlayerInventory::isEquippedItem(int32_t itemId) {
	const ItemInventory &equips = m_items[Inventories::EquipInventory - 1];
	bool has = false;
	for (auto iter = equips.begin(); iter != equips.end(); ++iter) {
		if (iter->first < 0 && iter->second->getId() == itemId) {
			has = true;
			break;
		}
	}
	return has;
}

bool PlayerInventory::hasOpenSlotsFor(int32_t itemId, int16_t amount, bool canStack) {
	int16_t required = 0;
	int8_t inv = GameLogicUtilities::getInventory(itemId);
	if (!GameLogicUtilities::isStackable(itemId)) {
		required = amount; // These aren't stackable
	}
	else {
		int16_t maxSlot = ItemDataProvider::Instance()->getMaxSlot(itemId);
		uint16_t existing = getItemAmount(itemId) % maxSlot;
		// Bug in global:
		// It doesn't matter if you already have a slot with a partial stack or not, non-shops require at least 1 empty slot
		if (canStack && existing > 0) {
			// If not, calculate how many slots necessary
			existing += amount;
			if (existing > maxSlot) {
				// Only have to bother with required slots if it would put us over the limit of a slot
				required = static_cast<int16_t>(existing / maxSlot);
				if ((existing % maxSlot) > 0) {
					++required;
				}
			}
		}
		else {
			// If it is, treat it as though no items exist at all
			required = static_cast<int16_t>(amount / maxSlot);
			if ((amount % maxSlot) > 0) {
				++required;
			}
		}
	}
	return getOpenSlotsNum(inv) >= required;
}

int16_t PlayerInventory::getOpenSlotsNum(int8_t inv) {
	int16_t openSlots = 0;
	for (int16_t i = 1; i <= getMaxSlots(inv); ++i) {
		if (getItem(inv, i) == nullptr) {
			++openSlots;
		}
	}
	return openSlots;
}

int32_t PlayerInventory::doShadowStars() {
	for (int16_t s = 1; s <= getMaxSlots(Inventories::UseInventory); ++s) {
		Item *item = getItem(Inventories::UseInventory, s);
		if (item == nullptr) {
			continue;
		}
		if (GameLogicUtilities::isStar(item->getId()) && item->getAmount() >= Items::ShadowStarsCost) {
			Inventory::takeItemSlot(m_player, Inventories::UseInventory, s, Items::ShadowStarsCost);
			return item->getId();
		}
	}
	return 0;
}
void PlayerInventory::addRockMap(int32_t mapId, int8_t type) {
	const int8_t mode = InventoryPacket::RockModes::Add;
	if (type == InventoryPacket::RockTypes::Regular) {
		if (m_rockLocations.size() < Inventories::TeleportRockMax) {
			m_rockLocations.push_back(mapId);
		}
		InventoryPacket::sendRockUpdate(m_player, mode, type, m_rockLocations);
	}
	else if (type == InventoryPacket::RockTypes::Vip) {
		if (m_vipLocations.size() < Inventories::VipRockMax) {
			m_vipLocations.push_back(mapId);
			// Want packet
		}
		InventoryPacket::sendRockUpdate(m_player, mode, type, m_vipLocations);
	}
}

void PlayerInventory::delRockMap(int32_t mapId, int8_t type) {
	const int8_t mode = InventoryPacket::RockModes::Delete;
	if (type == InventoryPacket::RockTypes::Regular) {
		for (size_t k = 0; k < m_rockLocations.size(); ++k) {
			if (m_rockLocations[k] == mapId) {
				m_rockLocations.erase(m_rockLocations.begin() + k);
				InventoryPacket::sendRockUpdate(m_player, mode, type, m_rockLocations);
				break;
			}
		}
	}
	else if (type == InventoryPacket::RockTypes::Vip) {
		for (size_t k = 0; k < m_vipLocations.size(); ++k) {
			if (m_vipLocations[k] == mapId) {
				m_vipLocations.erase(m_vipLocations.begin() + k);
				InventoryPacket::sendRockUpdate(m_player, mode, type, m_vipLocations);
				break;
			}
		}
	}
}

void PlayerInventory::swapItems(int8_t inventory, int16_t slot1, int16_t slot2) {
	bool equippedSlot2 = (slot2 < 0);
	if (inventory == Inventories::EquipInventory && equippedSlot2) {
		// Handle these specially
		Item *item1 = getItem(inventory, slot1);
		if (item1 == nullptr) {
			// Hacking
			return;
		}

		int32_t itemId1 = item1->getId();
		int16_t strippedSlot1 = GameLogicUtilities::stripCashSlot(slot1);
		int16_t strippedSlot2 = GameLogicUtilities::stripCashSlot(slot2);
		if (!EquipDataProvider::Instance()->validSlot(itemId1, strippedSlot2)) {
			// Hacking
			return;
		}

		Item *remove = nullptr;
		int16_t oldSlot = 0;
		bool weapon = (strippedSlot2 == EquipSlots::Weapon);
		bool shield = (strippedSlot2 == EquipSlots::Shield);
		bool top = (strippedSlot2 == EquipSlots::Top);
		bool bottom = (strippedSlot2 == EquipSlots::Bottom);

		if (weapon && GameLogicUtilities::is2hWeapon(itemId1) && getEquippedId(EquipSlots::Shield) != 0) {
			oldSlot = -EquipSlots::Shield;
		}
		else if (shield && GameLogicUtilities::is2hWeapon(getEquippedId(EquipSlots::Weapon))) {
			oldSlot = -EquipSlots::Weapon;
		}
		else if (top && GameLogicUtilities::isOverall(itemId1) && getEquippedId(EquipSlots::Bottom) != 0) {
			oldSlot = -EquipSlots::Bottom;
		}
		else if (bottom && GameLogicUtilities::isOverall(getEquippedId(EquipSlots::Top))) {
			oldSlot = -EquipSlots::Top;
		}
		if (oldSlot != 0) {
			remove = getItem(inventory, oldSlot);
			bool onlySwap = true;
			if ((getEquippedId(EquipSlots::Shield) != 0) && (getEquippedId(EquipSlots::Weapon) != 0)) {
				onlySwap = false;
			}
			else if ((getEquippedId(EquipSlots::Top) != 0) && (getEquippedId(EquipSlots::Bottom) != 0)) {
				onlySwap = false;
			}
			if (onlySwap) {
				int16_t swapSlot = 0;
				if (weapon) {
					swapSlot = -EquipSlots::Shield;
					m_player->getActiveBuffs()->swapWeapon();
				}
				else if (shield) {
					swapSlot = -EquipSlots::Weapon;
					m_player->getActiveBuffs()->swapWeapon();
				}
				else if (top) {
					swapSlot = -EquipSlots::Bottom;
				}
				else if (bottom) {
					swapSlot = -EquipSlots::Top;
				}
				setItem(inventory, swapSlot, nullptr);
				setItem(inventory, slot1, remove);
				setItem(inventory, slot2, item1);
				InventoryPacket::moveItem(m_player, inventory, slot1, slot2);
				InventoryPacket::moveItem(m_player, inventory, swapSlot, slot1);
				InventoryPacket::updatePlayer(m_player);
				return;
			}
			else {
				if (getOpenSlotsNum(inventory) == 0) {
					InventoryPacket::blankUpdate(m_player);
					return;
				}
				int16_t freeSlot = 0;
				for (int16_t s = 1; s <= getMaxSlots(inventory); s++) {
					Item *oldItem = getItem(inventory, s);
					if (oldItem == nullptr) {
						freeSlot = s;
						break;
					}
				}
				setItem(inventory, freeSlot, remove);
				setItem(inventory, oldSlot, nullptr);
				InventoryPacket::moveItem(m_player, inventory, oldSlot, freeSlot);
			}
		}
		// Nothing special happening, just a simple equip swap
		Item *item2 = getItem(inventory, slot2);
		setItem(inventory, slot1, item2);
		setItem(inventory, slot2, item1);
		InventoryPacket::moveItem(m_player, inventory, slot1, slot2);
	}
	else {
		// The only interesting things that can happen here are stack modifications and slot swapping
		Item *item1 = getItem(inventory, slot1);
		Item *item2 = getItem(inventory, slot2);
		
		if (item1 == nullptr) {
			// If item2 is nullptr, it's moving item1 into slot2
			// Hacking
			return;
		}

		int32_t itemId1 = item1->getId();
		int32_t itemId2 = (item2 == nullptr ? 0 : item2->getId());
		if (item2 != nullptr && itemId1 == itemId2 && GameLogicUtilities::isStackable(itemId1)) {
			int32_t maxSlot = ItemDataProvider::Instance()->getMaxSlot(itemId1);
			if (item1->getAmount() + item2->getAmount() <= maxSlot) {
				item2->incAmount(item1->getAmount());
				deleteItem(inventory, slot1, false);
				InventoryPacket::updateItemAmounts(m_player, inventory, slot2, item2->getAmount(), 0, 0);
				InventoryPacket::moveItem(m_player, inventory, slot1, 0);
			}
			else {
				item1->decAmount(maxSlot - item2->getAmount());
				item2->setAmount(maxSlot);
				InventoryPacket::updateItemAmounts(m_player, inventory, slot1, item1->getAmount(), slot2, item2->getAmount());
			}
		}
		else {
			// The item is not stackable, not the same item, or a blank slot swap is occurring, either way it's a plain swap
			setItem(inventory, slot1, item2);
			setItem(inventory, slot2, item1);
			if (item1->getPetId() > 0) {
				m_player->getPets()->getPet(item1->getPetId())->setInventorySlot((int8_t) slot2);
			}
			if (item2 != nullptr && item2->getPetId() > 0) {
				m_player->getPets()->getPet(item2->getPetId())->setInventorySlot((int8_t) slot1);
			}
			InventoryPacket::moveItem(m_player, inventory, slot1, slot2);
		}
	}
}

bool PlayerInventory::ensureRockDestination(int32_t mapId) {
	for (size_t k = 0; k < m_rockLocations.size(); ++k) {
		if (m_rockLocations[k] == mapId) {
			return true;
		}
	}
	for (size_t k = 0; k < m_vipLocations.size(); ++k) {
		if (m_vipLocations[k] == mapId) {
			return true;
		}
	}
	return false;
}

void PlayerInventory::addWishListItem(int32_t itemId) {
	m_wishlist.push_back(itemId);
}

void PlayerInventory::connectData(PacketCreator &packet) {
	packet.add<int32_t>(m_mesos);

	for (uint8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		packet.add<int8_t>(getMaxSlots(i));
	}

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
	for (int8_t i = Inventories::UseInventory; i <= Inventories::InventoryCount; ++i) {
		for (int16_t s = 1; s <= getMaxSlots(i); ++s) {
			Item *item = getItem(i, s);
			if (item == nullptr) {
				continue;
			}
			if (item->getPetId() == 0) {
				PlayerPacketHelper::addItemInfo(packet, s, item);
			}
			else {
				Pet *pet = m_player->getPets()->getPet(item->getPetId());
				packet.add<int8_t>(static_cast<int8_t>(s));
				PetsPacket::addInfo(packet, pet, item);
			}
		}
		packet.add<int8_t>(0);
	}
}

void PlayerInventory::rockPacket(PacketCreator &packet) {
	InventoryPacketHelper::fillRockPacket(packet, m_rockLocations, Inventories::TeleportRockMax);
	InventoryPacketHelper::fillRockPacket(packet, m_vipLocations, Inventories::VipRockMax);
}

void PlayerInventory::wishListPacket(PacketCreator &packet) {
	packet.add<uint8_t>(m_wishlist.size());
	for (size_t i = 0; i < m_wishlist.size(); ++i) {
		packet.add<int32_t>(m_wishlist[i]);
	}
}

void PlayerInventory::checkExpiredItems() {
	vector<int32_t> expiredItemIds;
	int64_t serverTime = TimeUtilities::getServerTime();

	for (int8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		for (int16_t s = 1; s <= getMaxSlots(i); ++s) {
			if (Item *item = getItem(i, s)) {
				if (item->getExpirationTime() != Items::NoExpiration && item->getExpirationTime() <= serverTime) {
					expiredItemIds.push_back(item->getId());
					Inventory::takeItemSlot(m_player, i, s, item->getAmount());
				}
			}
		}
	}

	if (expiredItemIds.size() > 0) {
		InventoryPacket::sendItemExpired(m_player, expiredItemIds);
	}
}