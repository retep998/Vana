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

	soci::rowset<> rs = (sql.prepare << "SELECT i.*, p.index, p.name AS pet_name, p.level, p.closeness, p.fullness " <<
										"FROM items i " <<
										"LEFT OUTER JOIN pets p ON i.pet_id = p.pet_id " <<
										"WHERE i.location = :location AND i.character_id = :char",
										soci::use(m_player->getId(), "char"),
										soci::use(location, "location"));

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
		item->setHammers(row.get<int32_t>("hammers"));
		item->setPetId(row.get<int32_t>("pet_id"));
		item->setName(row.get<string>("name"));
		item->setExpirationTime(row.get<int64_t>("expiration"));

		addItem(row.get<int8_t>("inv"), row.get<int16_t>("slot"), item, true);
		if (item->getPetId() != 0) {
			Pet *pet = new Pet(
				m_player,
				item,
				row.get<int8_t>("index"),
				row.get<string>("pet_name"),
				row.get<int8_t>("level"),
				row.get<int16_t>("closeness"),
				row.get<int8_t>("fullness"),
				row.get<int8_t>("slot")
			);
			m_player->getPets()->addPet(pet);
		}
	}

	rs = (sql.prepare << "SELECT t.map_index, t.map_id FROM teleport_rock_locations t WHERE t.character_id = :char",
							soci::use(m_player->getId(), "char"));

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
	string location = "inventory";

	sql.once << "DELETE FROM items WHERE location = :inv AND character_id = :char",
				use(charId, "char"),
				use(location, "inv");

	sql.once << "DELETE FROM teleport_rock_locations WHERE character_id = :char", use(charId, "char");

	if (m_rockLocations.size() > 0 || m_vipLocations.size() > 0) {
		int32_t mapId = 0;
		size_t i = 0;

		statement st = (sql.prepare << "INSERT INTO teleport_rock_locations " <<
												"VALUES (:char, :i, :map)",
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

	int8_t slots = 0;
	int8_t scrolls = 0;
	uint8_t inv = 0;
	int16_t slot = 0;
	int16_t amount = 0;
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
	int32_t itemId = 0;
	int32_t hammers = 0;
	int64_t petId = 0;
	int64_t expiration = 0;
	string name = "";

	statement st = (sql.prepare << "INSERT INTO items " <<
									"VALUES (" <<
									":char, :inv, :slot, :location, :user, " <<
									":world, :item, :amount, :slots, :scrolls, " <<
									":str, :dex, :int, :luk, :hp, " <<
									":mp, :watk, :matk, :wdef, :mdef, " <<
									":acc, :avo, :hands, :speed, :jump, " <<
									":flags, :hammers, :pet, :name, :expiration" <<
									")",
									use(charId, "char"),
									use(inv, "inv"),
									use(slot, "slot"),
									use(location, "location"),
									use(m_player->getUserId(), "user"),
									use(m_player->getWorldId(), "world"),
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

	for (int8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		ItemInventory &itemsInv = m_items[i - 1];
		for (ItemInventory::iterator iter = itemsInv.begin(); iter != itemsInv.end(); ++iter) {
			Item *item = iter->second;
			slot = iter->first;

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

	packet.add<int8_t>(0);
}

uint16_t PlayerInventory::getItemAmount(int32_t itemId) {
	return m_itemAmounts.find(itemId) != m_itemAmounts.end() ? m_itemAmounts[itemId] : 0;
}

bool PlayerInventory::isEquippedItem(int32_t itemId) {
	ItemInventory &equips = m_items[Inventories::EquipInventory - 1];
	bool has = false;
	for (ItemInventory::iterator iter = equips.begin(); iter != equips.end(); ++iter) {
		if (iter->first == itemId) {
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

	// New V.100+
	{
		size_t amount = 0;
		packet.add<int32_t>(amount);
		for (size_t i = 0; i < amount; i++) {
			packet.add<int32_t>(0); // I have no idea!
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
			packet.add<int32_t>(0);
		}
	}

	for (uint8_t i = Inventories::EquipInventory; i <= Inventories::InventoryCount; ++i) {
		packet.add<int8_t>(getMaxSlots(i));
	}

	packet.add<int64_t>(94354848000000000); // New in V.83+?

	// Go through equips
	ItemInventory &equips = m_items[Inventories::EquipInventory - 1];
	ItemInventory::reverse_iterator iter;
	for (iter = equips.rbegin(); iter != equips.rend(); ++iter) {
		if (iter->first < 0 && iter->first > -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second, true);
		}
	}
	packet.add<int16_t>(0);
	for (iter = equips.rbegin(); iter != equips.rend(); ++iter) {
		if (iter->first < -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second, true);
		}
	}
	packet.add<int16_t>(0);
	for (iter = equips.rbegin(); iter != equips.rend(); ++iter) {
		if (iter->first > 0) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second, true);
		}
	}
	packet.add<int16_t>(0);

	packet.add<int16_t>(0);

	packet.add<int16_t>(0);

	packet.add<int16_t>(0);

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
				PetsPacket::addInfo(packet, pet);
			}
		}
		packet.add<int8_t>(0);
	}
}

void PlayerInventory::rockPacket(PacketCreator &packet) {
	InventoryPacketHelper::fillRockPacket(packet, m_rockLocations, Inventories::TeleportRockMax);
	InventoryPacketHelper::fillRockPacket(packet, m_vipLocations, Inventories::VipRockMax);
	vector<int32_t> empty;
	InventoryPacketHelper::fillRockPacket(packet, empty, Inventories::HyperRockMax);
	InventoryPacketHelper::fillRockPacket(packet, empty, Inventories::HyperRockMax);

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