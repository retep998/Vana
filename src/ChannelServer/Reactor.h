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

class Player;

class Reactor {
public:
	Reactor(int32_t mapid, int32_t reactorId, const Pos &pos);
	void kill() { this->alive = false; }
	void revive() { this->alive = true; }
	void setState(int8_t state, bool is);
	void setId(int32_t id) { this->id = id; }

	int8_t getState() const { return this->state; }
	int32_t getId() const { return this->id; }
	int32_t getReactorId() const { return this->reactorId; }
	int32_t getMapId() const { return this->mapid; }
	bool isAlive() const { return this->alive; }
	Pos getPos() const { return this->pos; }

	void restore();
	void drop(Player *player);
private:
	int8_t state;
	int32_t id;
	int32_t reactorId;
	int32_t mapid;
	bool alive;
	Pos pos;
};