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
#include "Inventory.h"
#include "InventoryPacket.h"
#include "PacketCreator.h"
#include "Pets.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Randomizer.h"
#include "MySQLM.h"

/* Item struct */
Item::Item(int equipid, bool random) : id(equipid), amount(1), scrolls(0), petid(0), name("") {
	EquipInfo ei = Inventory::equips[equipid];
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
		istr = ei.istr > 0 ? ei.istr + Randomizer::Instance()->randInt(2)-1 : 0;
		idex = ei.idex > 0 ? ei.idex + Randomizer::Instance()->randInt(2)-1 : 0;
		iint = ei.iint > 0 ? ei.iint + Randomizer::Instance()->randInt(2)-1 : 0;
		iluk = ei.iluk > 0 ? ei.iluk + Randomizer::Instance()->randInt(2)-1 : 0;
		ihp = ei.ihp > 0 ? ei.ihp + Randomizer::Instance()->randInt(10)-5 : 0;
		imp = ei.imp > 0 ? ei.imp + Randomizer::Instance()->randInt(10)-5 : 0;
		iwatk = ei.iwatk > 0 ? ei.iwatk + Randomizer::Instance()->randInt(10)-5 : 0;
		imatk = ei.imatk > 0 ? ei.imatk + Randomizer::Instance()->randInt(10)-5 : 0;
		iwdef = ei.iwdef > 0 ? ei.iwdef + Randomizer::Instance()->randInt(10)-5 : 0;
		imdef = ei.imdef > 0 ? ei.imdef + Randomizer::Instance()->randInt(10)-5 : 0;
		iacc = ei.iacc > 0 ? ei.iacc + Randomizer::Instance()->randInt(2)-1 : 0;
		iavo = ei.iavo > 0 ? ei.iavo + Randomizer::Instance()->randInt(2)-1 : 0;
		ihand = ei.ihand;
		ijump = ei.ijump > 0 ? ei.ijump + Randomizer::Instance()->randInt(2)-1 : 0;
		ispeed = ei.ispeed > 0 ? ei.ispeed + Randomizer::Instance()->randInt(2)-1 : 0;
	}
}

/* PlayerInventory class */
PlayerInventory::PlayerInventory(Player *player, unsigned char maxslots[5],
								 int mesos) :
player(player),
mesos(mesos)
{
	memcpy_s(this->maxslots, sizeof(this->maxslots), maxslots, sizeof(this->maxslots));
	memset(this->equipped, 0, sizeof(this->equipped));
	load();
}

void PlayerInventory::addMaxSlots(char inventory, char rows) { // Useful with .lua
	inventory -= 1;
	maxslots[inventory] += (rows * 4);
	if (maxslots[inventory] > 100)
		maxslots[inventory] = 100;
	if (maxslots[inventory] < 24) // Retard.
		maxslots[inventory] = 24;
	InventoryPacket::updateSlots(player, inventory + 1, maxslots[inventory]);
}

void PlayerInventory::setMesos(int mesos, bool is) {
	if (mesos < 0)
		mesos = 0;
	this->mesos = mesos;
	PlayerPacket::updateStat(player, 0x40000, mesos, is);
}

void PlayerInventory::addItem(char inv, short slot, Item *item) {
	items[inv-1][slot] = item;
	if (itemamounts.find(item->id) != itemamounts.end())
		itemamounts[item->id] += item->amount;
	else
		itemamounts[item->id] = item->amount;
	if (slot < 0)
		addEquipped(slot, item->id);
}

Item * PlayerInventory::getItem(char inv, short slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot];
	return 0;
}

void PlayerInventory::deleteItem(char inv, short slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end()) {
		itemamounts[items[inv][slot]->id] -= items[inv][slot]->amount;
		if (slot < 0)
			addEquipped(slot, 0);
		delete items[inv][slot];
		items[inv].erase(slot);
	}
}

