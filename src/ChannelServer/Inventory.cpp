/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Inventory.h"
#include "Buffs.h"
#include "Drops.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "Levels.h"
#include "Maps.h"
#include "Pets.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "PacketReader.h"
#include "ShopDataProvider.h"
#include "Skills.h"
#include "StoragePacket.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <cmath>
#include <functional>

using std::tr1::bind;

void Inventory::itemMove(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int8_t inv = packet.getByte();
	int16_t slot1 = packet.getShort();
	int16_t slot2 = packet.getShort();

	if (slot2 == 0) {
		int16_t amount = packet.getShort();
		Item *item = player->getInventory()->getItem(inv, slot1);
		if (item == 0)
			return;
		if (ISRECHARGEABLE(item->id))
			amount = item->amount;
		Item droppeditem = Item(item);
		droppeditem.amount = amount;
		if (item->amount == amount) {
			InventoryPacket::moveItem(player, inv, slot1, slot2);
			player->getInventory()->deleteItem(inv, slot1);
		}
		else {
			item->amount -= amount;
			player->getInventory()->changeItemAmount(item->id, -amount);
			InventoryPacket::updateItemAmounts(player, inv, slot1, item->amount, 0, 0);
		}
		Drop *drop = new Drop(player->getMap(), droppeditem, player->getPos(), player->getId(), true);
		drop->setTime(0);
		bool istradeable = true;
		if (ISEQUIP(droppeditem.id)) {
			EquipInfo info = ItemDataProvider::Instance()->getEquipInfo(droppeditem.id);
			istradeable = !(info.notrade || info.quest);
		}
		else {
			ItemInfo info = ItemDataProvider::Instance()->getItemInfo(droppeditem.id);
			istradeable = !(info.notrade || info.quest);
		}
		drop->setTradeable(istradeable);
		drop->doDrop(player->getPos());
		if (istradeable) // Drop is deleted otherwise, avoid like plague
			Reactors::checkDrop(player, drop);
	}
	else {
		Item *item1 = player->getInventory()->getItem(inv, slot1);
		Item *item2 = player->getInventory()->getItem(inv, slot2);

		if (item1 == 0) {
			// Hacking
			return;
		}

		if (item2 != 0 && !ISRECHARGEABLE(item1->id) && !ISEQUIP(item1->id) && !ISPET(item1->id) && item1->id == item2->id) {
			if (item1->amount + item2->amount <= ItemDataProvider::Instance()->getMaxslot(item1->id)) {
				item2->amount += item1->amount;
				player->getInventory()->deleteItem(inv, slot1, false);
				InventoryPacket::updateItemAmounts(player, inv, slot2, item2->amount, 0, 0);
				InventoryPacket::moveItem(player, inv, slot1, 0);
			}
			else {
				item1->amount -= (ItemDataProvider::Instance()->getMaxslot(item1->id) - item2->amount);
				item2->amount = ItemDataProvider::Instance()->getMaxslot(item2->id);
				InventoryPacket::updateItemAmounts(player, inv, slot1, item1->amount, slot2, item2->amount);
			}
		}
		else {
			player->getInventory()->setItem(inv, slot1, item2);
			player->getInventory()->setItem(inv, slot2, item1);
			if (item1->petid > 0)
				player->getPets()->getPet(item1->petid)->setInventorySlot((int8_t) slot2);
			if (item2 != 0 && item2->petid > 0)
				player->getPets()->getPet(item2->petid)->setInventorySlot((int8_t) slot1);
			InventoryPacket::moveItem(player, inv, slot1, slot2);
		}
	}
	if (slot1 < 0 || slot2 < 0)
		InventoryPacket::updatePlayer(player);
}

