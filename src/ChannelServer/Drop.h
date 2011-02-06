/*
Copyright (C) 2008-2011 Vana Development Team

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
#pragma once

#include "PlayerInventory.h"
#include "Pos.h"
#include "Types.h"

class Player;

class Drop {
public:
	Drop(int32_t mapid, int32_t mesos, const Pos &pos, int32_t owner, bool playerdrop = false);
	Drop(int32_t mapid, const Item &item, const Pos &pos, int32_t owner, bool playerdrop = false);

	void setQuest(int16_t questid) { this->questid = questid; }
	void setTradeable(bool istrade) { tradeable = istrade; }
	void setItemAmount(int16_t amount) { this->item.setAmount(amount); }
	void setId(int32_t id) { this->id = id; }
	void setTime(int32_t time) { this->time = time; }
	void setDropped(int32_t time) { dropped = time; }
	void setPlayerId(int32_t playerid) { this->playerid = playerid; }
	void setOwner(int32_t owner) { this->owner = owner; }
	void setPos(Pos pos) { this->pos = pos; }
	void setType(int8_t t) { type = t; }

	int8_t getType() const { return type; }
	int16_t getQuest() const { return questid; }
	int32_t getId() const { return id; }
	int32_t getDropped() const { return dropped; }
	int32_t getTime() const { return time; }
	int32_t getOwner() const { return owner; }
	int32_t getMap() const { return mapid; }
	bool isPlayerDrop() const { return playerdrop; }
	bool isMesos() const { return mesos > 0; }
	bool isQuest() const { return questid > 0; }
	bool isTradeable() const { return tradeable; }
	Pos getPos() const { return pos; }
	Item getItem() const { return item; }

	int16_t getAmount();
	int32_t getObjectId();

	void doDrop(const Pos &origin);
	void showDrop(Player *player);
	void takeDrop(Player *player, int32_t petid);
	void removeDrop(bool showPacket = true);

	static const int8_t Explosive = 3;
	static const int8_t FreeForAll = 2;
	static const int8_t Party = 1;
	static const int8_t Normal = 0;
private:
	int8_t type;
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
};