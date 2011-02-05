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
#include <boost/tr1/unordered_map.hpp>
#include <vector>

class Player;

class Reactor {
public:
	Reactor(int32_t mapId, int32_t reactorId, const Pos &pos);
	void kill() { m_alive = false; }
	void revive() { m_alive = true; }
	void setState(int8_t state, bool sendPacket);
	void setId(int32_t id) { m_id = id; }

	int8_t getState() const { return m_state; }
	int32_t getId() const { return m_id; }
	int32_t getReactorId() const { return m_reactorId; }
	int32_t getMapId() const { return m_mapId; }
	bool isAlive() const { return m_alive; }
	Pos getPos() const { return m_pos; }

	void restore();
	void drop(Player *player);
private:
	int8_t m_state;
	int32_t m_id;
	int32_t m_reactorId;
	int32_t m_mapId;
	bool m_alive;
	Pos m_pos;
};
