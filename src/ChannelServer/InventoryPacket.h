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
#ifndef INVPACK_H
#define INVPACK_H

#include <vector>
#include <string>

using std::string;
using std::vector;

class Player;
struct Equip;
struct Item;

namespace InventoryPacket {
	void moveItem(Player *player, char inv, short slot1, short slot2);
	void updatePlayer(Player *player);
	void bought(Player *player);
	void addItem(Player *player, char inv, short slot, Item *item, bool is);
	void addNewItem(Player *player, char inv, short slot, Item *item, bool is);
	void updateItemAmounts(Player *player, char inv, short slot1, short amount1, short slot2, short amount2);
	void sitChair(Player *player, int chairid);
	void stopChair(Player *player);
	void useItem(Player *player, int itemid, int time, unsigned char types[8], vector<short> vals, bool morph); // Use buff item
	void endItem(Player *player, unsigned char types[8], bool morph);
	void useSkillbook(Player *player, int skillid, int newMaxLevel, bool use, bool succeed);
	void useScroll(Player *player, bool succeed, bool destroy, bool legendary_spirit);
	void showMegaphone(Player *player, const string & msg);
	void showSuperMegaphone(Player *player, const string & msg, int whisper = 0);
	void showMessenger(Player *player, const string & msg, const string & msg2, const string & msg3, const string & msg4, unsigned char *displayInfo, int displayInfo_size, int itemid);
	void useItemEffect(Player *player, int itemid);
	void updateSlots(Player *player, char inventory, char slots);
	void blankUpdate(Player *player);
};

#endif
