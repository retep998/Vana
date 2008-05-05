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

using namespace std;

class Player;
struct Equip;
struct Item;

namespace InventoryPacket {
	void moveItem(Player* player, char inv, short slot1, short slot2);
	void updatePlayer(Player* player);
	void addEquip(Player* player, Equip* equip, bool is);
	void bought(Player* player);
	void newMesos(Player* player, int mesos, bool is);
	void addItem(Player* player, Item* item, bool is);
	void addNewItem(Player* player, Item* item, bool is);
	void moveItemS(Player* player, char inv, short slot, short amount);
	void moveItemS2(Player* player, char inv, short slot1, short amount1, short slot2, short amount2);
	void sitChair(Player* player, vector <Player*> players, int chairid);
	void stopChair(Player* player, vector <Player*> players);
	void useScroll(Player* player, vector <Player*> players, char s);
	void showMegaphone(Player* player, vector <Player*> players, char* msg);
	void showSuperMegaphone(Player* player, char* msg, int whisper = 0);
};

#endif