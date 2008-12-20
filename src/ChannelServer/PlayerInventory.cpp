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
#include "PlayerInventory.h"
#include "Database.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "ItemDataProvider.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayerPacketHelper.h"
#include "Randomizer.h"

/* Item struct */
Item::Item(int32_t equipid, bool random) : id(equipid), amount(1), scrolls(0), petid(0), name("") {
	EquipInfo ei = ItemDataProvider::Instance()->getEquipInfo(equipid);
	slots = ei.slots;
	if (!random) {
		istr = ei.istr;
		idex = ei.idex;
		iint = ei.iint;
		iluk = ei.iluk;
		ihp = ei.ihp;
		imp = ei.imp;
		iwatk = ei.iwatk;
		imatk = ei.imatk;
		iwdef = ei.iwdef;
		imdef = ei.imdef;
		iacc = ei.iacc;
		iavo = ei.iavo;
		ihand = ei.ihand;
		ijump = ei.ijump;
		ispeed = ei.ispeed;
	}
	else {
		istr = ei.istr > 0 ? ei.istr + Randomizer::Instance()->randShort(2)-1 : 0;
		idex = ei.idex > 0 ? ei.idex + Randomizer::Instance()->randShort(2)-1 : 0;
		iint = ei.iint > 0 ? ei.iint + Randomizer::Instance()->randShort(2)-1 : 0;
		iluk = ei.iluk > 0 ? ei.iluk + Randomizer::Instance()->randShort(2)-1 : 0;
		ihp = ei.ihp > 0 ? ei.ihp + Randomizer::Instance()->randShort(10)-5 : 0;
		imp = ei.imp > 0 ? ei.imp + Randomizer::Instance()->randShort(10)-5 : 0;
		iwatk = ei.iwatk > 0 ? ei.iwatk + Randomizer::Instance()->randShort(10)-5 : 0;
		imatk = ei.imatk > 0 ? ei.imatk + Randomizer::Instance()->randShort(10)-5 : 0;
		iwdef = ei.iwdef > 0 ? ei.iwdef + Randomizer::Instance()->randShort(10)-5 : 0;
		imdef = ei.imdef > 0 ? ei.imdef + Randomizer::Instance()->randShort(10)-5 : 0;
		iacc = ei.iacc > 0 ? ei.iacc + Randomizer::Instance()->randShort(2)-1 : 0;
		iavo = ei.iavo > 0 ? ei.iavo + Randomizer::Instance()->randShort(2)-1 : 0;
		ihand = ei.ihand;
		ijump = ei.ijump > 0 ? ei.ijump + Randomizer::Instance()->randShort(2)-1 : 0;
		ispeed = ei.ispeed > 0 ? ei.ispeed + Randomizer::Instance()->randShort(2)-1 : 0;
	}
}

/* PlayerInventory class */
PlayerInventory::PlayerInventory(Player *player, uint8_t maxslots[5], int32_t mesos) : player(player), mesos(mesos) {
	memcpy(this->maxslots, maxslots, sizeof(this->maxslots));
	memset(this->equipped, 0, sizeof(this->equipped));
	load();
}

void PlayerInventory::addMaxSlots(int8_t inventory, int8_t rows) { // Useful with .lua
	inventory -= 1;
	maxslots[inventory] += (rows * 4);
	if (maxslots[inventory] > 100)
		maxslots[inventory] = 100;
	if (maxslots[inventory] < 24) // Retard.
		maxslots[inventory] = 24;
	InventoryPacket::updateSlots(player, inventory + 1, maxslots[inventory]);
}

void PlayerInventory::setMesos(int32_t mesos, bool is) {
	if (mesos < 0)
		mesos = 0;
	this->mesos = mesos;
	PlayerPacket::updateStatInt(player, 0x40000, mesos, is);
}

