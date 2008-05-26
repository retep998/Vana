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

#include <hash_map>
#include <vector>
#include <string>

using namespace std;
using namespace stdext;

#include "Player.h"
#include "Drops.h"

class Player;
class Packet;
class Drop;
class Reactor;

struct ReactorSpawnInfo {
	int id;
	short x;
	short y;
};
typedef vector<ReactorSpawnInfo> ReactorSpawnsInfo;

struct ReactorEventInfo {
	short state;
	short type;
	int itemid;
	short ltx;
	short lty;
	short rbx;
	short rby;
	short nextstate;
};
typedef vector<ReactorEventInfo> ReactorEventsInfo;

namespace Reactors {
	extern hash_map <int, ReactorEventsInfo> reactorinfo;
	extern hash_map <int, ReactorSpawnsInfo> info;
	extern hash_map <int, vector<Reactor*>> reactors;
	extern int reactorscount;
	void addReactorSpawn(int id, ReactorSpawnsInfo reactor);
	void addReactorEventInfo(int id, ReactorEventInfo revent);
	void loadReactors();
	void showReactors(Player* player);
	void hitReactor(Player* player, unsigned char *packet);
	void checkDrop(Player* player, Drop *drop);
	Reactor* getReactorByID(int id, int mapid);
};

class Reactor {
public:
	Reactor () {
		alive = true;
		state = 0;
	}
	void kill() {
		this->alive = false;
	}
	void revive() {
		this->alive = true;
	}
	bool isAlive() {
		return this->alive;
	}
	void setID(int id) {
		this->id = id;
	}
	int getID() {
		return this->id;
	}
	void setReactorID(int reactorid) {
		this->reactorid = reactorid;
	}
	int getReactorID() {
		return this->reactorid;
	}
	void setMapID(int mapid) {
		this->mapid = mapid;
	}
	int getMapID() {
		return this->mapid;
	}
	void setState(int state) {
		this->state = state;
	}
	int getState() {
		return this->state;
	}
	void setPos(Pos pos) {
		this->pos = pos;
	}
	Pos getPos() {
		return this->pos;
	}
private:
	Pos pos;
	bool alive;
	int id;
	int reactorid;
	int mapid;
	int state;
};

#endif
