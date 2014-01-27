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

// A base class for player, mobs that can move
class MovableLife {
	NONCOPYABLE(MovableLife);
	NO_DEFAULT_CONSTRUCTOR(MovableLife);
public:
	MovableLife(int16_t foothold, const Pos &pos, int8_t stance) : m_stance(stance), m_foothold(foothold), m_pos(pos) { }
	virtual ~MovableLife() = default;

	auto isFacingRight() const -> bool { return m_stance % 2 == 0; }
	auto isFacingLeft() const -> bool { return m_stance % 2 == 1; }
	auto getStance() const -> int8_t { return m_stance; }
	auto getFoothold() const -> int16_t { return m_foothold; }
	virtual auto getPos() const -> Pos { return m_pos; }

	auto setPos(const Pos &val) -> void { m_pos = val; }
	auto resetMovement(int16_t foothold, const Pos &pos, int8_t stance) -> void {
		m_stance = stance;
		m_foothold = foothold;
		m_pos = pos;
	}
protected:
	auto setStance(int8_t val) -> void { m_stance = val; }
	auto setFoothold(int16_t val) -> void { m_foothold = val; }

	int8_t m_stance = 0;
	int16_t m_foothold = 0;
	Pos m_pos;
};