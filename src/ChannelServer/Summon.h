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

#include "MovableLife.h"
#include "Types.h"

class Summon : public MovableLife {
	NONCOPYABLE(Summon);
	NO_DEFAULT_CONSTRUCTOR(Summon);
public:
	enum MovementPatterns : int8_t {
		Static = 0,
		Follow = 1,
		Flying = 3
	};

	Summon(int32_t id, int32_t summonId, uint8_t level);

	auto getId() -> int32_t { return m_id; }
	auto getSummonId() -> int32_t { return m_summonId; }
	auto getLevel() -> uint8_t { return m_level; }
	auto getType() -> uint8_t { return m_type; }
	auto getHP() -> int32_t { return m_hp; }
	auto doDamage(int32_t damage) -> void { m_hp -= damage; }
private:
	uint8_t m_level = 0;
	uint8_t m_type = 0;
	int32_t m_id = 0;
	int32_t m_summonId = 0;
	int32_t m_hp = 0;
};