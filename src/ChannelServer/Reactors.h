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
#ifndef REACTOR_H
#define REACTOR_H

#include <tr1/unordered_map>
#include <vector>
#include "Pos.h"

using std::vector;
using std::tr1::unordered_map;

class LoopingId;
class Player;
class Drop;
class PacketReader;

struct ReactorEventInfo {
	int8_t state;
	int16_t type;
	int32_t itemid;
	int16_t ltx;
	int16_t lty;
	int16_t rbx;
	int16_t rby;
	int8_t nextstate;
};
typedef vector<ReactorEventInfo> ReactorEventsInfo;

namespace Reactors {
	extern unordered_map<int32_t, ReactorEventsInfo> reactorinfo;
	extern unordered_map<int32_t, int16_t> maxstates;
	void addEventInfo(int32_t id, ReactorEventInfo revent);
	void setMaxstates(int32_t id, int16_t state);
	void hitReactor(Player *player, PacketReader &packet);
	void checkDrop(Player *player, Drop *drop);
	void checkLoot(Drop *drop);
};

class Reactor {
public:
	Reactor (int32_t mapid, int32_t reactorid, Pos pos);
	void kill() { this->alive = false; }
	void revive() { this->alive = true; }
	void setState(int8_t state, bool is);
	void setID(int32_t id) { this->id = id; }

	int8_t getState() const { return this->state; }
	int32_t getID() const { return this->id; }
	int32_t getReactorID() const { return this->reactorid; }
	int32_t getMapID() const { return this->mapid; }
	bool isAlive() const { return this->alive; }
	Pos getPos() const { return this->pos; }

	void restore();
	void drop(Player *player);
private:
	int8_t state;
	int32_t id;
	int32_t reactorid;
	int32_t mapid;
	bool alive;
	Pos pos;
};

#endif
