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
#ifndef PLAYERINVENTORY_H
#define PLAYERINVENTORY_H

#include <unordered_map>

using std::tr1::unordered_map;
using std::string;

class Player;
class PacketCreator;

struct Item {
	Item () : amount(1), slots(0), scrolls(0), istr(0), idex(0), iint(0), iluk(0), ihp(0),
		imp(0), iwatk(0), imatk(0), iwdef(0), imdef(0), iacc(0), iavo(0), ihand(0), ijump(0), ispeed(0), petid(0), name("") { }
	Item (int itemid, short amount) : id(itemid), amount(amount), petid(0), name("") { }
	Item (int equipid, bool random);
	Item (Item *item) {
		id = item->id;
		amount = item->amount;
		slots = item->slots;
		scrolls = item->scrolls;
		istr = item->istr;
		idex = item->idex;
		iint = item->iint;
		iluk = item->iluk;
		ihp = item->ihp;
		imp = item->imp;
		iwatk = item->iwatk;
		imatk = item->imatk;
		iwdef = item->iwdef;
		imdef = item->imdef;
		iacc = item->iacc;
		iavo = item->iavo;
		ihand = item->ihand;
		ijump = item->ijump;
		ispeed = item->ispeed;
		petid = item->petid;
		name = item->name;
	}
	int id;
	short amount;
	char slots;
	char scrolls;
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
	int petid;
	string name;
};
typedef unordered_map<short, Item *> iteminventory;

class PlayerInventory {
public:
	PlayerInventory(Player *player, unsigned char maxslots[5]);
	unsigned char getMaxSlots(char inv) const { return maxslots[inv - 1]; }
	void setMesosStart(int mesos) { this->mesos = mesos; }
	void setMesos(int mesos, bool is = false);
	int getMesos() const { return this->mesos; }
	void addMaxSlots(char inventory, char rows);
	void addItem(char inv, short slot, Item *item);
	Item * getItem(char inv, short slot);
	void deleteItem(char inv, short slot);
	void setItem(char inv, short slot, Item *item);
	short getItemAmountBySlot(char inv, short slot);
	void addEquippedPacket(PacketCreator &packet);
	int getEquippedID(short slot);
	void changeItemAmount(int itemid, short amount) { itemamounts[itemid] += amount; }
	int getItemAmount(int itemid);
	bool hasOpenSlotsFor(int itemid, short amount);
	iteminventory * getItems(char inv) { return &items[inv - 1]; }
	void save();
private:
	unsigned char maxslots[5];
	Player *player;
	int mesos;
	iteminventory items[5];
	int equipped[50][2];
	unordered_map<int, int> itemamounts;
	void addEquipped(short slot, int itemid);
};

#endif
