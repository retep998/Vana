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
#ifndef DROPS_H
#define DROPS_H

#include <unordered_map>
#include <vector>

using std::vector;
using std::tr1::unordered_map;

#include "Players.h"
#include "Player.h"

class Drop;
class Player;
class Mob;
class Reactor;
class ReadPacket;
class Map;
struct Item;

struct DropInfo {
	DropInfo() : id(0), chance(0), quest(0), ismesos(false), minmesos(0), maxmesos(0) { }
	int id;
	int chance;
	int quest;
	bool ismesos;
	int minmesos;
	int maxmesos;
};
typedef vector<DropInfo> DropsInfo;

namespace Drops {
	extern unordered_map<int, DropsInfo> dropdata;
	void addDropData(int id, DropInfo drop);
	void doDrops(Player *player, int droppingID, Pos origin);
	void dropMesos(Player *player, ReadPacket *packet);
	void lootItem(Player *player, ReadPacket *packet);
};

class Drop {
private:
	int mapid;
	int id;
	int mesos;
	Item item;
	int owner;
	int time;
	int questid;
	int dropped;
	int playerid;
	Pos pos;
public:
	Drop (int mapid, int mesos, Pos pos, int owner);
	Drop (int mapid, Item item, Pos pos, int owner);
	void setID(int id) {
		this->id = id;
	}
	int getID() {
		return id;
	}
	int getObjectID();
	int getAmount();
	bool isQuest() {
		return questid > 0;
	}
	void setQuest(int questid) {
		this->questid = questid;
	}
	int getQuest() {
		return questid;
	}
	void setTime(int time) {
		this->time = time;
	}
	int getTime() {
		return time;
	}
	int getOwner() {
		return owner;
	}
	int getMap() {
		return mapid;
	}
	bool isMesos() {
		return mesos > 0;
	}
	Item getItem() {
		return item;
	}
	void setItemAmount(short amount) {
		this->item.amount = amount;
	}
	void setPos(Pos pos) {
		this->pos = pos;
	}
	Pos getPos() {
		return pos;
	}
	void setDropped(int time) {
		dropped = time;
	}
	int getDropped() {
		return dropped;
	}
	void setPlayer(int playerid) {
		playerid = playerid;
	}
	void doDrop(Pos origin);
	void showDrop(Player *player);
	void takeDrop(Player *player);
	void removeDrop(bool showPacket = true);
};

#endif