bool PlayerInventory::modifyMesos(int32_t mod, bool is) {
	bool negative = mod < 0;
	if (negative && (mesos + mod) < 0)
		mesos = 0;
	else {
		int32_t mesotest = mesos + mod;
		if (!negative && mesotest < 0) // Refuse to modify mesos when it would put you over the cap
			return false;
		mesos = mesotest;
	}
	PlayerPacket::updateStatInt(player, 0x40000, mesos, is);
	return true;
}

void PlayerInventory::addItem(int8_t inv, int16_t slot, Item *item) {
	items[inv-1][slot] = item;
	if (itemamounts.find(item->id) != itemamounts.end())
		itemamounts[item->id] += item->amount;
	else
		itemamounts[item->id] = item->amount;
	if (slot < 0)
		addEquipped(slot, item->id);
}

Item * PlayerInventory::getItem(int8_t inv, int16_t slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot];
	return 0;
}

void PlayerInventory::deleteItem(int8_t inv, int16_t slot, bool updateAmount) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end()) {
		if (updateAmount)
			itemamounts[items[inv][slot]->id] -= items[inv][slot]->amount;
		if (slot < 0)
			addEquipped(slot, 0);
		delete items[inv][slot];
		items[inv].erase(slot);
	}
}

void PlayerInventory::setItem(int8_t inv, int16_t slot, Item *item) {
	inv -= 1;
	if (item == 0) {
		items[inv].erase(slot);
		if (slot < 0)
			addEquipped(slot, 0);
	}
	else {
		items[inv][slot] = item;
		if (slot < 0)
			addEquipped(slot, item->id);
	}
}

int16_t PlayerInventory::getItemAmountBySlot(int8_t inv, int16_t slot) {
	inv -= 1;
	return items[inv].find(slot) != items[inv].end() ? items[inv][slot]->amount : 0;
}

void PlayerInventory::addEquipped(int16_t slot, int32_t itemid) {
	slot = abs(slot);

	if (slot > 100) // Cash items
		equipped[slot - 100][1] = itemid;
	else // Normal items
		equipped[slot][0] = itemid;
}

int32_t PlayerInventory::getEquippedID(int16_t slot) {
	return equipped[slot][0];
}

void PlayerInventory::addEquippedPacket(PacketCreator &packet) {
	for (int8_t i = 0; i < 50; i++) { // Shown items
		if (equipped[i][0] > 0 || equipped[i][1] > 0) {
			packet.addByte(i);
			if (equipped[i][1] <= 0 || (i == 11 && equipped[i][0] > 0)) // Normal weapons always here
				packet.addInt(equipped[i][0]);
			else
				packet.addInt(equipped[i][1]);
		}
	}
	packet.addByte(-1);
	for (int8_t i = 0; i < 50; i++) { // Covered items
		if (equipped[i][1] > 0 && equipped[i][0] > 0 && i != 11) {
			packet.addByte(i);
			packet.addInt(equipped[i][0]);
		}
	}
	packet.addByte(-1);
	packet.addInt(equipped[11][1]); // Cash weapon
}

uint16_t PlayerInventory::getItemAmount(int32_t itemid) {
	return itemamounts.find(itemid) != itemamounts.end() ? itemamounts[itemid] : 0;
}

bool PlayerInventory::hasOpenSlotsFor(int32_t itemid, int16_t amount, bool canStack) {
	int16_t incrementor = 0;
	int16_t required = 0;
	int8_t inv = GETINVENTORY(itemid);
	if (inv == 1 || ISRECHARGEABLE(itemid))
		required = amount; // These aren't stackable
	else {
		int16_t maxslot = ItemDataProvider::Instance()->getMaxslot(itemid);
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
		if (getItem(inv, i) == 0)
			openslots ++;
	}
	return openslots;
}

