/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "PlayerInventory.hpp"
#include "Algorithm.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "EquipDataProvider.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "InventoryPacket.hpp"
#include "InventoryPacketHelper.hpp"
#include "ItemConstants.hpp"
#include "ItemDataProvider.hpp"
#include "Map.hpp"
#include "MiscUtilities.hpp"
#include "Pet.hpp"
#include "PetsPacket.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "PlayerPacketHelper.hpp"
#include "TimeUtilities.hpp"

namespace Vana {

PlayerInventory::PlayerInventory(Player *player, const array_t<inventory_slot_count_t, Inventories::InventoryCount> &maxSlots, mesos_t mesos) :
	m_maxSlots{maxSlots},
	m_mesos{mesos},
	m_player{player}
{
	array_t<item_id_t, 2> init = {0};

	for (size_t i = 0; i < Inventories::EquippedSlots; ++i) {
		m_equipped[i] = init;
	}

	load();
}

PlayerInventory::~PlayerInventory() {
	/* TODO FIXME just convert the damn Item * to ref_ptr_t or owned_ptr_t */
	for (const auto &inv : m_items) {
		std::for_each(std::begin(inv), std::end(inv), [](pair_t<const inventory_slot_t, Item *> p) { delete p.second; });
	}
}

auto PlayerInventory::load() -> void {
	using namespace soci;
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();
	string_t location = "inventory";

	soci::rowset<> rs = (sql.prepare
		<< "SELECT i.*, p.index, p.name AS pet_name, p.level, p.closeness, p.fullness "
		<< "FROM " << db.makeTable("items") << " i "
		<< "LEFT OUTER JOIN " << db.makeTable("pets") << " p ON i.pet_id = p.pet_id "
		<< "WHERE i.location = :location AND i.character_id = :char",
		soci::use(m_player->getId(), "char"),
		soci::use(location, "location"));

	for (const auto &row : rs) {
		Item *item = new Item(row);
		addItem(row.get<inventory_t>("inv"), row.get<inventory_slot_t>("slot"), item, true);

		if (item->getPetId() != 0) {
			Pet *pet = new Pet(m_player, item, row);
			m_player->getPets()->addPet(pet);
		}
	}

	rs = (sql.prepare << "SELECT t.map_index, t.map_id FROM " << db.makeTable("teleport_rock_locations") << " t WHERE t.character_id = :char", soci::use(m_player->getId(), "char"));

	for (const auto &row : rs) {
		int8_t index = row.get<int8_t>("map_index");
		map_id_t mapId = row.get<map_id_t>("map_id");

		if (index >= Inventories::TeleportRockMax) {
			m_vipLocations.push_back(mapId);
		}
		else {
			m_rockLocations.push_back(mapId);
		}

	}
}

auto PlayerInventory::save() -> void {
	using namespace soci;
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();

	sql.once << "DELETE FROM " << db.makeTable("teleport_rock_locations") << " WHERE character_id = :char", use(charId, "char");
	if (m_rockLocations.size() > 0 || m_vipLocations.size() > 0) {
		map_id_t mapId = 0;
		size_t rockIndex = 0;

		statement st = (sql.prepare
			<< "INSERT INTO " << db.makeTable("teleport_rock_locations") << " "
			<< "VALUES (:char, :i, :map)",
			use(charId, "char"),
			use(mapId, "map"),
			use(rockIndex, "i"));

		for (rockIndex = 0; rockIndex < m_rockLocations.size(); ++rockIndex) {
			mapId = m_rockLocations[rockIndex];
			st.execute(true);
		}

		rockIndex = Inventories::TeleportRockMax;
		for (size_t i = 0; i < m_vipLocations.size(); ++i) {
			mapId = m_vipLocations[i];
			st.execute(true);
			++rockIndex;
		}
	}

	sql.once
		<< "DELETE FROM " << db.makeTable("items") << " "
		<< "WHERE location = :inv AND character_id = :char",
		use(charId, "char"),
		use(Item::Inventory, "inv");

	vector_t<ItemDbRecord> v;
	for (inventory_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		const auto &itemsInv = m_items[i - 1];
		for (const auto &kvp : itemsInv) {
			ItemDbRecord rec(kvp.first, charId, m_player->getAccountId(), m_player->getWorldId(), Item::Inventory, kvp.second);
			v.push_back(rec);
		}
	}

	Item::databaseInsert(db, v);
}

auto PlayerInventory::addMaxSlots(inventory_t inventory, inventory_slot_count_t rows) -> void {
	inventory -= 1;

	inventory_slot_count_t &inv = m_maxSlots[inventory];
	inv += (rows * 4);

	inv = ext::constrain_range(inv, Inventories::MinSlotsPerInventory, Inventories::MaxSlotsPerInventory);
	m_player->send(Packets::Inventory::updateSlots(inventory + 1, inv));
}

auto PlayerInventory::setMesos(mesos_t mesos, bool sendPacket) -> void {
	if (mesos < 0) {
		mesos = 0;
	}
	m_mesos = mesos;
	m_player->send(Packets::Player::updateStat(Stats::Mesos, m_mesos, sendPacket));
}

auto PlayerInventory::modifyMesos(mesos_t mod, bool sendPacket) -> bool {
	if (mod < 0) {
		if (-mod > m_mesos) {
			return false;
		}
		m_mesos += mod;
	}
	else {
		mesos_t mesoTest = m_mesos + mod;
		if (mesoTest < 0) {
			return false;
		}
		m_mesos = mesoTest;
	}
	m_player->send(Packets::Player::updateStat(Stats::Mesos, m_mesos, sendPacket));
	return true;
}

auto PlayerInventory::addItem(inventory_t inv, inventory_slot_t slot, Item *item, bool isLoading) -> void {
	m_items[inv - 1][slot] = item;
	item_id_t itemId = item->getId();
	if (m_itemAmounts.find(itemId) != std::end(m_itemAmounts)) {
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

auto PlayerInventory::getItem(inventory_t inv, inventory_slot_t slot) -> Item * {
	if (!GameLogicUtilities::isValidInventory(inv)) {
		return nullptr;
	}
	inv -= 1;
	if (m_items[inv].find(slot) != std::end(m_items[inv])) {
		return m_items[inv][slot];
	}
	return nullptr;
}

auto PlayerInventory::deleteItem(inventory_t inv, inventory_slot_t slot, bool updateAmount) -> void {
	inv -= 1;
	if (m_items[inv].find(slot) != std::end(m_items[inv])) {
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

auto PlayerInventory::setItem(inventory_t inv, inventory_slot_t slot, Item *item) -> void {
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

auto PlayerInventory::destroyEquippedItem(item_id_t itemId) -> void {
	inventory_t inv = Inventories::EquipInventory;
	const auto &equips = m_items[inv - 1];
	for (const auto &kvp : equips) {
		if (kvp.first < 0 && kvp.second->getId() == itemId) {
			vector_t<InventoryPacketOperation> ops;
			ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, kvp.second, kvp.first);
			m_player->send(Packets::Inventory::inventoryOperation(true, ops));

			deleteItem(inv, kvp.first, false);
			break;
		}
	}
}

auto PlayerInventory::getItemAmountBySlot(inventory_t inv, inventory_slot_t slot) -> slot_qty_t {
	inv -= 1;
	return (m_items[inv].find(slot) != std::end(m_items[inv]) ? m_items[inv][slot]->getAmount() : 0);
}

auto PlayerInventory::addEquipped(inventory_slot_t slot, item_id_t itemId) -> void {
	if (std::abs(slot) == EquipSlots::Mount) {
		m_player->getMounts()->setCurrentMount(itemId);
	}

	int8_t cash = GameLogicUtilities::isCashSlot(slot) ? 1 : 0;
	m_equipped[GameLogicUtilities::stripCashSlot(slot)][cash] = itemId;
}

auto PlayerInventory::getEquippedId(inventory_slot_t slot, bool cash) -> item_id_t {
	return m_equipped[slot][(cash ? 1 : 0)];
}

auto PlayerInventory::addEquippedPacket(PacketBuilder &packet) -> void {
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

auto PlayerInventory::getItemAmount(item_id_t itemId) -> slot_qty_t {
	return m_itemAmounts.find(itemId) != std::end(m_itemAmounts) ? m_itemAmounts[itemId] : 0;
}

auto PlayerInventory::isEquippedItem(item_id_t itemId) -> bool {
	const auto &equips = m_items[Inventories::EquipInventory - 1];
	bool has = false;
	for (const auto &kvp : equips) {
		if (kvp.first < 0 && kvp.second->getId() == itemId) {
			has = true;
			break;
		}
	}
	return has;
}

auto PlayerInventory::hasOpenSlotsFor(item_id_t itemId, slot_qty_t amount, bool canStack) -> bool {
	slot_qty_t required = 0;
	inventory_t inv = GameLogicUtilities::getInventory(itemId);
	if (!GameLogicUtilities::isStackable(itemId)) {
		required = amount; // These aren't stackable
	}
	else {
		auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId);
		slot_qty_t maxSlot = itemInfo->maxSlot;
		slot_qty_t existing = getItemAmount(itemId) % maxSlot;
		// Bug in global:
		// It doesn't matter if you already have a slot with a partial stack or not, non-shops require at least 1 empty slot
		if (canStack && existing > 0) {
			// If not, calculate how many slots necessary
			existing += amount;
			if (existing > maxSlot) {
				// Only have to bother with required slots if it would put us over the limit of a slot
				required = static_cast<slot_qty_t>(existing / maxSlot);
				if ((existing % maxSlot) > 0) {
					++required;
				}
			}
		}
		else {
			// If it is, treat it as though no items exist at all
			required = static_cast<slot_qty_t>(amount / maxSlot);
			if ((amount % maxSlot) > 0) {
				++required;
			}
		}
	}
	return getOpenSlotsNum(inv) >= required;
}

auto PlayerInventory::getOpenSlotsNum(inventory_t inv) -> inventory_slot_count_t {
	inventory_slot_count_t openSlots = 0;
	for (inventory_slot_count_t i = 1; i <= getMaxSlots(inv); ++i) {
		if (getItem(inv, i) == nullptr) {
			++openSlots;
		}
	}
	return openSlots;
}

auto PlayerInventory::doShadowStars() -> item_id_t {
	for (inventory_slot_t s = 1; s <= getMaxSlots(Inventories::UseInventory); ++s) {
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

auto PlayerInventory::addRockMap(map_id_t mapId, int8_t type) -> void {
	const int8_t mode = Packets::Inventory::RockModes::Add;
	if (type == Packets::Inventory::RockTypes::Regular) {
		if (m_rockLocations.size() < Inventories::TeleportRockMax) {
			m_rockLocations.push_back(mapId);
		}
		m_player->send(Packets::Inventory::sendRockUpdate(mode, type, m_rockLocations));
	}
	else if (type == Packets::Inventory::RockTypes::Vip) {
		if (m_vipLocations.size() < Inventories::VipRockMax) {
			m_vipLocations.push_back(mapId);
			// Want packet
		}
		m_player->send(Packets::Inventory::sendRockUpdate(mode, type, m_vipLocations));
	}
}

auto PlayerInventory::delRockMap(map_id_t mapId, int8_t type) -> void {
	const int8_t mode = Packets::Inventory::RockModes::Delete;
	if (type == Packets::Inventory::RockTypes::Regular) {
		for (size_t k = 0; k < m_rockLocations.size(); ++k) {
			if (m_rockLocations[k] == mapId) {
				m_rockLocations.erase(std::begin(m_rockLocations) + k);
				m_player->send(Packets::Inventory::sendRockUpdate(mode, type, m_rockLocations));
				break;
			}
		}
	}
	else if (type == Packets::Inventory::RockTypes::Vip) {
		for (size_t k = 0; k < m_vipLocations.size(); ++k) {
			if (m_vipLocations[k] == mapId) {
				m_vipLocations.erase(std::begin(m_vipLocations) + k);
				m_player->send(Packets::Inventory::sendRockUpdate(mode, type, m_vipLocations));
				break;
			}
		}
	}
}

auto PlayerInventory::swapItems(int8_t inventory, int16_t slot1, int16_t slot2) -> void {
	bool equippedSlot2 = (slot2 < 0);
	if (inventory == Inventories::EquipInventory && equippedSlot2) {
		// Handle these specially
		Item *item1 = getItem(inventory, slot1);
		if (item1 == nullptr) {
			// Hacking
			return;
		}

		item_id_t itemId1 = item1->getId();
		inventory_slot_t strippedSlot1 = GameLogicUtilities::stripCashSlot(slot1);
		inventory_slot_t strippedSlot2 = GameLogicUtilities::stripCashSlot(slot2);
		if (!ChannelServer::getInstance().getEquipDataProvider().isValidSlot(itemId1, strippedSlot2)) {
			// Hacking
			return;
		}

		auto bindTradeBlockOnEquip = [this, slot1, equippedSlot2, item1, itemId1](vector_t<InventoryPacketOperation> &ops) -> bool {
			// We don't care about any case other than equipping because we're checking for gear binds which only happen on first equip
			if (slot1 >= 0 && equippedSlot2) {
				auto &equipInfo = ChannelServer::getInstance().getEquipDataProvider().getEquipInfo(itemId1);
				if (equipInfo.tradeBlockOnEquip && !item1->hasTradeBlock()) {
					item1->setTradeBlock(true);
					ops.emplace_back(Packets::Inventory::OperationTypes::RemoveItem, item1, slot1);
					ops.emplace_back(Packets::Inventory::OperationTypes::AddItem, item1, slot1);
					return true;
				}
			}
			return false;
		};

		Item *remove = nullptr;
		inventory_slot_t oldSlot = 0;
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

				vector_t<InventoryPacketOperation> ops;
				bindTradeBlockOnEquip(ops);
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item1, slot1, slot2);
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, remove, swapSlot, slot1);
				m_player->send(Packets::Inventory::inventoryOperation(true, ops));
				m_player->sendMap(Packets::Inventory::updatePlayer(m_player));
				return;
			}
			else {
				if (getOpenSlotsNum(inventory) == 0) {
					m_player->send(Packets::Inventory::blankUpdate());
					return;
				}
				inventory_slot_t freeSlot = 0;
				for (inventory_slot_t s = 1; s <= getMaxSlots(inventory); s++) {
					Item *oldItem = getItem(inventory, s);
					if (oldItem == nullptr) {
						freeSlot = s;
						break;
					}
				}

				setItem(inventory, freeSlot, remove);
				setItem(inventory, oldSlot, nullptr);

				vector_t<InventoryPacketOperation> ops;
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item1, oldSlot, freeSlot);
				m_player->send(Packets::Inventory::inventoryOperation(true, ops));
			}
		}

		// Nothing special happening, just a simple equip swap
		Item *item2 = getItem(inventory, slot2);
		setItem(inventory, slot1, item2);
		setItem(inventory, slot2, item1);

		vector_t<InventoryPacketOperation> ops;
		bindTradeBlockOnEquip(ops);
		ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item1, slot1, slot2);
		m_player->send(Packets::Inventory::inventoryOperation(true, ops));
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

		item_id_t itemId1 = item1->getId();
		item_id_t itemId2 = item2 == nullptr ? 0 : item2->getId();
		if (item2 != nullptr && itemId1 == itemId2 && GameLogicUtilities::isStackable(itemId1)) {
			auto itemInfo = ChannelServer::getInstance().getItemDataProvider().getItemInfo(itemId1);
			slot_qty_t maxSlot = itemInfo->maxSlot;

			if (item1->getAmount() + item2->getAmount() <= maxSlot) {
				item2->incAmount(item1->getAmount());
				deleteItem(inventory, slot1, false);

				vector_t<InventoryPacketOperation> ops;
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifyQuantity, item2, slot2);
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item1, slot1);
				m_player->send(Packets::Inventory::inventoryOperation(true, ops));
			}
			else {
				item1->decAmount(maxSlot - item2->getAmount());
				item2->setAmount(maxSlot);

				vector_t<InventoryPacketOperation> ops;
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifyQuantity, item1, slot1);
				ops.emplace_back(Packets::Inventory::OperationTypes::ModifyQuantity, item2, slot2);
				m_player->send(Packets::Inventory::inventoryOperation(true, ops));
			}
		}
		else {
			// The item is not stackable, not the same item, or a blank slot swap is occurring, either way it's a plain swap
			setItem(inventory, slot1, item2);
			setItem(inventory, slot2, item1);
			if (item1->getPetId() > 0) {
				m_player->getPets()->getPet(item1->getPetId())->setInventorySlot(static_cast<int8_t>(slot2));
			}
			if (item2 != nullptr && item2->getPetId() > 0) {
				m_player->getPets()->getPet(item2->getPetId())->setInventorySlot(static_cast<int8_t>(slot1));
			}

			vector_t<InventoryPacketOperation> ops;
			ops.emplace_back(Packets::Inventory::OperationTypes::ModifySlot, item1, slot1, slot2);
			m_player->send(Packets::Inventory::inventoryOperation(true, ops));
		}
	}
}

