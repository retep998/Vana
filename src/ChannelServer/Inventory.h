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

#define ISARROW(x) (x/10000 == 206)
#define ISSTAR(x) (x/10000 == 207)

class Player;
class ItemTimer;
class ReadPacket;
struct Equip;
struct Item;

namespace Inventory {
	void startTimer();
	void stopTimerPlayer(Player *player);
	extern int findSlot(Player *player, int itemid ,char inv, short amount);
	extern bool isCash(int itemid);
	extern ItemTimer * timer;
	extern Equip * setEquipStats(Player *player, int equipid);
	void useShop(Player *player, ReadPacket *packet);
	void itemMove(Player *player, ReadPacket *packet);
	void addEquip(Player *player, Equip *equip, bool is = false);
	void addItem(Player *player, Item *item, bool is = false);
	void addNewItem(Player *player, int itemid, int howmany);
	void takeItem(Player *player, int itemid, int howmany);
	void useItem(Player *player, ReadPacket *packet);
	void cancelItem(Player *player, ReadPacket *packet);
	void endItem(Player *player, int itemid);
	void useSkillbook(Player *player, ReadPacket *packet);
	void takeItemSlot(Player *player, short slot, char inv, short amount, bool takeStar = false);
	void useChair(Player *player, ReadPacket *packet);
	void useItemEffect(Player *player, ReadPacket *packet);
	void stopChair(Player *player, ReadPacket *packet);
	void useSummonBag(Player *player, ReadPacket *packet);
	void useReturnScroll(Player *player, ReadPacket *packet);
	void useScroll(Player *player, ReadPacket *packet);
	void useCashItem(Player *player, ReadPacket *packet);
};

#endif
