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

#include "Pos.h"
#include "Types.h"

class Map;
class Mob;
class Player;
struct SkillLevelInfo;
struct MobSkillLevelInfo;

class Mist {
public:
	Mist(int32_t mapid, Player *owner, const Pos &origin, SkillLevelInfo *skill, int32_t skillid, uint8_t level, bool ispoison = false);
	Mist(int32_t mapid, Mob *owner, const Pos &origin, MobSkillLevelInfo *skill, uint8_t skillid, uint8_t level);

	void setId(int32_t id) { m_id = id; }
	uint8_t getSkillLevel() const { return m_level; }
	int16_t getTime() const { return m_time; }
	int16_t getDelay() const { return m_delay; }
	int32_t getSkillId() const { return m_skill; }
	int32_t getId() const { return m_id; }
	int32_t getOwnerId() const { return m_ownerid; }
	bool isMobMist() const { return m_ismobmist; }
	bool isPoison() const { return m_poison; }
	Pos getLt() const { return m_lt; }
	Pos getRb() const { return m_rb; }
	Pos getSkillLt() const { return m_skilllt; }
	Pos getSkillRb() const { return m_skillrb; }
	Pos getOrigin() const { return m_origin; }
	Map * getMap() const;
private:
	uint8_t m_level;
	int16_t m_time;
	int16_t m_delay;
	int32_t m_id;
	int32_t m_skill;
	int32_t m_ownermap;
	int32_t m_ownerid;
	bool m_ismobmist;
	bool m_poison;
	Pos m_lt;
	Pos m_rb;
	Pos m_skilllt;
	Pos m_skillrb;
	Pos m_origin;
};