auto PlayerInventory::ensureRockDestination(map_id_t mapId) -> bool {
	for (const auto &location : m_rockLocations) {
		if (location == mapId) {
			return true;
		}
	}
	for (const auto &location : m_vipLocations) {
		if (location == mapId) {
			return true;
		}
	}
	return false;
}

auto PlayerInventory::addWishListItem(item_id_t itemId) -> void {
	m_wishlist.push_back(itemId);
}

auto PlayerInventory::connectData(PacketBuilder &packet) -> void {
	packet.add<int32_t>(m_mesos);

	for (inventory_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		packet.add<inventory_slot_count_t>(getMaxSlots(i));
	}

	// Go through equips
	const auto &equips = m_items[Inventories::EquipInventory - 1];
	for (const auto &kvp : equips) {
		if (kvp.first < 0 && kvp.first > -100) {
			packet.addBuffer(Packets::Helpers::addItemInfo(kvp.first, kvp.second));
		}
	}
	packet.add<int8_t>(0);
	for (const auto &kvp : equips) {
		if (kvp.first < -100) {
			packet.addBuffer(Packets::Helpers::addItemInfo(kvp.first, kvp.second));
		}
	}
	packet.add<int8_t>(0);
	for (const auto &kvp : equips) {
		if (kvp.first > 0) {
			packet.addBuffer(Packets::Helpers::addItemInfo(kvp.first, kvp.second));
		}
	}
	packet.add<int8_t>(0);

	// Equips done, do rest of user's items starting with Use
	for (inventory_t i = Inventories::UseInventory; i <= Inventories::InventoryCount; ++i) {
		for (inventory_slot_count_t s = 1; s <= getMaxSlots(i); ++s) {
			Item *item = getItem(i, s);
			if (item == nullptr) {
				continue;
			}
			if (item->getPetId() == 0) {
				packet.addBuffer(Packets::Helpers::addItemInfo(s, item));
			}
			else {
				Pet *pet = m_player->getPets()->getPet(item->getPetId());
				packet.add<int8_t>(static_cast<int8_t>(s));
				packet.addBuffer(Packets::Pets::addInfo(pet, item));
			}
		}
		packet.add<int8_t>(0);
	}
}