int16_t Inventory::addItem(Player *player, Item *item, bool is) {
	int8_t inv = GETINVENTORY(item->id);
	int16_t freeslot = 0;
	for (int16_t s = 1; s <= player->getInventory()->getMaxSlots(inv); s++) {
		Item *olditem = player->getInventory()->getItem(inv, s);
		if (olditem != 0) {
			if (!ISRECHARGEABLE(item->id) && !ISEQUIP(item->id) && !ISPET(item->id) && olditem->id == item->id && olditem->amount < ItemDataProvider::Instance()->getMaxslot(item->id)) {
				if (item->amount + olditem->amount > ItemDataProvider::Instance()->getMaxslot(item->id)) {
					int16_t amount = ItemDataProvider::Instance()->getMaxslot(item->id) - olditem->amount;
					item->amount -= amount;
					olditem->amount = ItemDataProvider::Instance()->getMaxslot(item->id);
					InventoryPacket::addItem(player, inv, s, olditem, is);
				}
				else {
					item->amount += olditem->amount;
					player->getInventory()->deleteItem(inv, s);
					player->getInventory()->addItem(inv, s, item);
					InventoryPacket::addItem(player, inv, s, item, is);
					return 0;
				}
			}
		}
		else if (!freeslot) {
			freeslot = s;
			if (ISRECHARGEABLE(item->id) || ISEQUIP(item->id) || ISPET(item->id))
				break;
		}
	}
	if (freeslot != 0) {
		player->getInventory()->addItem(inv, freeslot, item);
		InventoryPacket::addNewItem(player, inv, freeslot, item, is);
		if (ISPET(item->id)) {
			Pet *pet = new Pet(player, item);
			player->getPets()->addPet(pet);
			pet->setInventorySlot((int8_t) freeslot);
			PetsPacket::updatePet(player, pet);
		}
		return 0;
	}
	else {
		return item->amount;
	}
}

void Inventory::useShop(Player *player, PacketReader &packet) {
	int8_t type = packet.getByte();
	switch (type) {
		case 0: { // Buy
			packet.skipBytes(2);
			int32_t itemid = packet.getInt();
			int16_t amount = packet.getShort();
			int32_t price = ShopDataProvider::Instance()->getPrice(player->getShop(), itemid);
			if (price == 0 || player->getInventory()->getMesos() < (price*amount) ||  amount > ItemDataProvider::Instance()->getMaxslot(itemid)) {
				// Hacking
				return;
			}
			bool haveslot = player->getInventory()->hasOpenSlotsFor(itemid, amount, true);
			if (haveslot) {
				addNewItem(player, itemid, amount);
				player->getInventory()->modifyMesos(-(price * amount));
			}
			InventoryPacket::bought(player, haveslot ? 0 : 3);
			break;
		}
		case 1: { // Sell
			int16_t slot = packet.getShort();
			int32_t itemid = packet.getInt();
			int16_t amount = packet.getShort();
			int8_t inv = GETINVENTORY(itemid);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == 0 || (!ISRECHARGEABLE(itemid) && amount > item->amount)) {
				InventoryPacket::bought(player, 1); // Hacking
				return;
			}
			int32_t price = ItemDataProvider::Instance()->getPrice(itemid);

			player->getInventory()->modifyMesos(price * amount);
			if (ISRECHARGEABLE(itemid))
				takeItemSlot(player, inv, slot, item->amount, true);
			else
				takeItemSlot(player, inv, slot, amount, true);
			InventoryPacket::bought(player, 0);
			break;
		}
		case 2: { // Recharge
			int16_t slot = packet.getShort();
			Item *item = player->getInventory()->getItem(2, slot);
			if (item == 0 || ISRECHARGEABLE(item->id) == false) {
				// Hacking
				return;
			}
			item->amount = ItemDataProvider::Instance()->getMaxslot(item->id) + (ISSTAR(item->id) ? player->getSkills()->getSkillLevel(4100000) * 10 : player->getSkills()->getSkillLevel(5200000) * 10);
			player->getInventory()->modifyMesos(-1); // TODO: Calculate price, letting players recharge for 1 meso for now
			InventoryPacket::updateItemAmounts(player, 2, slot, item->amount, 0, 0);
			InventoryPacket::bought(player, 0);
			break;
		}
		case 3:
			// 3 is close shop. For now we have no reason to handle this.
			break;
	}
}

