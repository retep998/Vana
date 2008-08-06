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
#include "Player.h"
#include "InventoryPacket.h"
#include "StoragePacket.h"
#include "Skills.h"
#include "Drops.h"
#include "Shops.h"
#include "Quests.h"
#include "Reactors.h"
#include "Maps.h"
#include "Mobs.h"
#include "Randomizer.h"
#include "Timer.h"
#include "ReadPacket.h"
#include <cmath>

class ItemTimer: public Timer::TimerHandler {
public:
	static ItemTimer * Instance() {
		if (singleton == 0)
			singleton = new ItemTimer;
		return singleton;
	}
	void setItemTimer(Player *player, int item, int time) {
		ITimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.player = player;
		timer.item = item;
		timer.time = time;
		timers.push_back(timer);
	}
	void stop(Player *player, int item) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (player == timers[i].player && timers[i].item == item) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	void stop(Player *player) {
		for (size_t i = timers.size(); i > 0; i--) { // fix missing timer cancels
			if (player == timers[i-1].player) {
				Timer::Instance()->cancelTimer(timers[i-1].id);
			}
		}
	}
private:
	static ItemTimer *singleton;
	ItemTimer() {};
	ItemTimer(const ItemTimer&);
	ItemTimer& operator=(const ItemTimer&);

	struct ITimer {
		int id;
		Player *player;
		int item;
		int time;
	};
	static vector <ITimer> timers;
	void handle(Timer *timer, int id) {
		int item;
		Player *player;
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				player = timers[i].player;
				item = timers[i].item;
				break;
			}
		}
		Inventory::endItem(player, item);
	}
	void remove (int id) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {	
				timers.erase(timers.begin()+i);
				return;
			}
		}
	}
};

vector <ItemTimer::ITimer> ItemTimer::timers;
ItemTimer * ItemTimer::singleton = 0;

/* Inventory Namespace */
hash_map <int, EquipInfo> Inventory::equips;
hash_map <int, ItemInfo> Inventory::items;

void Inventory::addEquipInfo(int id, EquipInfo equip) {
	equips[id] = equip;
}

void Inventory::addItemInfo(int id, ItemInfo item) {
	if (ISRECHARGEABLE(id))
		Shops::rechargables.push_back(id);
	// Set all types to 0 initially
	memset(item.cons.types, 0, sizeof(item.cons.types));

	if (item.cons.watk > 0) {
		item.cons.types[TYPE_1 - 1] += 0x01;
		item.cons.vals.push_back(item.cons.watk);
	}
	if (item.cons.wdef > 0) {
		item.cons.types[TYPE_1 - 1] += 0x02;
		item.cons.vals.push_back(item.cons.wdef);
	}
	if (item.cons.matk > 0) {
		item.cons.types[TYPE_1 - 1] += 0x04;
		item.cons.vals.push_back(item.cons.matk);
	}
	if (item.cons.mdef > 0) {
		item.cons.types[TYPE_1 - 1] += 0x08;
		item.cons.vals.push_back(item.cons.mdef);
	}
	if (item.cons.acc > 0) {
		item.cons.types[TYPE_1 - 1] += 0x10;
		item.cons.vals.push_back(item.cons.acc);
	}
	if (item.cons.avo > 0) {
		item.cons.types[TYPE_1 - 1] += 0x20;
		item.cons.vals.push_back(item.cons.avo);
	}
	if (item.cons.speed > 0) {
		item.cons.types[TYPE_1 - 1] += 0x80;
		item.cons.vals.push_back(item.cons.speed);
	}
	if (item.cons.jump > 0) {
		item.cons.types[TYPE_2 - 1] = 0x01;
		item.cons.vals.push_back(item.cons.jump);
	}
	if (item.cons.morph > 0) {
		item.cons.types[TYPE_5 - 1] = 0x02;
		item.cons.vals.push_back(item.cons.morph);
	}

	items[id] = item;
}

void Inventory::stopTimersPlayer(Player *player) {
	ItemTimer::Instance()->stop(player);
}

