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
using stdext::hash_map;

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
	PlayerInventory(Player *player, unsigned char maxslots[5]) : player(player) {
		memcpy_s(this->maxslots, sizeof(this->maxslots), maxslots, sizeof(this->maxslots));
	}
	unsigned char getMaxSlots(char inv) {
		return maxslots[inv - 1];
	}
	void setMesosStart(int mesos) {
		this->mesos = mesos;
	}
	void setMesos(int mesos, bool is = false);
	int getMesos() {
		return this->mesos;
	}
	void addEquip(short slot, Equip *equip);
	void setEquip(short slot, Equip *equip);
	Equip * getEquip(short slot);
	void deleteEquip(short slot);
	equipinventory * getEquips() {
		return &equips;
	}
	void addItem(char inv, short slot, Item *item);
	Item * getItem(char inv, short slot);
	void deleteItem(char inv, short slot);
	void setItem(char inv, short slot, Item *item);
	short getItemAmountBySlot(char inv, short slot);
	void changeItemAmount(int itemid, short amount) {
		itemamounts[itemid] += amount;
	}
	int getItemAmount(int itemid);
	void addMaxSlots(char inventory, char rows);
private:
	unsigned char maxslots[5];
	Player *player;
	int mesos;
	equipinventory equips;
	iteminventory items[4];
	hash_map <int, int> itemamounts;
};

#endif