auto PlayerInventory::rockPacket(PacketBuilder &packet) -> void {
	packet.addBuffer(Packets::Helpers::fillRockPacket(m_rockLocations, Inventories::TeleportRockMax));
	packet.addBuffer(Packets::Helpers::fillRockPacket(m_vipLocations, Inventories::VipRockMax));
}

auto PlayerInventory::wishListPacket(PacketBuilder &packet) -> void {
	packet.add<uint8_t>(static_cast<uint8_t>(m_wishlist.size()));
	for (const auto &item : m_wishlist) {
		packet.add<int32_t>(item);
	}
}

auto PlayerInventory::checkExpiredItems() -> void {
	vector_t<item_id_t> expiredItemIds;
	FileTime serverTime{};

	for (inventory_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		for (inventory_slot_count_t s = 1; s <= getMaxSlots(i); ++s) {
			if (Item *item = getItem(i, s)) {
				if (item->getExpirationTime() != Items::NoExpiration && item->getExpirationTime() <= serverTime) {
					expiredItemIds.push_back(item->getId());
					Inventory::takeItemSlot(m_player, i, s, item->getAmount());
				}
			}
		}
	}

	if (expiredItemIds.size() > 0) {
		m_player->send(Packets::Inventory::sendItemExpired(expiredItemIds));
	}
}

}