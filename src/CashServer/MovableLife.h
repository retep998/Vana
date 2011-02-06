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

// A base class for player, mobs that can move
class MovableLife {
public:
	MovableLife() { }
	MovableLife(int16_t foothold, Pos pos, int8_t stance) : m_stance(stance), m_foothold(foothold), m_pos(pos) { }
	virtual ~MovableLife() { }

	bool isFacingRight() const { return m_stance % 2 == 0; }
	virtual int8_t getStance() const { return m_stance; }
	virtual int16_t getFh() const { return m_foothold; }
	virtual Pos getPos() const { return m_pos; }

	virtual void setStance(int8_t val) { m_stance = val; }
	virtual void setFh(int16_t val) { m_foothold = val; }
	virtual void setPos(const Pos &val) { m_pos = val; }
protected:
	int8_t m_stance;
	int16_t m_foothold;
	Pos m_pos;
};