void Inventory::itemMove(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	char inv = packet->getByte();
	short slot1 = packet->getShort();
	short slot2 = packet->getShort();

	if (slot2 == 0) {
		short amount = packet->getShort();
		Item *item = player->inv->getItem(inv, slot1);
		if (item == 0)
			return;
		if (ISRECHARGEABLE(item->id)) amount = item->amount;
		Item droppeditem = Item(item);
		droppeditem.amount = amount;
		if (item->amount - amount == 0) {
			item->amount = 0;
			InventoryPacket::moveItem(player, inv, slot1, slot2);
		}
		else {
			item->amount -= amount;
			InventoryPacket::updateItemAmounts(player, inv, slot1, item->amount, 0, 0);
		}
		Drop *drop = new Drop(player->getMap(), droppeditem, player->getPos(), player->getPlayerid());
		drop->setTime(0);
		drop->doDrop(player->getPos());
		if (item->amount == 0)
			player->inv->deleteItem(inv, slot1);
		Reactors::checkDrop(player, drop);
	}
	else {
		Item *item1 = player->inv->getItem(inv, slot1);
		Item *item2 = player->inv->getItem(inv, slot2);

		if (item2 != 0 && !ISRECHARGEABLE(item1->id) && !ISEQUIP(item1->id) && item1->id == item2->id) {
			if (item1->amount + item2->amount <= items[item1->id].maxslot) {
				item2->amount += item1->amount;
				player->inv->deleteItem(inv, slot1);
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
			player->inv->setItem(inv, slot1, item2);
			player->inv->setItem(inv, slot2, item1);
			InventoryPacket::moveItem(player, inv, slot1, slot2);
		}
	}
	if (slot1 < 0 || slot2 < 0)
		InventoryPacket::updatePlayer(player);
}

short Inventory::addItem(Player *player, Item *item, bool is) {
	char inv = GETINVENTORY(item->id);
	short freeslot = 0;
	for (short s = 1; s <= player->inv->getMaxSlots(inv); s++) {
		Item *olditem = player->inv->getItem(inv, s);
		if (olditem != 0) {
			if (!ISRECHARGEABLE(item->id) && !ISEQUIP(item->id) && olditem->id == item->id && olditem->amount < items[item->id].maxslot) {
				if (item->amount + olditem->amount > items[item->id].maxslot) {
					short amount = items[item->id].maxslot - olditem->amount;
					item->amount -= amount;
					olditem->amount = items[item->id].maxslot;
					InventoryPacket::addItem(player, inv, s, olditem, is);
				}
				else {
					item->amount += olditem->amount;
					player->inv->deleteItem(inv, s);
					player->inv->addItem(inv, s, item);
					InventoryPacket::addItem(player, inv, s, item, is);
					return 0;
				}
			}
		}
		else if (!freeslot) {
			freeslot = s;
			if (ISRECHARGEABLE(item->id) || ISEQUIP(item->id))
				break;
		}
	}
	if (freeslot != 0) {
		player->inv->addItem(inv, freeslot, item);
		InventoryPacket::addNewItem(player, inv, freeslot, item, is);
		return 0;
	}
	else {
		return item->amount;
	}
}

void Inventory::useShop(Player *player, ReadPacket *packet) {
	char type = packet->getByte();
	if (type == 0) { // Buy
		packet->skipBytes(2);
		int itemid = packet->getInt();
		short howmany = packet->getShort();
		int price = Shops::getPrice(player->getShop(), itemid);
		if (price == 0) {
			// hacking
			return;
		}
		addNewItem(player, itemid, howmany);
		player->inv->setMesos(player->inv->getMesos() - price * howmany);
		InventoryPacket::bought(player);
	}
	else if (type == 1) { // Sell
		short slot = packet->getShort();
		int itemid = packet->getInt();
		short amount = packet->getShort();
		char inv = GETINVENTORY(itemid);
		Item *item = player->inv->getItem(inv, slot);
		if (item == 0 || item->amount < amount) {
			// hacking
			return;
		}
		int price = 0;
		if (inv == 1)
			price = equips[itemid].price;
		else
			price = items[itemid].price;
		player->inv->setMesos(player->inv->getMesos() + price * amount);
		takeItemSlot(player, inv, slot, amount, true);
		InventoryPacket::bought(player);
	}
	else if (type == 2) { // Recharge
		short slot = packet->getShort();
		Item *item = player->inv->getItem(2, slot);
		if (ISSTAR(item->id))
			item->amount = items[item->id].maxslot + player->skills->getSkillLevel(4100000)*10;
		else
			item->amount = items[item->id].maxslot + player->skills->getSkillLevel(5200000)*10;
		player->inv->setMesos(player->inv->getMesos() - 1); // TODO: Calculate price, letting players recharge for 1 meso for now
		InventoryPacket::updateItemAmounts(player, 2, slot, item->amount, 0, 0);
		InventoryPacket::bought(player);
	}
}

void Inventory::useStorage(Player *player, ReadPacket *packet) {
	char type = packet->getByte();

	if (type == 0x04) { // Take item out
		char itemtype = packet->getByte();
		char slot = packet->getByte();
		Item *item = player->storage->getItem(slot);
		if (item == 0) // It's a trap
			return; // Abort

		char inv = GETINVENTORY(item->id);
		addItem(player, new Item(item));
		player->storage->takeItem(slot);
		StoragePacket::takeItem(player, inv, slot, itemtype);
	}

	else if (type == 0x05) { // Store item
		short slot = packet->getShort();
		int itemid = packet->getInt();
		short amount = packet->getShort();
		if (player->storage->isFull()) { // Storage is full, so tell the player and abort the mission.
			StoragePacket::storageFull(player);
			return;
		}
		char inv = GETINVENTORY(itemid);
		Item *item = player->inv->getItem(inv, slot);
		if (item == 0 || amount > player->inv->getItemAmountBySlot(inv, slot)) // Be careful, it might be a trap.
			return; // Do a barrel roll

		if (inv == 1) // For equips we create a new object for storage with the inventory object, and allow the one in the inventory to go bye bye.
			player->storage->addItem(new Item(item));
		else // For items we just create a new item based on the ID and amount.
			player->storage->addItem(new Item(itemid, amount));
		takeItemSlot(player, inv, slot, amount, true);
		player->inv->setMesos(player->inv->getMesos() - 100); // Take 100 mesos for storage cost
		StoragePacket::addItem(player, inv);
	}

	else if (type == 0x07) { // Take out/store mesos
		int mesos = packet->getInt(); // Amount of mesos to remove. Deposits are negative, and withdrawls are positive.
		player->storage->changeMesos(mesos);
		player->inv->setMesos(player->inv->getMesos() + mesos);
	}
	// 0x08 is Close storage. For now we have no reason to handle this.
}

void Inventory::addNewItem(Player *player, int itemid, int amount) {
	if (items.find(itemid) == items.end() && equips.find(itemid) == equips.end())
		return;
	char inv = GETINVENTORY(itemid);
	short max = items[itemid].maxslot;
	short thisamount = 0;
	if (ISSTAR(itemid)) {
		thisamount = max + player->skills->getSkillLevel(4100000)*10;
		amount -= 1;
	}
	else if (ISBULLET(itemid)) {
		thisamount = max + player->skills->getSkillLevel(5200000)*10;
		amount -= 1;
	}
	else if (ISEQUIP(itemid)) {
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

	if (addItem(player, item) == 0 && amount > 0)
		addNewItem(player, itemid, amount);
}

void Inventory::takeItem(Player *player, int itemid, int howmany) {
	player->inv->changeItemAmount(itemid, -howmany);
	char inv = GETINVENTORY(itemid);
	for (short i = 1; i <= player->inv->getMaxSlots(inv); i++) {
		Item *item = player->inv->getItem(inv, i);
		if (item == 0)
			continue;
		if (item->id == itemid) {
			if (item->amount >= howmany) {
				item->amount -= howmany;
				if (item->amount == 0 && !ISRECHARGEABLE(item->id)) {
					InventoryPacket::moveItem(player, inv, i, 0);
					player->inv->deleteItem(inv, i);
				}
				else {
					InventoryPacket::updateItemAmounts(player, inv, i, item->amount, 0, 0);
					player->inv->changeItemAmount(item->id, -howmany);
				}
				break;
			}
			else if (!ISRECHARGEABLE(item->id)) {
				howmany -= item->amount;
				item->amount = 0;
				InventoryPacket::moveItem(player, inv, i, 0);
				player->inv->deleteItem(inv, i);
			}
		}
	}
}

void Inventory::takeItemSlot(Player *player, char inv, short slot, short amount, bool takeStar) {
	Item *item = player->inv->getItem(inv, slot);
	if (item == 0)
		return;

	item->amount -= amount;
	if (item->amount == 0 && !ISRECHARGEABLE(item->id) || (takeStar && ISRECHARGEABLE(item->id))) {
		InventoryPacket::moveItem(player, inv, slot, 0);
		player->inv->deleteItem(inv, slot);
	}
	else {
		player->inv->changeItemAmount(item->id, -item->amount);
		InventoryPacket::updateItemAmounts(player, inv, slot, item->amount, 0, 0);
	}
}

void Inventory::useItem(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short slot = packet->getShort();
	int itemid = packet->getInt();
	if (player->inv->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	takeItemSlot(player, 2, slot, 1);
	ItemInfo *item = &items[itemid];
	// Alchemist
	short alchemist = 0;
	if (player->skills->getSkillLevel(4110000) > 0) {
		alchemist = Skills::skills[4110000][player->skills->getSkillLevel(4110000)].x;
	}
	if (item->cons.hp > 0) {
		player->setHP(player->getHP() + item->cons.hp + ((item->cons.hp * alchemist) / 100));
	}
	if (item->cons.mp > 0) {
		player->setMP(player->getMP() + item->cons.mp + ((item->cons.mp * alchemist) / 100));
	}
	else
		player->setMP(player->getMP(), true);
	if (item->cons.hpr > 0) {
		player->setHP(player->getHP() + (((item->cons.hpr + alchemist) * player->getMHP() / 100)));
	}
	if (item->cons.mpr > 0) {
		player->setMP(player->getMP() + (((item->cons.mpr + alchemist) * player->getMMP() / 100)));
	}
	// Item buffs
	if (item->cons.time > 0) {
		InventoryPacket::useItem(player, itemid, item->cons.time * 1000, item->cons.types, item->cons.vals, (item->cons.morph > 0));
		ItemTimer::Instance()->stop(player, itemid);
		ItemTimer::Instance()->setItemTimer(player, itemid, item->cons.time * 1000);
	}
}
// Cancel item buffs
void Inventory::cancelItem(Player *player, ReadPacket *packet) {
	int itemid = packet->getInt()*-1;
	ItemTimer::Instance()->stop(player, itemid);
	Inventory::endItem(player, itemid);
}
// End item buffs
void Inventory::endItem(Player *player, int itemid) {
	InventoryPacket::endItem(player, items[itemid].cons.types, (items[itemid].cons.morph > 0));
}
// Skill books
void Inventory::useSkillbook(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short slot = packet->getShort();
	int itemid = packet->getInt();

	if (player->inv->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}

	int skillid = 0;
	int newMaxLevel = 0;
	bool use = false;
	bool succeed = false;
	bool update = false;

	ItemInfo item = items[itemid];
	for (size_t i = 0; i < item.cons.skills.size(); i++) {
		skillid = item.cons.skills[i].skillid;
		newMaxLevel = item.cons.skills[i].maxlevel;
		if (player->getJob() == item.cons.skills[i].skillid/10000) { // Make sure the skill is for the person's job
			if (player->skills->getSkillLevel(skillid) >= item.cons.skills[i].reqlevel && player->skills->getMaxSkillLevel(skillid) < newMaxLevel)
				use = true;
		}
		if (use) {
			if (Randomizer::Instance()->randInt(100) <= item.cons.success) {
				if (player->skills->getSkillLevel(skillid) == player->skills->getMaxSkillLevel(skillid)) {
					update = true;
				}
				player->skills->setMaxSkillLevel(skillid, newMaxLevel);
				succeed = true;
			}
			takeItemSlot(player, 2, slot, 1);
			break;
		}
	}

	if (skillid == 0) return;

	InventoryPacket::useSkillbook(player, skillid, newMaxLevel, use, succeed);
	if (update) {
		player->skills->addSkillLevel(skillid, 0);
	}
}
void Inventory::useChair(Player *player, ReadPacket *packet) {
	int chairid = packet->getInt();
	player->setChair(chairid);
	InventoryPacket::sitChair(player, chairid);
}

void Inventory::stopChair(Player *player, ReadPacket *packet) {
	player->setChair(0);
	InventoryPacket::stopChair(player);
}

bool Inventory::isCash(int itemid) {
	if (equips.find(itemid) != equips.end() && equips[itemid].cash)
		return true;
	return false;
}

void Inventory::useSummonBag(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short slot = packet->getShort();
	int itemid = packet->getInt();
	if (player->inv->getItemAmountBySlot(2, slot) == 0) {
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
	short slot = packet->getShort();
	int itemid = packet->getInt();
	if (player->inv->getItemAmountBySlot(2, slot) == 0) {
		// hacking
		return;
	}
	if (items.find(itemid) == items.end())
		return;
	takeItemSlot(player, 2, slot, 1);
	int map = items[itemid].cons.moveTo;
	if (map == 999999999)
		Maps::changeMap(player, Maps::maps[player->getMap()]->getInfo().rm, 0);
	else
		Maps::changeMap(player, map, 0);
}

void Inventory::useScroll(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	short slot = packet->getShort();
	short eslot = packet->getShort();
	short wscroll = packet->getShort();
	bool legendary_spirit = (packet->getByte() != 0);

	Item *item = player->inv->getItem(2, slot);
	Item *equip = player->inv->getItem(1, eslot);
	if (item == 0 || equip == 0)
		return;

	int itemid = item->id;
	bool succeed = false;
	bool cursed = false;
	bool scrolled = false;
	if (items.find(itemid) == items.end())
		return;
	switch (itemid) {
		case 2049000: // Clean Slate 1%
		case 2049001: // Clean Slate 3%
		case 2049002: // Clean Slate 5%
			if ((equips[equip->id].slots - equip->scrolls) > equip->slots) {
				if (Randomizer::Instance()->randInt(99) < items[itemid].cons.success) { // Give back a slot
					equip->slots++;
					succeed = true;
				}
				else {
					if (Randomizer::Instance()->randInt(99) < items[itemid].cons.cursed) {
						cursed = true;
						InventoryPacket::moveItem(player, 1, eslot, 0);
						player->inv->deleteItem(1, eslot);
					}
				}
				scrolled = true;
			}
			break;
		case 2049100: // Chaos Scroll
			if (equip->slots > 0) {
				if (Randomizer::Instance()->randInt(99) < items[itemid].cons.success) { // Add stats
					char n = -1; // Default - Decrease stats
					// TODO: Make sure that Chaos Scrolls are working like they do in global
					if (Randomizer::Instance()->randInt(99) < 50) // Increase
						n = 1;
					// Gives 0-5 stats on every stat on the item
					if (equip->istr > 0)
						equip->istr += Randomizer::Instance()->randInt(5) * n;
					if (equip->idex > 0)
						equip->idex += Randomizer::Instance()->randInt(5) * n;
					if (equip->iint > 0)
						equip->iint += Randomizer::Instance()->randInt(5) * n;
					if (equip->iluk > 0)
						equip->iluk += Randomizer::Instance()->randInt(5) * n;
					if (equip->iavo > 0)
						equip->iavo += Randomizer::Instance()->randInt(5) * n;
					if (equip->iacc > 0)
						equip->iacc += Randomizer::Instance()->randInt(5) * n;
					if (equip->ihand > 0)
						equip->ihand += Randomizer::Instance()->randInt(5) * n;
					if (equip->ijump > 0)
						equip->ijump += Randomizer::Instance()->randInt(5) * n;
					if (equip->ispeed > 0)
						equip->ispeed += Randomizer::Instance()->randInt(5) * n;
					if (equip->imatk > 0)
						equip->imatk += Randomizer::Instance()->randInt(5) * n;
					if (equip->iwatk > 0)
						equip->iwatk += Randomizer::Instance()->randInt(5) * n;
					if (equip->imdef > 0)
						equip->imdef += Randomizer::Instance()->randInt(5) * n;
					if (equip->iwdef > 0)
						equip->iwdef += Randomizer::Instance()->randInt(5) * n;
					if (equip->ihp > 0)
						equip->ihp += Randomizer::Instance()->randInt(5) * n;
					if (equip->imp > 0)
						equip->imp += Randomizer::Instance()->randInt(5) * n;
					equip->slots--;
					equip->scrolls++;
					succeed = true;
				}
				else { // Break
					cursed = true;
					InventoryPacket::moveItem(player, 1, eslot, 0);
					player->inv->deleteItem(1, eslot);
				}
				scrolled = true;
			}
			break;
		default: // Most scrolls
			if (equip->slots > 0) {
				if (wscroll == 2)
					takeItem(player, 2340000, 1);
				if (Randomizer::Instance()->randInt(99) < items[itemid].cons.success) {
					succeed = true;
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
					if (Randomizer::Instance()->randInt(99) < items[itemid].cons.cursed) {
						cursed = true;
						InventoryPacket::moveItem(player, 1, eslot, 0);
						player->inv->deleteItem(1, eslot);
					}
					else if (wscroll != 2) equip->slots--;
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
		InventoryPacket::updatePlayer(player);
	}
	else 
		InventoryPacket::blankUpdate(player);
}

void Inventory::useCashItem(Player *player, ReadPacket *packet) {
	char type = packet->getByte();
	packet->skipBytes(1);
	int itemid = packet->getInt();
	string msg = packet->getString();
	if (itemid == 5071000) { // Megaphone
		InventoryPacket::showMegaphone(player, msg);
	}
	else if (itemid == 5072000) { // Super Megaphone
		int whisper = packet->getByte();
		InventoryPacket::showSuperMegaphone(player, msg, whisper);
	}
	else if (itemid/10000 == 539) { // Messenger
		string msg2 = packet->getString();
		string msg3 = packet->getString();
		string msg4 = packet->getString();

		InventoryPacket::showMessenger(player, msg, msg2, msg3, msg4, packet->getBuffer(), packet->getBufferLength(), itemid);
	}
	Inventory::takeItem(player, itemid, 1);
}

void Inventory::useItemEffect(Player *player, ReadPacket *packet) {
	int itemid = packet->getInt();
	if (player->inv->getItemAmount(itemid) == 0) {
		// hacking
		return;
	}
	player->setItemEffect(itemid);
	InventoryPacket::useItemEffect(player, itemid);
}
