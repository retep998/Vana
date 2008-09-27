/*
Copyright (C) 2008 Vana Development Team

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
#include "Maps.h"
#include "Pets.h"
#include "PetsPacket.h"
#include "Player.h"
#include "Randomizer.h"
#include "Reactors.h"
#include "ReadPacket.h"
#include "Shops.h"
#include "Skills.h"
#include "StoragePacket.h"
#include "Timer/Timer.h"
#include <cmath>
#include <functional>
#include <unordered_map>

using std::tr1::bind;
using std::tr1::unordered_map;

unordered_map<int32_t, EquipInfo> Inventory::equips;
unordered_map<int32_t, ItemInfo> Inventory::items;

void Inventory::addEquipInfo(int32_t id, EquipInfo equip) {
	equips[id] = equip;
}

void Inventory::addItemInfo(int32_t id, ItemInfo item) {
	if (ISRECHARGEABLE(id))
		Shops::rechargables.push_back(id);
	// Set all types to 0 initially
	memset(item.cons.types, 0, sizeof(item.cons.types));

	if (item.cons.watk > 0) {
		item.cons.types[TYPE_1] += 0x01;
		item.cons.vals.push_back(item.cons.watk);
	}
	if (item.cons.wdef > 0) {
		item.cons.types[TYPE_1] += 0x02;
		item.cons.vals.push_back(item.cons.wdef);
	}
	if (item.cons.matk > 0) {
		item.cons.types[TYPE_1] += 0x04;
		item.cons.vals.push_back(item.cons.matk);
	}
	if (item.cons.mdef > 0) {
		item.cons.types[TYPE_1] += 0x08;
		item.cons.vals.push_back(item.cons.mdef);
	}
	if (item.cons.acc > 0) {
		item.cons.types[TYPE_1] += 0x10;
		item.cons.vals.push_back(item.cons.acc);
	}
	if (item.cons.avo > 0) {
		item.cons.types[TYPE_1] += 0x20;
		item.cons.vals.push_back(item.cons.avo);
	}
	if (item.cons.speed > 0) {
		item.cons.types[TYPE_1] += 0x80;
		item.cons.vals.push_back(item.cons.speed);
	}
	if (item.cons.jump > 0) {
		item.cons.types[TYPE_2] = 0x01;
		item.cons.vals.push_back(item.cons.jump);
	}
	if (item.cons.morph > 0) {
		item.cons.types[TYPE_5] = 0x02;
		item.cons.vals.push_back(item.cons.morph);
	}

	items[id] = item;
}

void Inventory::itemMove(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int8_t inv = packet->getByte();
	int16_t slot1 = packet->getShort();
	int16_t slot2 = packet->getShort();

	if (slot2 == 0) {
		int16_t amount = packet->getShort();
		Item *item = player->getInventory()->getItem(inv, slot1);
		if (item == 0)
			return;
		if (ISRECHARGEABLE(item->id)) amount = item->amount;
		Item droppeditem = Item(item);
		droppeditem.amount = amount;
		item->amount -= amount;
		if (item->amount == 0) {
			InventoryPacket::moveItem(player, inv, slot1, slot2);
			player->getInventory()->deleteItem(inv, slot1);
		}
		else {
			player->getInventory()->changeItemAmount(item->id, -amount);
			InventoryPacket::updateItemAmounts(player, inv, slot1, item->amount, 0, 0);
		}
		Drop *drop = new Drop(player->getMap(), droppeditem, player->getPos(), player->getId(), true);
		drop->setTime(0);
		drop->doDrop(player->getPos());
		Reactors::checkDrop(player, drop);
	}
	else {
		Item *item1 = player->getInventory()->getItem(inv, slot1);
		Item *item2 = player->getInventory()->getItem(inv, slot2);

		if (item1 == 0) {
			// hacking
			return;
		}

		if (item2 != 0 && !ISRECHARGEABLE(item1->id) && !ISEQUIP(item1->id) && !ISPET(item1->id) && item1->id == item2->id) {
			if (item1->amount + item2->amount <= items[item1->id].maxslot) {
				item2->amount += item1->amount;
				player->getInventory()->deleteItem(inv, slot1);
				InventoryPacket::updateItemAmounts(player, inv, slot2, item2->amount, 0, 0);
				InventoryPacket::moveItem(player, inv, slot1, 0);
			}
			else {
				item1->amount -= (items[item1->id].maxslot - item2->amount);
				item2->amount = items[item2->id].maxslot;
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
			if (!ISRECHARGEABLE(item->id) && !ISEQUIP(item->id) && !ISPET(item->id) && olditem->id == item->id && olditem->amount < items[item->id].maxslot) {
				if (item->amount + olditem->amount > items[item->id].maxslot) {
					int16_t amount = items[item->id].maxslot - olditem->amount;
					item->amount -= amount;
					olditem->amount = items[item->id].maxslot;
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

void Inventory::useShop(Player *player, ReadPacket *packet) {
	int8_t type = packet->getByte();
	if (type == 0) { // Buy
		packet->skipBytes(2);
		int32_t itemid = packet->getInt();
		int16_t howmany = packet->getShort();
		int32_t price = Shops::getPrice(player->getShop(), itemid);
		if (price == 0) {
			// hacking
			return;
		}
		addNewItem(player, itemid, howmany);
		player->getInventory()->modifyMesos(-1 * (price * howmany));
		InventoryPacket::bought(player);
	}
	else if (type == 1) { // Sell
		int16_t slot = packet->getShort();
		int32_t itemid = packet->getInt();
		int16_t amount = packet->getShort();
		int8_t inv = GETINVENTORY(itemid);
		Item *item = player->getInventory()->getItem(inv, slot);
		if (item == 0 || item->amount < amount) {
			// hacking
			return;
		}
		int32_t price = 0;
		if (inv == 1)
			price = equips[itemid].price;
		else
			price = items[itemid].price;
		player->getInventory()->modifyMesos(price * amount);
		takeItemSlot(player, inv, slot, amount, true);
		InventoryPacket::bought(player);
	}
	else if (type == 2) { // Recharge
		int16_t slot = packet->getShort();
		Item *item = player->getInventory()->getItem(2, slot);
		if (ISSTAR(item->id))
			item->amount = items[item->id].maxslot + player->getSkills()->getSkillLevel(4100000)*10;
		else
			item->amount = items[item->id].maxslot + player->getSkills()->getSkillLevel(5200000)*10;
		player->getInventory()->modifyMesos(-1); // TODO: Calculate price, letting players recharge for 1 meso for now
		InventoryPacket::updateItemAmounts(player, 2, slot, item->amount, 0, 0);
		InventoryPacket::bought(player);
	}
}

void Inventory::useStorage(Player *player, ReadPacket *packet) {
	int8_t type = packet->getByte();

	if (type == 0x04) { // Take item out
		int8_t inv = packet->getByte(); // Inventory, as in equip, use, etc
		int8_t slot = packet->getByte(); // Slot within the inventory
		Item *item = player->getStorage()->getItem(slot);
		if (item == 0) // It's a trap
			return; // Abort

		addItem(player, new Item(item));
		player->getStorage()->takeItem(slot);
		StoragePacket::takeItem(player, inv);
	}

	else if (type == 0x05) { // Store item
		int16_t slot = packet->getShort();
		int32_t itemid = packet->getInt();
		int16_t amount = packet->getShort();
		if (player->getStorage()->isFull()) { // Storage is full, so tell the player and abort the mission.
			StoragePacket::storageFull(player);
			return;
		}
		int8_t inv = GETINVENTORY(itemid);
		Item *item = player->getInventory()->getItem(inv, slot);
		if (item == 0 || amount > player->getInventory()->getItemAmountBySlot(inv, slot)) // Be careful, it might be a trap.
			return; // Do a barrel roll

		if (inv == 1) // For equips we create a new object for storage with the inventory object, and allow the one in the inventory to go bye bye.
			player->getStorage()->addItem(new Item(item));
		else // For items we just create a new item based on the ID and amount.
			player->getStorage()->addItem(new Item(itemid, amount));
		takeItemSlot(player, inv, slot, amount, true);
		player->getInventory()->modifyMesos(-100); // Take 100 mesos for storage cost
		StoragePacket::addItem(player, inv);
	}

	else if (type == 0x07) { // Take out/store mesos
		int32_t mesos = packet->getInt(); // Amount of mesos to remove. Deposits are negative, and withdrawls are positive.
		bool success = player->getInventory()->modifyMesos(mesos);
		if (success)
			player->getStorage()->changeMesos(mesos);
	}
	// 0x08 is Close storage. For now we have no reason to handle this.
}

void Inventory::addNewItem(Player *player, int32_t itemid, int16_t amount) {
	if (items.find(itemid) == items.end() && equips.find(itemid) == equips.end())
		return;
	int8_t inv = GETINVENTORY(itemid);
	int16_t max = items[itemid].maxslot;
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
	if (item == 0)
		return;

	item->amount -= amount;
	if (item->amount == 0 && !ISRECHARGEABLE(item->id) || (takeStar && ISRECHARGEABLE(item->id))) {
		InventoryPacket::moveItem(player, inv, slot, 0);
		player->getInventory()->deleteItem(inv, slot);
	}
	else {
		player->getInventory()->changeItemAmount(item->id, -item->amount);
		InventoryPacket::updateItemAmounts(player, inv, slot, item->amount, 0, 0);
	}
}

void Inventory::useItem(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int16_t slot = packet->getShort();
	int32_t itemid = packet->getInt();
	if (player->getHP() == 0 || player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	takeItemSlot(player, 2, slot, 1);
	ItemInfo *item = &items[itemid];
	// Alchemist
	int16_t alchemist = 0;
	if (player->getSkills()->getSkillLevel(4110000) > 0)
		alchemist = Skills::skills[4110000][player->getSkills()->getSkillLevel(4110000)].x;
	if (item->cons.hp > 0)
		player->modifyHP(item->cons.hp + ((item->cons.hp * alchemist) / 100));
	if (item->cons.mp > 0)
		player->modifyMP(item->cons.mp + ((item->cons.mp * alchemist) / 100));
	else
		player->setMP(player->getMP(), true);
	if (item->cons.hpr > 0)
		player->modifyHP(item->cons.hpr * player->getMHP() / 100);
	if (item->cons.mpr > 0)
		player->modifyMP(item->cons.mpr * player->getMMP() / 100);
	// Item buffs
	if (item->cons.time > 0) {
		int32_t time = item->cons.time;
		if (alchemist > 0)
			time = (time * alchemist) / 100;
		InventoryPacket::useItem(player, itemid, time * 1000, item->cons.types, item->cons.vals, (item->cons.morph > 0));

		Timer::Id id(Timer::Types::ItemTimer, itemid, 0);
		player->getTimers()->removeTimer(id);
		new Timer::Timer(bind(&Inventory::endItem, player,
			itemid), id, player->getTimers(), time * 1000, false);
	}
}
// Cancel item buffs
void Inventory::cancelItem(Player *player, ReadPacket *packet) {
	int32_t itemid = packet->getInt() * -1;
	player->getTimers()->removeTimer(Timer::Id(Timer::Types::ItemTimer, itemid, 0));
	Inventory::endItem(player, itemid);
}
// End item buffs
void Inventory::endItem(Player *player, int32_t itemid) {
	InventoryPacket::endItem(player, items[itemid].cons.types, (items[itemid].cons.morph > 0));
}
// Skill books
void Inventory::useSkillbook(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int16_t slot = packet->getShort();
	int32_t itemid = packet->getInt();

	if (player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}

	int32_t skillid = 0;
	uint8_t newMaxLevel = 0;
	bool use = false;
	bool succeed = false;

	ItemInfo item = items[itemid];
	for (size_t i = 0; i < item.cons.skills.size(); i++) {
		skillid = item.cons.skills[i].skillid;
		newMaxLevel = item.cons.skills[i].maxlevel;
		if (player->getJob() == item.cons.skills[i].skillid/10000) { // Make sure the skill is for the person's job
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
void Inventory::useChair(Player *player, ReadPacket *packet) {
	int32_t chairid = packet->getInt();
	player->setChair(chairid);
	InventoryPacket::sitChair(player, chairid);
}

void Inventory::stopChair(Player *player, ReadPacket *packet) {
	player->setChair(0);
	InventoryPacket::stopChair(player);
}

bool Inventory::isCash(int32_t itemid) {
	if (equips.find(itemid) != equips.end() && equips[itemid].cash)
		return true;
	return false;
}

void Inventory::useSummonBag(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int16_t slot = packet->getShort();
	int32_t itemid = packet->getInt();
	if (player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	if (items.find(itemid) == items.end())
		return;
	takeItemSlot(player, 2, slot, 1);
	for (size_t i = 0; i < items[itemid].cons.mobs.size(); i++) {
		if (Randomizer::Instance()->randInt(100) <= items[itemid].cons.mobs[i].chance) {
			Mobs::spawnMob(player, items[itemid].cons.mobs[i].mobid);
		}
	}
}

void Inventory::useReturnScroll(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int16_t slot = packet->getShort();
	int32_t itemid = packet->getInt();
	if (player->getInventory()->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	if (items.find(itemid) == items.end())
		return;
	takeItemSlot(player, 2, slot, 1);
	int32_t map = items[itemid].cons.moveTo;
	if (map == 999999999)
		Maps::changeMap(player, Maps::maps[player->getMap()]->getInfo().rm, 0);
	else
		Maps::changeMap(player, map, 0);
}

void Inventory::useScroll(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	int16_t slot = packet->getShort();
	int16_t eslot = packet->getShort();
	int16_t wscroll = packet->getShort();
	bool legendary_spirit = (packet->getByte() != 0);

	Item *item = player->getInventory()->getItem(2, slot);
	Item *equip = player->getInventory()->getItem(1, eslot);
	if (item == 0 || equip == 0)
		return;

	int32_t itemid = item->id;
	int8_t succeed = -1;
	bool cursed = false;
	bool scrolled = false;
	if (items.find(itemid) == items.end())
		return;
	switch (itemid) {
		case 2049000: // Clean Slate 1%
		case 2049001: // Clean Slate 3%
		case 2049002: // Clean Slate 5%
		case 2049003: // Clean Slate 20%
			if ((equips[equip->id].slots - equip->scrolls) > equip->slots) {
				if ((int16_t) Randomizer::Instance()->randShort(99) < items[itemid].cons.success) { // Give back a slot
					equip->slots++;
					succeed = 1;
				}
				else {
					if ((int16_t) Randomizer::Instance()->randShort(99) < items[itemid].cons.cursed)
						cursed = true;
					succeed = 0;
				}
				scrolled = true;
			}
			break;
		case 2049100: // Chaos Scroll
			if (equip->slots > 0) {
				if ((int16_t) Randomizer::Instance()->randShort(99) < items[itemid].cons.success) { // Add stats
					int8_t n = -1; // Default - Decrease stats
					// TODO: Make sure that Chaos Scrolls are working like they do in global
					if ((int16_t) Randomizer::Instance()->randShort(99) < 50) // Increase
						n = 1;
					// Gives 0-5 stats on every stat on the item
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
					equip->slots--;
					equip->scrolls++;
					succeed = 1;
				}
				else { // Break
					cursed = true;
					succeed = 0;
				}
				scrolled = true;
			}
			break;
		case 2040727: // Shoe for Spikes 10%
			break;
		case 2041058: // Cape for Cold Protection 10%
			break;
		default: // Most scrolls
			if (equip->slots > 0) {
				if (wscroll == 2)
					takeItem(player, 2340000, 1);
				if ((int16_t) Randomizer::Instance()->randShort(99) < items[itemid].cons.success) {
					succeed = 1;
					equip->istr += items[itemid].cons.istr;
					equip->idex += items[itemid].cons.idex;
					equip->iint += items[itemid].cons.iint;
					equip->iluk += items[itemid].cons.iluk;
					equip->ihp += items[itemid].cons.ihp;
					equip->imp += items[itemid].cons.imp;
					equip->iwatk += items[itemid].cons.iwatk;
					equip->imatk += items[itemid].cons.imatk;
					equip->iwdef += items[itemid].cons.iwdef;
					equip->imdef += items[itemid].cons.imdef;
					equip->iacc += items[itemid].cons.iacc;
					equip->iavo += items[itemid].cons.iavo;
					equip->ihand += items[itemid].cons.ihand;
					equip->ijump += items[itemid].cons.ijump;
					equip->ispeed += items[itemid].cons.ispeed;
					equip->scrolls++;
					equip->slots--;
				}
				else {
					succeed = 0;
					if ((int16_t) Randomizer::Instance()->randShort(99) < items[itemid].cons.cursed)
						cursed = true;
					else if (wscroll != 2)
						equip->slots--;
				}
				scrolled = true;
			}
			break;
	}
	if (scrolled) {
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

void Inventory::useCashItem(Player *player, ReadPacket *packet) {
	int8_t type = packet->getByte();
	packet->skipBytes(1);
	int32_t itemid = packet->getInt();
	bool used = false;
	switch (itemid) {
		case 5071000: { // Megaphone
			string msg = packet->getString();
			InventoryPacket::showMegaphone(player, msg);
			used = true;
			break;
		}
		case 5072000: { // Super Megaphone
			string msg = packet->getString();
			uint8_t whisper = packet->getByte();
			InventoryPacket::showSuperMegaphone(player, msg, whisper);
			used = true;
			break;
		}
		case 5390000: // Diablo Messenger
		case 5390001: // Cloud 9 Messenger
		case 5390002: { // Loveholic Messenger
			string msg = packet->getString();
			string msg2 = packet->getString();
			string msg3 = packet->getString();
			string msg4 = packet->getString();

			InventoryPacket::showMessenger(player, msg, msg2, msg3, msg4, packet->getBuffer(), packet->getBufferLength(), itemid);
			used = true;
			break;
		}
		case 5170000: { // Pet Name Tag
			string name = packet->getString();
			Pets::changeName(player, name);
			used = true;
			break;
		}
		case 5060000: { // Item Name Tag
			int16_t slot = packet->getShort();
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
		default:
			break;
	}
	if (used)
		Inventory::takeItem(player, itemid, 1);
	else
		InventoryPacket::blankUpdate(player);
}

void Inventory::useItemEffect(Player *player, ReadPacket *packet) {
	int32_t itemid = packet->getInt();
	if (player->getInventory()->getItemAmount(itemid) == 0) {
		// hacking
		return;
	}
	player->setItemEffect(itemid);
	InventoryPacket::useItemEffect(player, itemid);
}
