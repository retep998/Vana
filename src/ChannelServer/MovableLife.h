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
#ifndef MOVABLELIFE_H
#define MOVABLELIFE_H

#include "Pos.h"

// A base class for player, mobs that can move
class MovableLife {
public:
	MovableLife() { }
	MovableLife(int16_t foothold, Pos pos, char stance)  :
	m_foothold(foothold),
	m_pos(pos),
	m_stance(stance)
	{
	}

	virtual int16_t getFH() { return m_foothold; }
	virtual void setFH(int16_t val) { m_foothold = val; }
	virtual Pos getPos() const { return m_pos; }
	virtual void setPos(const Pos &val) { m_pos = val; }
	virtual char getStance() const { return m_stance; }
	virtual void setStance(char val) { m_stance = val; }
protected:
	int16_t m_foothold;
	Pos m_pos;
	char m_stance;
};

#endif