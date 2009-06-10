/*
Copyright (C) 2008-2009 Vana Development Team

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

#include "Players.h"
#include "Player.h"

class Drop;
class Player;
class PacketReader;
struct Item;

namespace Drops {
	void doDrops(int32_t playerid, int32_t mapid, int32_t droppingId, Pos origin, bool explosive, bool ffa, int16_t taunt = 100, bool isSteal = false);
	void dropMesos(Player *player, PacketReader &packet);
	void playerLoot(Player *player, PacketReader &packet);
	void petLoot(Player *player, PacketReader &packet);
	void lootItem(Player *player, int32_t dropid, int32_t petid = 0);
};

class Drop {
private:
	int16_t questid;
	int32_t owner;
	int32_t time;
	int32_t mapid;
	int32_t id;
	int32_t mesos;
	int32_t dropped;
	int32_t playerid;
	bool playerdrop;
	bool tradeable;
	Pos pos;
	Item item;
public:
	Drop(int32_t mapid, int32_t mesos, Pos pos, int32_t owner, bool playerdrop = false);
	Drop(int32_t mapid, Item item, Pos pos, int32_t owner, bool playerdrop = false);

	void setQuest(int16_t questid) { this->questid = questid; }
	void setTradeable(bool istrade) { tradeable = istrade; }
	void setItemAmount(int16_t amount) { this->item.amount = amount; }
	void setId(int32_t id) { this->id = id; }
	void setTime(int32_t time) { this->time = time; }
	void setDropped(int32_t time) { dropped = time; }
	void setPlayer(int32_t playerid) { playerid = playerid; }
	void setPos(Pos pos) { this->pos = pos; }

	int16_t getQuest() const { return questid; }
	int32_t getId() const { return id; }
	int32_t getDropped() const { return dropped; }
	int32_t getTime() const { return time; }
	int32_t getOwner() const { return owner; }
	int32_t getMap() const { return mapid; }
	bool isplayerDrop() const { return playerdrop; }
	bool isMesos() const { return mesos > 0; }
	bool isQuest() const { return questid > 0; }
	bool isTradeable() const { return tradeable; }
	Pos getPos() const { return pos; }
	Item getItem() const { return item; }

	int16_t getAmount();
	int32_t getObjectId();

	void doDrop(Pos origin);
	void showDrop(Player *player);
	void takeDrop(Player *player, int32_t petid);
	void removeDrop(bool showPacket = true);
};

#endif