void Inventory::useStorage(Player *player, PacketReader &packet) {
	int8_t type = packet.getByte();
	switch (type) {
		case 4: { // Take item out
			int8_t inv = packet.getByte(); // Inventory, as in equip, use, etc
			int8_t slot = packet.getByte(); // Slot within the inventory
			Item *item = player->getStorage()->getItem(slot);
			if (item == 0) { // It's a trap
				// hacking
				return; // Abort
			}
			addItem(player, new Item(item));
			player->getStorage()->takeItem(slot);
			StoragePacket::takeItem(player, inv);
			break;
		}
		case 5: { // Store item
			int16_t slot = packet.getShort();
			int32_t itemid = packet.getInt();
			int16_t amount = packet.getShort();
			if (player->getStorage()->isFull()) { // Storage is full, so tell the player and abort the mission.
				StoragePacket::storageFull(player);
				return;
			}
			int8_t inv = GETINVENTORY(itemid);
			Item *item = player->getInventory()->getItem(inv, slot);
			if (item == 0 || (!ISRECHARGEABLE(itemid) && amount > item->amount)) { // Be careful, it might be a trap.
				// hacking
				return; // Do a barrel roll
			}
			player->getStorage()->addItem((inv == 1 || ISRECHARGEABLE(itemid)) ? new Item(item) : new Item(itemid, amount));
			// For equips or rechargeable items (stars/bullets) we create a
			// new object for storage with the inventory object, and allow
			// the one in the inventory to go bye bye.
			// Else: For items we just create a new item based on the ID and amount.
			takeItemSlot(player, inv, slot, ISRECHARGEABLE(itemid) ? item->amount : amount, true);
			player->getInventory()->modifyMesos(-100); // Take 100 mesos for storage cost
			StoragePacket::addItem(player, inv);
			break;
		}
		case 7: { // Take out/store mesos
			int32_t mesos = packet.getInt(); // Amount of mesos to remove. Deposits are negative, and withdrawls are positive.
			bool success = player->getInventory()->modifyMesos(mesos);
			if (success)
				player->getStorage()->changeMesos(mesos);
			break;
		}
		case 8:
			// 8 is close storage. For now we have no reason to handle this.
			break;
	}
}

void Inventory::addNewItem(Player *player, int32_t itemid, int16_t amount) {
	if (!ItemDataProvider::Instance()->itemExists(itemid))
		return;
	int8_t inv = GETINVENTORY(itemid);
	int16_t max = ItemDataProvider::Instance()->getMaxslot(itemid);
	int16_t thisamount = 0;
	if (ISSTAR(itemid)) {
		thisamount = max + player->getSkills()->getSkillLevel(4100000) * 10;
		amount -= 1;
	}
	else if (ISBULLET(itemid)) {
		thisamount = max + player->getSkills()->getSkillLevel(5200000) * 10;
		amount -= 1;
	}
	else if (ISEQUIP(itemid) || ISPET(itemid)) {
		thisamount = 1;
		amount -= 1;
	}
	else if (amount > max) {
		thisamount = max;
		amount -= max;
	}
	else {
		thisamount = amount;
		amount = 0;
	}

	Item *item = 0;
	if (ISEQUIP(itemid))
		item = new Item(itemid, false);
	else
		item = new Item(itemid, thisamount);

	if (addItem(player, item, ISPET(itemid)) == 0 && amount > 0)
		addNewItem(player, itemid, amount);
}

