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
#ifndef PINV_H
#define PINV_H

#include <hash_map>
using namespace std;
using namespace stdext;

class Player;

struct Equip {
	Equip () { }
	Equip (Equip *equip) {
		id = equip->id;
		slots = equip->slots;
		scrolls = equip->scrolls;
		type = equip->type;
		istr = equip->istr;
		idex = equip->idex;
		iint = equip->iint;
		iluk = equip->iluk;
		ihp = equip->ihp;
		imp = equip->imp;
		iwatk = equip->iwatk;
		imatk = equip->imatk;
		iwdef = equip->iwdef;
		imdef = equip->imdef;
		iacc = equip->iacc;
		iavo = equip->iavo;
		ihand = equip->ihand;
		ijump = equip->ijump;
		ispeed = equip->ispeed;
	}
	int id;
	char slots;
	char scrolls;
	char type;
	short istr;
	short idex;
	short iint;
	short iluk;
	short ihp;
	short imp;
	short iwatk;
	short imatk;
	short iwdef;
	short imdef;
	short iacc;
	short iavo;
	short ihand;
	short ijump;
	short ispeed;
};
typedef hash_map<short, Equip *> equipinventory;

struct Item {
	Item () : id(0), amount(0) { }
	Item (Item *item) {
		id = item->id;
		amount = item->amount;
	}
	int id;
	int amount;
};
typedef hash_map<short, Item *> iteminventory;

class PlayerInventory {
public:
	PlayerInventory() {
		maxslots = 100;
		items[0] = use;
		items[1] = setup;
		items[2] = etc;
		items[3] = cash;
	}
	short getMaxslots() {
		return maxslots;
	}
	void setPlayer(Player *player) {
		this->player = player;
	}
	void setMesosStart(int mesos) {
		this->mesos = mesos;
	}
	void setMesos(int mesos, bool is = false);
	int getMesos() {
		return this->mesos;
	}
	void addEquip(short pos, Equip *equip) {
		equips[pos] = equip;
	}
	Equip * getEquip(short slot) {
		if (equips.find(slot) != equips.end())
			return equips[slot];
		return 0;
	}
	void deleteEquip(short slot) {
		if (equips.find(slot) != equips.end()) {
			delete equips[slot];
			equips.erase(slot);
		}
	}
	equipinventory * getEquips() {
		return &equips;
	}
	void addItem(char inv, short slot, Item *item) {
		items[inv-2][slot] = item;
		if (itemamounts.find(item->id) != itemamounts.end())
			itemamounts[item->id] += item->amount;
		else
			itemamounts[item->id] = item->amount;
	}
	Item * getItem(char inv, short slot) {
		inv -= 2;
		if (items[inv].find(slot) != items[inv].end())
			return items[inv][slot];
		return 0;
	}
	void deleteItem(char inv, short slot) {
		inv -= 2;
		if (items[inv].find(slot) != items[inv].end()) {
			itemamounts[items[inv][slot]->id] -= items[inv][slot]->amount;
			delete items[inv][slot];
			items[inv].erase(slot);
		}
	}
	void setItem(char inv, short slot, Item *item) {
		items[inv-2][slot] = item;
	}
	int getItemAmountBySlot(char inv, short slot) {
		inv -= 2;
		if (items[inv].find(slot) != items[inv].end())
			return items[inv][slot]->amount;
		return 0;
	}
	int getItemAmount(int itemid) {
		if (itemamounts.find(itemid) != itemamounts.end())
			return itemamounts[itemid];
		return 0;
	}
private:
	short maxslots;
	Player *player;
	int mesos;
	equipinventory equips;
	iteminventory items[4];
	iteminventory use;
	iteminventory setup;
	iteminventory etc;
	iteminventory cash;
	hash_map <int, int> itemamounts;
};

#endif
