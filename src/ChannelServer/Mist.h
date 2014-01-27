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

#include "Pos.h"
#include "Rect.h"
#include "Types.h"

class Map;
class Mob;
class Player;
struct SkillLevelInfo;
struct MobSkillLevelInfo;

class Mist {
	NONCOPYABLE(Mist);
	NO_DEFAULT_CONSTRUCTOR(Mist);
public:
	Mist(int32_t mapId, Player *owner, int32_t time, const Rect &area, int32_t skillId, uint8_t level, bool isPoison = false);
	Mist(int32_t mapId, Mob *owner, int16_t time, const Rect &area, uint8_t skillId, uint8_t level);

	auto setId(int32_t id) -> void { m_id = id; }
	auto getSkillLevel() const -> uint8_t { return m_level; }
	auto getTime() const -> int16_t { return m_time; }
	auto getDelay() const -> int16_t { return m_delay; }
	auto getSkillId() const -> int32_t { return m_skill; }
	auto getId() const -> int32_t { return m_id; }
	auto getOwnerId() const -> int32_t { return m_ownerId; }
	auto isMobMist() const -> bool { return m_isMobMist; }
	auto isPoison() const -> bool { return m_poison; }
	auto getArea() const -> Rect { return m_area; }
	auto getMap() const -> Map *;
private:
	bool m_isMobMist = true;
	bool m_poison = true;
	uint8_t m_level = 0;
	int16_t m_time = 0;
	int16_t m_delay = 0;
	int32_t m_id = 0;
	int32_t m_skill = 0;
	int32_t m_ownerMap = 0;
	int32_t m_ownerId = 0;
	Rect m_area;
};