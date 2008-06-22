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
		for (unsigned int i=0; i<timers.size(); i++) {
			if (player == timers[i].player && timers[i].item == item) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
	void stop(Player *player) {
		for (unsigned int i=timers.size(); i>0; i--) { // fix missing timer cancels
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
		for (unsigned int i=0; i<timers.size(); i++) {
			if (timers[i].id == id) {
				player = timers[i].player;
				item = timers[i].item;
				break;
			}
		}
		Inventory::endItem(player, item);
	}
	void remove (int id) {
		for (unsigned int i=0; i<timers.size(); i++) {
			if (timers[i].id == id) {	
				timers.erase(timers.begin()+i);
				return;
			}
		}
	}
};

vector <ItemTimer::ITimer> ItemTimer::timers;
ItemTimer * ItemTimer::singleton = 0;

void Inventory::stopTimerPlayer(Player *player) {
	ItemTimer::Instance()->stop(player);
}

void Inventory::itemMove(Player *player, ReadPacket *packet) {
	packet->skipBytes(4);
	char inv = packet->getByte();
	short slot1 = packet->getShort();
	short slot2 = packet->getShort();
	if (inv == 1) { // Equips
		if (slot2 == 0) {
			InventoryPacket::moveItem(player, inv, slot1, slot2);
			Equip *equip = player->inv->getEquip(slot1);
			if (equip == 0)
				return;
			Equip droppedequip = Equip(equip);
			Drop *drop = new Drop(player->getMap(), droppedequip, player->getPos(), player->getPlayerid());
			drop->setTime(0);
			drop->doDrop(player->getPos());
			player->inv->deleteEquip(slot1);
			Reactors::checkDrop(player, drop);
		}
		else {
			Equip *equip1 = player->inv->getEquip(slot1);
			Equip *equip2 = player->inv->getEquip(slot2);
			player->inv->addEquip(slot1, equip2);
			player->inv->addEquip(slot2, equip1);
			InventoryPacket::moveItem(player, inv, slot1, slot2);
			InventoryPacket::updatePlayer(player);
		}
	}
	else { // Items
		if (slot2 == 0) {
			short amount = packet->getShort();
			Item *item = player->inv->getItem(inv, slot1);
			if (item == 0)
				return;
			if (ISSTAR(item->id)) amount = item->amount;
			Item droppeditem = Item(item);
			droppeditem.amount = amount;
			if (item->amount - amount == 0) {
				item->amount = 0;
				InventoryPacket::moveItem(player, inv, slot1, slot2);
			}
			else {
				item->amount -= amount;
				InventoryPacket::moveItemS(player, inv, slot1, item->amount);
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

			if (item2 != 0 && !ISSTAR(item1->id) && item1->id == item2->id) {
				if (item1->amount + item2->amount <= Drops::items[item1->id].maxslot) {
					item2->amount += item1->amount;
					player->inv->deleteItem(inv, slot1);
					InventoryPacket::moveItemS(player, inv, slot2, item2->amount);
					InventoryPacket::moveItem(player, inv, slot1, 0);
				}
				else {
					item1->amount -= (Drops::items[item1->id].maxslot - item2->amount);
					item2->amount = Drops::items[item2->id].maxslot;
					InventoryPacket::moveItemS2(player, inv, slot1, item1->amount, slot2, item2->amount);
				}
			}
			else {
				player->inv->setItem(inv, slot1, item2);
				player->inv->setItem(inv, slot2, item1);
				InventoryPacket::moveItem(player, inv, slot1, slot2);
			}
		}
	}
}

Equip * Inventory::setEquipStats(Player *player, int equipid) {
	EquipInfo ei = Drops::equips[equipid];
	Equip *equip = new Equip;
	equip->id = equipid;
	equip->slots = ei.slots;
	equip->scrolls = 0;
	equip->type = ei.type;
	equip->istr = ei.istr;
	equip->idex = ei.idex;
	equip->iint = ei.iint;
	equip->iluk = ei.iluk;
	equip->ihp = ei.ihp;
	equip->imp = ei.imp;
	equip->iwatk = ei.iwatk;
	equip->imatk = ei.imatk;
	equip->iwdef = ei.iwdef;
	equip->imdef = ei.imdef;
	equip->iacc = ei.iacc;
	equip->iavo = ei.iavo;
	equip->ihand = ei.ihand;
	equip->ijump = ei.ijump;
	equip->ispeed = ei.ispeed;	
	return equip;
}

bool Inventory::addEquip(Player *player, Equip *equip, bool is) {
	short slot = 0;
	for (short s = 1; s <= player->inv->getMaxslots(1); s++) {
		if (!player->inv->getEquip(s)) {
			slot = s;
			break;
		}
	}
	if (slot != 0) {
		player->inv->addEquip(slot, equip);
		InventoryPacket::addEquip(player, slot, equip, is);
		return true;
	}
	return false;
}

short Inventory::addItem(Player *player, Item *item, bool is) {
	player->inv->changeItemAmount(item->id, item->amount);
	char inv = Drops::items[item->id].type;
	short freeslot = 0;
	if (!ISSTAR(item->id)) {
		for (short s = 1; s <= player->inv->getMaxslots(inv); s++) {
			Item *olditem = player->inv->getItem(inv, s);
			if (olditem != 0) {
				if (olditem->id == item->id && olditem->amount < Drops::items[item->id].maxslot) {
					if (item->amount + olditem->amount > Drops::items[item->id].maxslot) {
						int amount = Drops::items[item->id].maxslot - olditem->amount;
						item->amount -= amount;
						olditem->amount = Drops::items[item->id].maxslot;
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
			else if (!freeslot)
				freeslot = s;
		}
	}
	if (freeslot != 0) {
		player->inv->addItem(inv, freeslot, item);
		InventoryPacket::addNewItem(player, inv, freeslot, item, is);
		return 0;
	}
	else {
		player->inv->changeItemAmount(item->id, -item->amount);
		return item->amount;
	}
}

void Inventory::useShop(Player *player, ReadPacket *packet) {
	char type = packet->getByte();
	if (type == 0) {
		packet->skipBytes(2);
		int itemid = packet->getInt();
		short howmany = packet->getShort();
		int price = Shops::getPrice(player, itemid);
		if (price == 0) {
			// hacking
			return;
		}
		addNewItem(player, itemid, howmany);
		player->inv->setMesos(player->inv->getMesos() - price * howmany);
		InventoryPacket::bought(player);
	}
	else if (type == 1) {
		short slot = packet->getShort();
		int itemid = packet->getInt();
		short amount = packet->getShort();
		char inv = itemid/1000000;
		if (inv == 1) {
			Equip *equip = player->inv->getEquip(slot);
			if (equip == 0)
				// hacking
				return;
			InventoryPacket::moveItem(player, 1, slot, 0);
			player->inv->deleteEquip(slot);
			player->inv->setMesos(player->inv->getMesos() + Drops::equips[itemid].price * amount);
		}
		else {
			Item *item = player->inv->getItem(inv, slot);
			if (item == 0 || item->amount < amount) {
				// hacking
				return;
			}
			player->inv->setMesos(player->inv->getMesos() + Drops::items[itemid].price * amount);
			takeItemSlot(player, inv, slot, amount, true);
		}
		InventoryPacket::bought(player);
	}
	else if (type == 2) { // Recharge
		short slot = packet->getShort();
		Item *item = player->inv->getItem(2, slot);
		item->amount = Drops::items[item->id].maxslot + player->skills->getSkillLevel(4100000)*10;
		player->inv->setMesos(player->inv->getMesos() - 1); // TODO: Calculate price, letting players recharge for 1 meso for now
		InventoryPacket::moveItemS(player, 2, slot, item->amount);
		InventoryPacket::bought(player);
	}
}

void Inventory::addNewItem(Player *player, int itemid, int amount) {
	if (Drops::equips.find(itemid) != Drops::equips.end()) {
		Equip *equip = setEquipStats(player, itemid);
		addEquip(player, equip);
	}
	else {
		if (Drops::items.find(itemid) == Drops::items.end())
			return;
		char inv = Drops::items[itemid].type;
		short max = Drops::items[itemid].maxslot;
		Item *item = new Item();
		item->id = itemid;
		if (ISSTAR(itemid)) {
			item->amount = max + player->skills->getSkillLevel(4100000)*10;
			amount -= 1;
		}
		else if (amount - max > 0) {
			item->amount = max;
			amount -= max;
		}
		else {
			item->amount = amount;
			amount = 0;
		}

		if (addItem(player, item) == 0 && amount > 0)
			addNewItem(player, itemid, amount);
	}
}

void Inventory::takeItem(Player *player, int itemid, int howmany) {
	player->inv->changeItemAmount(itemid, -howmany);
	char inv = Drops::items[itemid].type;
	for (short i = 1; i <= player->inv->getMaxslots(inv); i++) {
		Item *item = player->inv->getItem(inv, i);
		if (item == 0)
			continue;
		if (item->id == itemid) {
			if (item->amount >= howmany) {
				item->amount -= howmany;
				if (item->amount == 0 && !ISSTAR(item->id)) {
					InventoryPacket::moveItem(player, inv, i, 0);
					player->inv->deleteItem(inv, i);
				}
				else
					InventoryPacket::moveItemS(player, inv, i, item->amount);
				break;
			}
			else if (!ISSTAR(item->id)) {
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
	if (item->amount == 0 && !ISSTAR(item->id) || (takeStar && ISSTAR(item->id))) {
		InventoryPacket::moveItem(player, inv, slot, 0);
		player->inv->deleteItem(inv, slot);
	}
	else {
		player->inv->changeItemAmount(item->id, -item->amount);
		InventoryPacket::moveItemS(player, inv, slot, item->amount);
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
	// Alchemist
	int alchemist = 0;
	if (player->skills->getSkillLevel(4110000) > 0) {
		alchemist = Skills::skills[4110000][player->skills->getSkillLevel(4110000)].x;
	}
	if (Drops::consumes[itemid].hp > 0) {
		player->setHP(player->getHP()+Drops::consumes[itemid].hp + ((Drops::consumes[itemid].hp * alchemist)/100));
	}
	if (Drops::consumes[itemid].mp > 0) {
		player->setMP(player->getMP()+Drops::consumes[itemid].mp + ((Drops::consumes[itemid].mp * alchemist)/100));
	}
	else
		player->setMP(player->getMP(), 1);
	if (Drops::consumes[itemid].hpr > 0) {
		player->setHP(player->getHP() + ((Drops::consumes[itemid].hpr*player->getMHP()/100) + ((Drops::consumes[itemid].hpr * alchemist)/100)));
	}
	if (Drops::consumes[itemid].mpr > 0) {
		player->setMP(player->getMP() + ((Drops::consumes[itemid].mpr*player->getMMP()/100) + ((Drops::consumes[itemid].mpr * alchemist)/100)));
	}
	// Item buffs
	if (Drops::consumes[itemid].time > 0) {
		vector <short> vals;
		unsigned char types[8];
		// Set all types to 0 initially
		types[0] = 0;
		types[1] = 0;
		types[2] = 0;
		types[3] = 0;
		types[4] = 0;
		types[5] = 0;
		types[6] = 0;
		types[7] = 0;

		if (Drops::consumes[itemid].watk > 0) {
			types[0] += 0x01;
			vals.push_back(Drops::consumes[itemid].watk);
		}
		if (Drops::consumes[itemid].wdef > 0) {
			types[0] += 0x02;
			vals.push_back(Drops::consumes[itemid].wdef);
		}
		if (Drops::consumes[itemid].matk > 0) {
			types[0] += 0x04;
			vals.push_back(Drops::consumes[itemid].matk);
		}
		if (Drops::consumes[itemid].mdef > 0) {
			types[0] += 0x08;
			vals.push_back(Drops::consumes[itemid].mdef);
		}
		if (Drops::consumes[itemid].acc > 0) {
			types[0] += 0x10;
			vals.push_back(Drops::consumes[itemid].acc);
		}
		if (Drops::consumes[itemid].avo > 0) {
			types[0] += 0x20;
			vals.push_back(Drops::consumes[itemid].avo);
		}
		if (Drops::consumes[itemid].speed > 0) {
			types[0] += 0x80;
			vals.push_back(Drops::consumes[itemid].speed);
		}
		if (Drops::consumes[itemid].jump > 0) {
			types[1] = 0x01;
			vals.push_back(Drops::consumes[itemid].jump);
		}
		bool isMorph = false;
		if (Drops::consumes[itemid].morph > 0) {
			types[4] = 0x02;
			vals.push_back(Drops::consumes[itemid].morph);
			isMorph = true;
		}
		InventoryPacket::useItem(player, Maps::maps[player->getMap()]->getPlayers(), itemid, Drops::consumes[itemid].time*1000, types, vals, isMorph);
		ItemTimer::Instance()->stop(player, itemid);
		ItemTimer::Instance()->setItemTimer(player, itemid, Drops::consumes[itemid].time*1000);
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
	unsigned char types[8];
	// Set all types to 0 initially
	types[0] = 0;
	types[1] = 0;
	types[2] = 0;
	types[3] = 0;
	types[4] = 0;
	types[5] = 0;
	types[6] = 0;
	types[7] = 0;

	if (Drops::consumes[itemid].watk > 0) {
		types[0] += 0x01;
	}
	if (Drops::consumes[itemid].wdef > 0) {
		types[0] += 0x02;
	}
	if (Drops::consumes[itemid].matk > 0) {
		types[0] += 0x04;
	}
	if (Drops::consumes[itemid].mdef > 0) {
		types[0] += 0x08;
	}
	if (Drops::consumes[itemid].acc > 0) {
		types[0] += 0x10;
	}
	if (Drops::consumes[itemid].avo > 0) {
		types[0] += 0x20;
	}
	if (Drops::consumes[itemid].speed > 0) {
		types[0] += 0x80;
	}
	if (Drops::consumes[itemid].jump > 0) {
		types[1] = 0x01;
	}
	if (Drops::consumes[itemid].morph > 0) {
		types[4] = 0x02;
	}
	InventoryPacket::endItem(player, types);
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

	for (size_t i = 0; i < Drops::consumes[itemid].skills.size(); i++) {
		skillid = Drops::consumes[itemid].skills[i].skillid;
		newMaxLevel = Drops::consumes[itemid].skills[i].maxlevel;
		if (player->getJob() == Drops::consumes[itemid].skills[i].skillid/10000) { // Make sure the skill is for the person's job
			if (player->skills->getSkillLevel(skillid) >= Drops::consumes[itemid].skills[i].reqlevel && player->skills->getMaxSkillLevel(skillid) < newMaxLevel)
				use = true;
		}
		if (use) {
			if (Randomizer::Instance()->randInt(100) <= Drops::consumes[itemid].success) {
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

	InventoryPacket::useSkillbook(player, Maps::maps[player->getMap()]->getPlayers(), skillid, newMaxLevel, use, succeed);
	if (update) {
		player->skills->addSkillLevel(skillid, 0);
	}
}
void Inventory::useChair(Player *player, ReadPacket *packet) {
	int chairid = packet->getInt();
	player->setChair(chairid);
	InventoryPacket::sitChair(player, Maps::maps[player->getMap()]->getPlayers(), chairid);
}

void Inventory::stopChair(Player *player, ReadPacket *packet) {
	player->setChair(0);
	InventoryPacket::stopChair(player, Maps::maps[player->getMap()]->getPlayers());
}

bool Inventory::isCash(int itemid) {
	if (Drops::equips.find(itemid) != Drops::equips.end() && Drops::equips[itemid].cash)
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
	if (Drops::items.find(itemid) == Drops::items.end())
		return;
	takeItemSlot(player, 2, slot, 1);
	for (size_t i = 0; i < Drops::consumes[itemid].mobs.size(); i++) {
		if (Randomizer::Instance()->randInt(100) <= Drops::consumes[itemid].mobs[i].chance) {
			Mobs::spawnMob(player, Drops::consumes[itemid].mobs[i].mobid);
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
	if (Drops::items.find(itemid) == Drops::items.end())
		return;
	takeItemSlot(player, 2, slot, 1);
	int map = Drops::consumes[itemid].moveTo;
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
	Equip *equip = player->inv->getEquip(eslot);
	if (item == 0 || equip == 0)
		return;

	int itemid = item->id;
	bool succeed = false;
	bool cursed = false;
	if (equip->slots > 0) {
		if (Drops::items.find(itemid) == Drops::items.end())
			return;
		takeItemSlot(player, 2, slot, 1);
		if (wscroll == 2)
			takeItem(player, 2340000, 1);
		if (Randomizer::Instance()->randInt(99) < Drops::consumes[itemid].success) {
			succeed = true;
			equip->istr += Drops::consumes[itemid].istr;
			equip->idex += Drops::consumes[itemid].idex;
			equip->iint += Drops::consumes[itemid].iint;
			equip->iluk += Drops::consumes[itemid].iluk;
			equip->ihp += Drops::consumes[itemid].ihp;
			equip->imp += Drops::consumes[itemid].imp;
			equip->iwatk += Drops::consumes[itemid].iwatk;
			equip->imatk += Drops::consumes[itemid].imatk;
			equip->iwdef += Drops::consumes[itemid].iwdef;
			equip->imdef += Drops::consumes[itemid].imdef;
			equip->iacc += Drops::consumes[itemid].iacc;
			equip->iavo += Drops::consumes[itemid].iavo;
			equip->ihand += Drops::consumes[itemid].ihand;
			equip->ijump += Drops::consumes[itemid].ijump;
			equip->ispeed += Drops::consumes[itemid].ispeed;
			equip->scrolls++;
			equip->slots--;
		}
		else {
			if (Randomizer::Instance()->randInt(99) < Drops::consumes[itemid].cursed) {
				cursed = true;
				InventoryPacket::moveItem(player, 1, eslot, 0);
				player->inv->deleteEquip(eslot);
			}
			else if (wscroll != 2) equip->slots--;
		}
	}
	InventoryPacket::useScroll(player, Maps::maps[player->getMap()]->getPlayers(), succeed, cursed, legendary_spirit);
	if (!cursed)
		InventoryPacket::addEquip(player, eslot, equip, true);
	InventoryPacket::updatePlayer(player);
}

void Inventory::useCashItem(Player *player, ReadPacket *packet) {
	char type = packet->getByte();
	packet->skipBytes(1);
	int itemid = packet->getInt();
	string msg = packet->getString();
	if (itemid == 5071000) { // Megaphone
		InventoryPacket::showMegaphone(player, Maps::maps[player->getMap()]->getPlayers(), msg);
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
	InventoryPacket::useItemEffect(player, Maps::maps[player->getMap()]->getPlayers(), itemid);
}
