/*
Copyright (C) 2008-2014 Vana Development Team

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
	NO_DEFAULT_CONSTRUCTOR(Drop);
	NONCOPYABLE(Drop);
public:
	Drop(int32_t mapId, int32_t mesos, const Pos &pos, int32_t owner, bool playerDrop = false);
	Drop(int32_t mapId, const Item &item, const Pos &pos, int32_t owner, bool playerDrop = false);

	auto setQuest(int16_t questId) -> void { m_questId = questId; }
	auto setTradeable(bool isTrade) -> void { m_tradeable = isTrade; }
	auto setItemAmount(int16_t amount) -> void { m_item.setAmount(amount); }
	auto setId(int32_t id) -> void { m_id = id; }
	auto setTime(int32_t time) -> void { m_time = time; }
	auto setPlayerId(int32_t playerId) -> void { m_playerId = playerId; }
	auto setOwner(int32_t owner) -> void { m_owner = owner; }
	auto setPos(Pos pos) -> void { m_pos = pos; }
	auto setType(int8_t t) -> void { m_type = t; }
	auto setDroppedAtTime(time_point_t time) -> void { m_droppedAtTime = time; }

	auto getType() const -> int8_t { return m_type; }
	auto getQuest() const -> int16_t { return m_questId; }
	auto getId() const -> int32_t { return m_id; }
	auto getTime() const -> int32_t { return m_time; }
	auto getOwner() const -> int32_t { return m_owner; }
	auto getMap() const -> int32_t { return m_mapId; }
	auto isplayerDrop() const -> bool { return m_playerDrop; }
	auto isMesos() const -> bool { return m_mesos > 0; }
	auto isQuest() const -> bool { return m_questId > 0; }
	auto isTradeable() const -> bool { return m_tradeable; }
	auto getPos() const -> Pos { return m_pos; }
	auto getItem() const -> Item { return m_item; }
	auto getDroppedAtTime() const -> time_point_t { return m_droppedAtTime; }

	auto getAmount() -> int16_t;
	auto getObjectId() -> int32_t;

	auto doDrop(const Pos &origin) -> void;
	auto showDrop(Player *player) -> void;
	auto takeDrop(Player *player, int64_t petId) -> void;
	auto removeDrop(bool showPacket = true) -> void;

	static const int8_t Explosive = 3;
	static const int8_t FreeForAll = 2;
	static const int8_t Party = 1;
	static const int8_t Normal = 0;
private:
	bool m_playerDrop = false;
	bool m_tradeable = true;
	int8_t m_type = Drop::Normal;
	int16_t m_questId = 0;
	int32_t m_owner = 0;
	int32_t m_mapId = 0;
	int32_t m_id = 0;
	int32_t m_mesos = 0;
	int32_t m_time = 0;
	int32_t m_playerId = 0;
	time_point_t m_droppedAtTime;
	Pos m_pos;
	Item m_item;
};