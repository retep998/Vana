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
#ifndef INVENTORY_H
#define INVENTORY_H

#define ISSTAR(x) (x/10000 == 207)

class Player;
class ItemTimer;
struct Equip;
struct Item;

namespace Inventory {
	void startTimer();
	void stopTimerPlayer(Player* player);
	extern int findSlot(Player* player, int itemid ,char inv, short amount);
	extern int isCash(int itemid);
	extern ItemTimer* timer;
	extern Equip* setEquipStats(Player* player, int equipid);
	void useShop(Player* player, unsigned char* packet);
	void itemMove(Player* player, unsigned char* packet);
	void addEquip(Player* player, Equip* equip, bool is=0);
	void addItem(Player* player, Item* item, bool is=0);
	void addNewItem(Player* player, int item, int howmany);
	void takeItem(Player* player, int item, int howmany);
	void useItem(Player* player, unsigned char* packet);
	void cancelItem(Player* player, unsigned char* packet);
	void endItem(Player* player, int itemid);
	void takeItemSlot(Player* player, short slot, char inv, short amount);
	void useChair(Player* player, unsigned char* packet);
	void stopChair(Player* player, unsigned char* packet);
	void useSummonBag(Player* player, unsigned char* packet);
	void useReturnScroll(Player* player, unsigned char* packet);
	void useScroll(Player* player, unsigned char* packet);
	void useMegaphone(Player* player, unsigned char *packet);
};

#endif