void PlayerInventory::setItem(char inv, short slot, Item *item) {
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

short PlayerInventory::getItemAmountBySlot(char inv, short slot) {
	inv -= 1;
	if (items[inv].find(slot) != items[inv].end())
		return items[inv][slot]->amount;
	return 0;
}

void PlayerInventory::addEquipped(short slot, int itemid) {
	slot = abs(slot);

	if (slot > 100) // Cash items
		equipped[slot - 100][0] = itemid;
	else // Normal items
		equipped[slot][1] = itemid;
}

int PlayerInventory::getEquippedID(short slot) {
	if (equipped[slot][1] != 0)
		return equipped[slot][1];
	return 0;
}

void PlayerInventory::addEquippedPacket(PacketCreator &packet) {
	for (char i = 0; i < 50; i++) { // Shown items
		if (equipped[i][0] > 0 || equipped[i][1] > 0) {
			packet.addByte(i);
			if (equipped[i][0] <= 0 || (i == 11 && equipped[i][1] > 0)) // Normal weapons always here
				packet.addInt(equipped[i][1]);
			else
				packet.addInt(equipped[i][0]);
		}
	}
	packet.addByte(-1);
	for (char i = 0; i < 50; i++) { // Covered items
		if (equipped[i][0] > 0 && equipped[i][1] > 0 && i != 11) {
			packet.addByte(i);
			packet.addInt(equipped[i][1]);
		}
	}
	packet.addByte(-1);
	packet.addInt(equipped[11][0]); // Cash weapon
}

int PlayerInventory::getItemAmount(int itemid) {
	if (itemamounts.find(itemid) != itemamounts.end())
		return itemamounts[itemid];
	return 0;
}

bool PlayerInventory::hasOpenSlotsFor(int itemid, short amount) {
	bool has = false;
	short incrementor = 0;
	short required = 0;
	char inv = GETINVENTORY(itemid);
	if (inv == 1 || ISRECHARGEABLE(itemid))
		required = amount; // These aren't stackable
	else {
	//	int existing = getItemAmount(itemid);
		short maxslot = Inventory::items[itemid].maxslot;
	/*
	// Bug in global, would be fixed by uncommenting all of these commented lines:
	// It doesn't matter if you already have a slot with a partial stack or not, scripts require at least 1 empty slot	
		if (existing > 0) { // Stackable item already exists
			existing = existing % maxslot; // Is the last slot full?
			if (existing > 0) { // If not, calculate how many slots necessary
				existing += amount;
				if (existing > maxslot) { // Only have to bother with required slots if it would put us over the limit of a slot
				required = (int)(existing / maxslot);
				if ((existing % maxslot) > 0)
					required += 1;
				}
			}
			else { // If it is, treat it as though no items exist at all
				required = (int)(amount / maxslot);
				if ((amount % maxslot) > 0)
					required += 1;
			}
		}
		else { // No items exist, straight computation
		*/
		required = (int)(amount / maxslot);
		if ((amount % maxslot) > 0)
			required += 1;
	//	}
	}
	for (unsigned char i = 0; i < getMaxSlots(inv); i++) {
		if (incrementor >= required) {
			has = true;
			break;
		}
		if (getItem(inv, i) == 0)
			incrementor++;
	}
	return has;
}

void PlayerInventory::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT inv, slot, itemid, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, petid, items.name, pets.index, pets.name, pets.level, pets.closeness, pets.fullness FROM items LEFT JOIN pets ON items.petid=pets.id WHERE charid = " << mysqlpp::quote << player->getId();
	mysqlpp::StoreQueryResult res = query.store();

	for (size_t i = 0; i < res.num_rows(); ++i) {
		Item *item = new Item;
		item->id = res[i][2];
		item->amount = res[i][3];
		item->slots = (unsigned char) res[i][4];
		item->scrolls = (unsigned char) res[i][5];
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
		addItem((unsigned char) res[i][0], res[i][1], item);
		if (item->petid != 0) {
			Pet *pet = new Pet(player);
			pet->setId(item->petid);
			pet->setType(item->id);
			pet->setIndex((signed char) res[i][23]);
			pet->setName((string) res[i][24]);
			pet->setLevel((unsigned char) res[i][25]);
			pet->setCloseness((short) res[i][26]);
			pet->setFullness((unsigned char) res[i][27]);
			pet->setInventorySlot((unsigned char)res[i][1]);
			pet->setSummoned(false);
			player->getPets()->addPet(pet);
		}
	}
}

void PlayerInventory::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "DELETE FROM items WHERE charid = " << mysqlpp::quote << player->getId();
	query.exec();

	bool firstrun = true;
	for (char i = 1; i <= 5; i++) {
		iteminventory *itemsinv = getItems(i);
		for (iteminventory::iterator iter = itemsinv->begin(); iter != itemsinv->end(); iter++) {
			Item *item = iter->second;
			if (firstrun) {
				query << "INSERT INTO items VALUES (";
				firstrun = false;
			}
			else {
				query << ",(";
			}
			query << mysqlpp::quote << player->getId() << ","
				<< mysqlpp::quote << (short) i << ","
				<< mysqlpp::quote << iter->first << ","
				<< mysqlpp::quote << item->id << ","
				<< mysqlpp::quote << item->amount << ","
				<< mysqlpp::quote << (short) item->slots << ","
				<< mysqlpp::quote << (short) item->scrolls << ","
				<< mysqlpp::quote << item->istr << ","
				<< mysqlpp::quote << item->idex << ","
				<< mysqlpp::quote << item->iint << ","
				<< mysqlpp::quote << item->iluk << ","
				<< mysqlpp::quote << item->ihp << ","
				<< mysqlpp::quote << item->imp << ","
				<< mysqlpp::quote << item->iwatk << ","
				<< mysqlpp::quote << item->imatk << ","
				<< mysqlpp::quote << item->iwdef << ","
				<< mysqlpp::quote << item->imdef << ","
				<< mysqlpp::quote << item->iacc << ","
				<< mysqlpp::quote << item->iavo << ","
				<< mysqlpp::quote << item->ihand << ","
				<< mysqlpp::quote << item->ijump << ","
				<< mysqlpp::quote << item->ispeed << ","
				<< mysqlpp::quote << item->petid << ","
				<< mysqlpp::quote << item->name << ")";
		}
	}
	query.exec();
}