void Inventory::takeItem(Player *player, int32_t itemid, uint16_t howmany) {
	player->getInventory()->changeItemAmount(itemid, -howmany);
	int8_t inv = GETINVENTORY(itemid);
	for (int16_t i = 1; i <= player->getInventory()->getMaxSlots(inv); i++) {
		Item *item = player->getInventory()->getItem(inv, i);
		if (item == 0)
			continue;
		if (item->id == itemid) {
			if (item->amount >= howmany) {
				item->amount -= howmany;
				if (item->amount == 0 && !ISRECHARGEABLE(item->id)) {
					InventoryPacket::moveItem(player, inv, i, 0);
					player->getInventory()->deleteItem(inv, i);
				}
				else {
					InventoryPacket::updateItemAmounts(player, inv, i, item->amount, 0, 0);
				}
				break;
			}
			else if (!ISRECHARGEABLE(item->id)) {
				howmany -= item->amount;
				item->amount = 0;
				InventoryPacket::moveItem(player, inv, i, 0);
				player->getInventory()->deleteItem(inv, i);
			}
		}
	}
}

void Inventory::takeItemSlot(Player *player, int8_t inv, int16_t slot, int16_t amount, bool takeStar) {
	Item *item = player->getInventory()->getItem(inv, slot);
	if (item == 0 || item->amount - amount < 0)
		return;

	item->amount -= amount;
	if ((item->amount == 0 && !ISRECHARGEABLE(item->id)) || (takeStar && ISRECHARGEABLE(item->id))) {
		InventoryPacket::moveItem(player, inv, slot, 0);
		player->getInventory()->deleteItem(inv, slot);
	}
	else {
		player->getInventory()->changeItemAmount(item->id, -amount);
		InventoryPacket::updateItemAmounts(player, inv, slot, item->amount, 0, 0);
	}
}

