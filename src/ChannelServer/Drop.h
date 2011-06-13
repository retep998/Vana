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
	Drop(int32_t mapId, int32_t mesos, const Pos &pos, int32_t owner, bool playerDrop = false);
	Drop(int32_t mapId, const Item &item, const Pos &pos, int32_t owner, bool playerDrop = false);

	void setQuest(int16_t questId) { m_questId = questId; }
	void setTradeable(bool isTrade) { m_tradeable = isTrade; }
	void setItemAmount(int16_t amount) { m_item.setAmount(amount); }
	void setId(int32_t id) { m_id = id; }
	void setTime(int32_t time) { m_time = time; }
	void setDropped(int32_t time) { m_dropped = time; }
	void setPlayerId(int32_t playerId) { m_playerId = playerId; }
	void setOwner(int32_t owner) { m_owner = owner; }
	void setPos(Pos pos) { m_pos = pos; }
	void setType(int8_t t) { m_type = t; }

	int8_t getType() const { return m_type; }
	int16_t getQuest() const { return m_questId; }
	int32_t getId() const { return m_id; }
	int32_t getDropped() const { return m_dropped; }
	int32_t getTime() const { return m_time; }
	int32_t getOwner() const { return m_owner; }
	int32_t getMap() const { return m_mapId; }
	bool isplayerDrop() const { return m_playerDrop; }
	bool isMesos() const { return m_mesos > 0; }
	bool isQuest() const { return m_questId > 0; }
	bool isTradeable() const { return m_tradeable; }
	Pos getPos() const { return m_pos; }
	Item getItem() const { return m_item; }

	int16_t getAmount();
	int32_t getObjectId();

	void doDrop(const Pos &origin);
	void showDrop(Player *player);
	void takeDrop(Player *player, int32_t petId);
	void removeDrop(bool showPacket = true);

	static const int8_t Explosive = 3;
	static const int8_t FreeForAll = 2;
	static const int8_t Party = 1;
	static const int8_t Normal = 0;
private:
	int8_t m_type;
	int16_t m_questId;
	int32_t m_owner;
	int32_t m_time;
	int32_t m_mapId;
	int32_t m_id;
	int32_t m_mesos;
	int32_t m_dropped;
	int32_t m_playerId;
	bool m_playerDrop;
	bool m_tradeable;
	Pos m_pos;
	Item m_item;
};