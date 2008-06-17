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

#include <vector>
using namespace std;

class Player;

struct Equip {
	Equip () : pos(0) { }
	Equip (Equip *equip) : pos(0) {
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
		iavo = equip->ihand;
		ihand = equip->ihand;
		ijump = equip->ijump;
		ispeed = equip->ispeed;
	}
	short pos;
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

struct Item {
	Item () : pos(0) {}
	Item (Item *item) : pos(0) {
		inv = item->inv;
		id = item->id;
		amount = item->amount;
	}
	short pos;
	char inv;
	int id;
	int amount;
};

class PlayerInventory {
public:
	PlayerInventory() {
		maxslots = 100;
	}
	int getMaxslots() {
		return maxslots;
	}
	void setMesos(int mesos, bool is = false);
	void setMesosStart(int mesos) {
		this->mesos = mesos;
	}
	int getMesos() {
		return this->mesos;
	}
	void addEquip(Equip *equip) {
		equips.push_back(equip);
	}
	int getEquipNum() {
		return equips.size();
	}
	Equip * getEquipByPos(short pos) {
		for (int i = 0; i < getEquipNum(); i++) { // Get Equips
			Equip *equip = getEquip(i);
			if (equip->pos == pos)
				return equip;
		}
		return 0;
	}
	short getEquipPos(int equipid) {
		return equips[equipid]->pos;
	}
	void setEquipPos(int equipid, short pos) {
		equips[equipid]->pos = pos;
	}
	void deleteEquip(int equipid) {
		delete equips[equipid];
		equips.erase(equips.begin()+equipid);
	}
	Equip * getEquip(int id) {
		return equips[id];
	}
	void addItem(Item *item) {
		items.push_back(item);
	}
	int getItemNum() {
		return items.size();
	}
	short getItemPos(int itemid) {
		return items[itemid]->pos;
	}
	void setItemPos(int itemid, short pos) {
		items[itemid]->pos = pos;
	}
	void deleteItem(int itemid) {
		delete items[itemid];
		items.erase(items.begin()+itemid);
	}
	void setItem(Item *item, int itemid) {
		items[itemid] = item;
	}
	Item * getItem(int id) {
		return items[id];
	}
	Item * getItemByPos(int pos, char inv) {
		for (int i = 0; i < getItemNum(); i++)
			if (getItem(i)->pos == pos && getItem(i)->inv == inv)
				return getItem(i);
		return 0;
	}
	void setPlayer(Player *player) {
		this->player = player;
	}
	int getItemAmount(int itemid) {
		int amount = 0;
		for (int i = 0; i < getItemNum(); i++)
			if (getItem(i)->id == itemid)
				amount += getItem(i)->amount;
		return amount;
	}
	int getItemAmountBySlot(int slot, char inv) {
		int amount = 0;
		for (int i = 0; i < getItemNum(); i++)
			if (getItem(i)->pos == slot && getItem(i)->inv == inv) {
				amount = getItem(i)->amount;
				break;
			}
		return amount;
	}
private:
	int maxslots;
	int mesos;
	Player *player;
	vector <Equip*> equips;
	vector <Item*> items;
};

#endif