void Inventory::useItem(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	int32_t itemid = packet.getInt();
	if (player->getHP() == 0 || player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	takeItemSlot(player, 2, slot, 1);
	useItem(player, itemid);
}

void Inventory::useItem(Player *player, int32_t itemid) {
	ItemInfo item = ItemDataProvider::Instance()->getItemInfo(itemid);
	// Alchemist
	int16_t alchemist = 0;
	if (player->getSkills()->getSkillLevel(4110000) > 0)
		alchemist = Skills::skills[4110000][player->getSkills()->getSkillLevel(4110000)].x;
	if (item.cons.hp > 0)
		player->modifyHP(item.cons.hp + ((item.cons.hp * alchemist) / 100));
	if (item.cons.mp > 0)
		player->modifyMP(item.cons.mp + ((item.cons.mp * alchemist) / 100));
	else
		player->setMP(player->getMP(), true);
	if (item.cons.hpr > 0)
		player->modifyHP(item.cons.hpr * player->getMHP() / 100);
	if (item.cons.mpr > 0)
		player->modifyMP(item.cons.mpr * player->getMMP() / 100);
	// Item buffs
	if (item.cons.time > 0) {
		int32_t time = item.cons.time;
		if (alchemist > 0)
			time = (time * alchemist) / 100;
		SkillActiveInfo iteminfo;
		memcpy(iteminfo.types, item.cons.types, sizeof(uint8_t[8]));
		iteminfo.vals = item.cons.vals;
		Buffs::Instance()->addBuff(player, itemid, time, iteminfo, (item.cons.morph > 0));
	}
}
// Cancel item buffs
void Inventory::cancelItem(Player *player, PacketReader &packet) {
	int32_t itemid = packet.getInt();
	Buffs::Instance()->endBuff(player, itemid);
}
// Skill books
void Inventory::useSkillbook(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	int32_t itemid = packet.getInt();

	if (player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}

	int32_t skillid = 0;
	uint8_t newMaxLevel = 0;
	bool use = false;
	bool succeed = false;

	ItemInfo item = ItemDataProvider::Instance()->getItemInfo(itemid);
	for (size_t i = 0; i < item.cons.skills.size(); i++) {
		skillid = item.cons.skills[i].skillid;
		newMaxLevel = item.cons.skills[i].maxlevel;
		if (player->getJob() == item.cons.skills[i].skillid / 10000) { // Make sure the skill is for the person's job
			if (player->getSkills()->getSkillLevel(skillid) >= item.cons.skills[i].reqlevel && player->getSkills()->getMaxSkillLevel(skillid) < newMaxLevel)
				use = true;
		}
		if (use) {
			if ((int16_t) Randomizer::Instance()->randShort(100) <= item.cons.success) {
				player->getSkills()->setMaxSkillLevel(skillid, newMaxLevel);
				succeed = true;
			}
			takeItemSlot(player, 2, slot, 1);
			break;
		}
	}

	if (skillid == 0) return;

	InventoryPacket::useSkillbook(player, skillid, newMaxLevel, use, succeed);
}
void Inventory::useChair(Player *player, PacketReader &packet) {
	int32_t chairid = packet.getInt();
	player->setChair(chairid);
	InventoryPacket::sitChair(player, chairid);
}

void Inventory::stopChair(Player *player, PacketReader &packet) {
	player->setChair(0);
	InventoryPacket::stopChair(player);
}

void Inventory::useSummonBag(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	int32_t itemid = packet.getInt();
	if (player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	if (!ItemDataProvider::Instance()->itemExists(itemid))
		return;
	takeItemSlot(player, 2, slot, 1);

	ItemInfo item = ItemDataProvider::Instance()->getItemInfo(itemid);
	for (size_t i = 0; i < item.cons.mobs.size(); i++) {
		if (Randomizer::Instance()->randInt(100) <= item.cons.mobs[i].chance) {
			Maps::getMap(player->getMap())->spawnMob(item.cons.mobs[i].mobid, player->getPos());
		}
	}
}

void Inventory::useReturnScroll(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	int32_t itemid = packet.getInt();
	if (player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	if (!ItemDataProvider::Instance()->itemExists(itemid))
		return;
	takeItemSlot(player, 2, slot, 1);
	int32_t map = ItemDataProvider::Instance()->getItemInfo(itemid).cons.moveTo;
	if (map == 999999999)
		Maps::changeMap(player, Maps::getMap(player->getMap())->getInfo()->rm, 0);
	else
		Maps::changeMap(player, map, 0);
}

void Inventory::useScroll(Player *player, PacketReader &packet) {
	packet.skipBytes(4);
	int16_t slot = packet.getShort();
	int16_t eslot = packet.getShort();
	bool wscroll = (packet.getShort() == 2);
	bool legendary_spirit = (packet.getByte() != 0);

	Item *item = player->getInventory()->getItem(2, slot);
	Item *equip = player->getInventory()->getItem(1, eslot);
	if (item == 0 || equip == 0)
		return;

	int32_t itemid = item->id;
	int8_t succeed = -1;
	bool cursed = false;
	if (!ItemDataProvider::Instance()->itemExists(itemid))
		return;

	ItemInfo iteminfo = ItemDataProvider::Instance()->getItemInfo(itemid);
	switch (itemid) {
		case 2049000: // Clean Slate 1%
		case 2049001: // Clean Slate 3%
		case 2049002: // Clean Slate 5%
		case 2049003: // Clean Slate 20%
			if ((ItemDataProvider::Instance()->getEquipInfo(equip->id).slots - equip->scrolls) > equip->slots) {
				if ((int16_t) Randomizer::Instance()->randShort(99) < iteminfo.cons.success) { // Give back a slot
					equip->slots++;
					succeed = 1;
				}
				else {
					if ((int16_t) Randomizer::Instance()->randShort(99) < iteminfo.cons.cursed)
						cursed = true;
					succeed = 0;
				}
			}
			break;
		case 2049102: // Maple Syrup 100%
		case 2049101: // Liar Tree Sap 100%
		case 2049100: // Chaos Scroll
			if (equip->slots > 0) {
				succeed = 0;
				if (wscroll)
					takeItem(player, 2340000, 1);
				if ((int16_t) Randomizer::Instance()->randShort(99) < iteminfo.cons.success) { // Add stats
					int8_t n = -1; // Default - Decrease stats
					if ((int16_t) Randomizer::Instance()->randShort(99) < 50) // Increase
						n = 1;
					// Gives/takes 0-5 stats on every stat on the item
					if (equip->istr > 0)
						equip->istr += Randomizer::Instance()->randShort(5) * n;
					if (equip->idex > 0)
						equip->idex += Randomizer::Instance()->randShort(5) * n;
					if (equip->iint > 0)
						equip->iint += Randomizer::Instance()->randShort(5) * n;
					if (equip->iluk > 0)
						equip->iluk += Randomizer::Instance()->randShort(5) * n;
					if (equip->iavo > 0)
						equip->iavo += Randomizer::Instance()->randShort(5) * n;
					if (equip->iacc > 0)
						equip->iacc += Randomizer::Instance()->randShort(5) * n;
					if (equip->ihand > 0)
						equip->ihand += Randomizer::Instance()->randShort(5) * n;
					if (equip->ijump > 0)
						equip->ijump += Randomizer::Instance()->randShort(5) * n;
					if (equip->ispeed > 0)
						equip->ispeed += Randomizer::Instance()->randShort(5) * n;
					if (equip->imatk > 0)
						equip->imatk += Randomizer::Instance()->randShort(5) * n;
					if (equip->iwatk > 0)
						equip->iwatk += Randomizer::Instance()->randShort(5) * n;
					if (equip->imdef > 0)
						equip->imdef += Randomizer::Instance()->randShort(5) * n;
					if (equip->iwdef > 0)
						equip->iwdef += Randomizer::Instance()->randShort(5) * n;
					if (equip->ihp > 0)
						equip->ihp += Randomizer::Instance()->randShort(5) * n;
					if (equip->imp > 0)
						equip->imp += Randomizer::Instance()->randShort(5) * n;
					equip->scrolls++;
					equip->slots--;
					succeed = 1;
				}
				else if (!wscroll)
					equip->slots--;
			}
			break;
		case 2040727: // Shoe for Spikes 10%
		case 2041058: // Cape for Cold Protection 10%
			succeed = 0;
			if ((int16_t) Randomizer::Instance()->randShort(99) < iteminfo.cons.success) { // These do not take slots and can be used even after success
				switch (itemid) {
					case 2040727:
						equip->flags |= FLAG_SPIKES;
						break;
					case 2041058:
						equip->flags |= FLAG_COLD;
						break;
				}
				succeed = 1;
			}
			break;
		default: // Most scrolls
			if (equip->slots > 0) {
				if (wscroll)
					takeItem(player, 2340000, 1);
				if ((int16_t) Randomizer::Instance()->randShort(99) < iteminfo.cons.success) {
					succeed = 1;
					equip->istr += iteminfo.cons.istr;
					equip->idex += iteminfo.cons.idex;
					equip->iint += iteminfo.cons.iint;
					equip->iluk += iteminfo.cons.iluk;
					equip->ihp += iteminfo.cons.ihp;
					equip->imp += iteminfo.cons.imp;
					equip->iwatk += iteminfo.cons.iwatk;
					equip->imatk += iteminfo.cons.imatk;
					equip->iwdef += iteminfo.cons.iwdef;
					equip->imdef += iteminfo.cons.imdef;
					equip->iacc += iteminfo.cons.iacc;
					equip->iavo += iteminfo.cons.iavo;
					equip->ihand += iteminfo.cons.ihand;
					equip->ijump += iteminfo.cons.ijump;
					equip->ispeed += iteminfo.cons.ispeed;
					equip->scrolls++;
					equip->slots--;
				}
				else {
					succeed = 0;
					if ((int16_t) Randomizer::Instance()->randShort(99) < iteminfo.cons.cursed)
						cursed = true;
					else if (!wscroll)
						equip->slots--;
				}
			}
			break;
	}
	if (succeed != -1) {
		takeItemSlot(player, 2, slot, 1);
		InventoryPacket::useScroll(player, succeed, cursed, legendary_spirit);
		if (!cursed)
			InventoryPacket::addNewItem(player, 1, eslot, equip, true);
		else {
			InventoryPacket::moveItem(player, 1, eslot, 0);
			player->getInventory()->deleteItem(1, eslot);
		}
		InventoryPacket::updatePlayer(player);
	}
	else {
		if (legendary_spirit)
			InventoryPacket::useScroll(player, succeed, cursed, legendary_spirit);
		InventoryPacket::blankUpdate(player);
	}
}

void Inventory::useCashItem(Player *player, PacketReader &packet) {
	int8_t type = packet.getByte();
	packet.skipBytes(1);
	int32_t itemid = packet.getInt();
	bool used = false;
	switch (itemid) {
		case 5050001: // 1st job SP Reset
		case 5050002: // 2nd job SP Reset
		case 5050003: // 3rd job SP Reset
		case 5050004: { // 4th job SP Reset
			int32_t toskill = packet.getInt();
			int32_t fromskill = packet.getInt();
			if (!player->getSkills()->addSkillLevel(fromskill, -1, true)) {
				// Hacking
				return;
			}
			if (!player->getSkills()->addSkillLevel(toskill, 1, true)) {
				// Hacking
				return;
			}
			used = true;
			break;
		}
		case 5050000: { // AP Reset
			int32_t tostat = packet.getInt();
			int32_t fromstat = packet.getInt();
			Levels::addStat(player, tostat, true, false);
			Levels::addStat(player, fromstat, true, true);
			used = true;
			break;
		}
		case 5071000: { // Megaphone
			string msg = packet.getString();
			InventoryPacket::showMegaphone(player, msg);
			used = true;
			break;
		}
		case 5072000: { // Super Megaphone
			string msg = packet.getString();
			uint8_t whisper = packet.getByte();
			InventoryPacket::showSuperMegaphone(player, msg, whisper);
			used = true;
			break;
		}
		case 5390000: // Diablo Messenger
		case 5390001: // Cloud 9 Messenger
		case 5390002: { // Loveholic Messenger
			string msg = packet.getString();
			string msg2 = packet.getString();
			string msg3 = packet.getString();
			string msg4 = packet.getString();

			InventoryPacket::showMessenger(player, msg, msg2, msg3, msg4, packet.getBuffer(), packet.getBufferLength(), itemid);
			used = true;
			break;
		}
		case 5170000: { // Pet Name Tag
			string name = packet.getString();
			Pets::changeName(player, name);
			used = true;
			break;
		}
		case 5060000: { // Item Name Tag
			int16_t slot = packet.getShort();
			if (slot != 0) {
				Item *item = player->getInventory()->getItem(1, slot);
				if (item == 0) {
					// Hacking or failure, dunno
					return;
				}
				item->name = player->getName();
				InventoryPacket::addNewItem(player, 1, slot, item, true);
				used = true;
			}
			break;
		}
		case 5060001: { // Item Lock
			int8_t inventory = (int8_t) packet.getInt();
			int16_t slot = (int16_t) packet.getInt();
			if (slot != 0) {
				Item *item = player->getInventory()->getItem(inventory, slot);
				if (item == 0) {
					// Hacking or failure, dunno
					return;
				}
				item->flags |= FLAG_LOCK;
				InventoryPacket::addNewItem(player, inventory, slot, item, true);
				used = true;
			}
			break;
		}
		case 5300000: // Fungus Scroll
		case 5300001: // Oinker Delight
		case 5300002: // Zeta Nightmare
			useItem(player, itemid);
			used = true;
			break;
		default:
			break;
	}
	if (used)
		Inventory::takeItem(player, itemid, 1);
	else
		InventoryPacket::blankUpdate(player);
}

void Inventory::useItemEffect(Player *player, PacketReader &packet) {
	int32_t itemid = packet.getInt();
	if (player->getInventory()->getItemAmount(itemid) == 0) {
		// hacking
		return;
	}
	player->setItemEffect(itemid);
	InventoryPacket::useItemEffect(player, itemid);
}