void PlayerInventory::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT inv, slot, itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, petid, items.name, pets.index, pets.name, pets.level, pets.closeness, pets.fullness FROM items LEFT JOIN pets ON items.petid=pets.id WHERE charid = " << player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		Item *item = new Item;
		item->id = res[i][2];
		item->amount = res[i][3];
		item->slots = (uint8_t) res[i][4];
		item->scrolls = (uint8_t) res[i][5];
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
		item->petid = res[i][21];
		res[i][22].to_string(item->name);
		addItem((uint8_t) res[i][0], res[i][1], item);
		if (item->petid != 0) {
			Pet *pet = new Pet(
				player,
				item, // Item - Gives id and type to pet
				(int8_t) res[i][23], // Index
				(string) res[i][24], // Name
				(uint8_t) res[i][25], // Level
				(int16_t) res[i][26], // Closeness
				(uint8_t) res[i][27], // Fullness
				(uint8_t) res[i][1] // Inventory Slot
			);
			player->getPets()->addPet(pet);
		}
	}
}

void PlayerInventory::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "DELETE FROM items WHERE charid = " << player->getId();
	query.exec();

	bool firstrun = true;
	for (int8_t i = 1; i <= 5; i++) {
		iteminventory &itemsinv = items[i - 1];
		for (iteminventory::iterator iter = itemsinv.begin(); iter != itemsinv.end(); iter++) {
			Item *item = iter->second;
			if (firstrun) {
				query << "INSERT INTO items VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << player->getId() << ","
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
				<< item->petid << ","
				<< mysqlpp::quote << item->name << ")";
		}
	}
	if (!firstrun)
		query.exec();
}

void PlayerInventory::connectData(PacketCreator &packet) {
	packet.addInt(mesos);
	packet.addByte(getMaxSlots(1));
	packet.addByte(getMaxSlots(2));
	packet.addByte(getMaxSlots(3));
	packet.addByte(getMaxSlots(4));
	packet.addByte(getMaxSlots(5));
	iteminventory &equips = items[0];
	for (iteminventory::iterator iter = equips.begin(); iter != equips.end(); iter++) {
		if (iter->first < 0 && iter->first > -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.addByte(0);
	for (iteminventory::iterator iter = equips.begin(); iter != equips.end(); iter++) {
		if (iter->first < -100) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.addByte(0);
	for (iteminventory::iterator iter = equips.begin(); iter != equips.end(); iter++) {
		if (iter->first > 0) {
			PlayerPacketHelper::addItemInfo(packet, iter->first, iter->second);
		}
	}
	packet.addByte(0);
	for (int8_t i = 2; i <= 5; i++) {
		for (int16_t s = 1; s <= getMaxSlots(i); s++) {
			Item *item = getItem(i, s);
			if (item == 0)
				continue;
			if (item->petid == 0) {
				PlayerPacketHelper::addItemInfo(packet, s, item);
			}
			else {
				Pet *pet = player->getPets()->getPet(item->petid);
				packet.addByte((int8_t) s);
				packet.addByte(3);
				packet.addInt(item->id);
				packet.addByte(1);
				packet.addInt(pet->getId());
				packet.addInt(0);
				packet.addBytes("008005BB46E61702");
				packet.addString(pet->getName(), 13);
				packet.addByte(pet->getLevel());
				packet.addShort(pet->getCloseness());
				packet.addByte(pet->getFullness());
				packet.addByte(0);
				packet.addBytes("B8D56000CEC8"); // Most likely has expire date in it in korean time stamp
				packet.addByte(1); // Propapbly is it alive (1 Alive, 2 Dead)
				packet.addInt(0);
			}
		}
		packet.addByte(0);
	}
}

int32_t PlayerInventory::doShadowClaw() {
	for (int16_t s = 1; s <= getMaxSlots(2); s++) {
		Item *item = getItem(2, s);
		if (item == 0)
			continue;
		if (ISSTAR(item->id) && item->amount >= 200) {
			Inventory::takeItemSlot(player, 2, s, 200);
			return item->id;
		}
	}
	return 0;
}
