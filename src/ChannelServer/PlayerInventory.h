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
	short amount;
};
typedef hash_map<short, Item *> iteminventory;

class PlayerInventory {
public:
	PlayerInventory(Player *player) : player(player) {
		maxslots[0] = 100; // Equip
		maxslots[1] = 100; // Use
		maxslots[2] = 100; // Setup
		maxslots[3] = 100; // Etc
		maxslots[4] = 100; // Cash
	}
	char getMaxslots(char inv) {
		return maxslots[inv-1];
	}
	void setMesosStart(int mesos) {
		this->mesos = mesos;
	}
	void setMesos(int mesos, bool is = false);
	int getMesos() {
		return this->mesos;
	}
	void addEquip(short slot, Equip *equip) {
		equips[slot] = equip;
		if (itemamounts.find(equip->id) != itemamounts.end())
			itemamounts[equip->id] += 1;
		else
			itemamounts[equip->id] = 1;
	}
	void setEquip(short slot, Equip *equip) {
		if (equip == 0)
			equips.erase(slot);
		else
			equips[slot] = equip;
	}
	Equip * getEquip(short slot) {
		if (equips.find(slot) != equips.end())
			return equips[slot];
		return 0;
	}
	void deleteEquip(short slot) {
		if (equips.find(slot) != equips.end()) {
			itemamounts[equips[slot]->id] -= 1;
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
		inv -= 2;
		if (item == 0)
			items[inv].erase(slot);
		else
			items[inv][slot] = item;
	}
	short getItemAmountBySlot(char inv, short slot) {
		inv -= 2;
		if (items[inv].find(slot) != items[inv].end())
			return items[inv][slot]->amount;
		return 0;
	}
	void changeItemAmount(int itemid, short amount) {
		itemamounts[itemid] += amount;
	}
	int getItemAmount(int itemid) {
		if (itemamounts.find(itemid) != itemamounts.end())
			return itemamounts[itemid];
		return 0;
	}
private:
	char maxslots[5];
	Player *player;
	int mesos;
	equipinventory equips;
	iteminventory items[4];
	hash_map <int, int> itemamounts;
};

#endif
