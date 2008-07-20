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
#include "Reactors.h"
#include "LuaReactor.h"
#include "Player.h"
#include "ReactorPacket.h"
#include "InventoryPacket.h"
#include "PacketCreator.h"
#include "Maps.h"
#include "Mobs.h"
#include "Drops.h"
#include "Inventory.h"
#include "Timer.h"
#include "ReadPacket.h"
#include "LoopingId.h"
#include "Pos.h"
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <string>

class ReactionTimer: public Timer::TimerHandler {
public:
	static ReactionTimer * Instance() {
		if (singleton == 0)
			singleton = new ReactionTimer;
		return singleton;
	}
	void setReactionTimer(Player *player, Reactor *reactor, Drop *drop, char state, int time) {
		RTimer timer;
		timer.id = Timer::Instance()->setTimer(time, this);
		timer.state = state;
		timer.player = player;
		timer.reactor = reactor;
		timer.drop = drop;
		timer.time = time;
		timers.push_back(timer);
	}

	void stop(Drop *drop) {
		for (unsigned int i = 0; i < timers.size(); i++) {
			if (drop == timers[i].drop) {
				Timer::Instance()->cancelTimer(timers[i].id);
				break;
			}
		}
	}
private:
	static ReactionTimer *singleton;
	ReactionTimer() {};
	ReactionTimer(const ItemTimer&);
	ReactionTimer& operator=(const ItemTimer&);

	struct RTimer {
		int id;
		char state;
		Player *player;
		Reactor *reactor;
		Drop *drop;
		int time;
	};

	static vector <RTimer> timers;
	void handle(Timer *timer, int id) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				Reactor *reactor = timers[i].reactor;
				Drop *drop = timers[i].drop;
				Player *player = timers[i].player;
				reactor->setState(timers[i].state, true);
				drop->removeDrop();
				std::ostringstream filenameStream;
				filenameStream << "scripts/reactors/" << reactor->getReactorID() << ".lua";
				LuaReactor(filenameStream.str(), player->getPlayerid(), reactor->getID(), reactor->getMapID());
				return;
			}
		}
	}
	void remove(int id) {
		for (size_t i = 0; i < timers.size(); i++) {
			if (timers[i].id == id) {
				timers.erase(timers.begin()+i);
				return;
			}
		}
	}
};

vector <ReactionTimer::RTimer> ReactionTimer::timers;
ReactionTimer *ReactionTimer::singleton = 0;
// End of ReactionTimer

// Reactor class
Reactor::Reactor (int mapid, int reactorid, Pos pos) : mapid(mapid), reactorid(reactorid), pos(pos), alive(true), state(0) {
	Maps::maps[mapid]->addReactor(this);
}

void Reactor::setState(char state, bool is) {
	this->state = state;
	if (is)
		ReactorPacket::triggerReactor(this);
}

// Reactors namespace
hash_map <int, ReactorEventsInfo> Reactors::reactorinfo;
hash_map <int, short> Reactors::maxstates;

void Reactors::addEventInfo(int id, ReactorEventInfo revent) {
	reactorinfo[id].push_back(revent);
}

void Reactors::setMaxstates(int id, short state) {
	maxstates[id] = state;
}

void Reactors::hitReactor(Player *player, ReadPacket *packet) {
	int id = packet->getInt() - 200;

	Reactor *reactor = Maps::maps[player->getMap()]->getReactor(id);

	if (reactor != 0 && reactor->isAlive()) {
		if (reactor->getState() < maxstates[reactor->getReactorID()]) {
			ReactorEventInfo *revent = &reactorinfo[reactor->getReactorID()][reactor->getState()];
			if (revent->nextstate < maxstates[reactor->getReactorID()]) {
				if (revent->type >= 100)
					return;
				reactor->setState(revent->nextstate, true);
				ReactorPacket::triggerReactor(reactor);
				return;
			}
			else {
				std::ostringstream filenameStream;
				filenameStream << "scripts/reactors/" << reactor->getReactorID() << ".lua";
				LuaReactor(filenameStream.str(), player->getPlayerid(), id, reactor->getMapID());
				reactor->setState(revent->nextstate, false);
				reactor->kill();
				ReactorPacket::destroyReactor(reactor);
			}
		}
	}
}

void Reactors::checkDrop(Player *player, Drop *drop) {
	for (size_t i = 0; i < Maps::maps[drop->getMap()]->getNumReactors(); i++) {
		Reactor *reactor = Maps::maps[drop->getMap()]->getReactor(i);
		if (reactor->getState() < maxstates[reactor->getReactorID()]) {
			ReactorEventInfo *revent = &reactorinfo[reactor->getReactorID()][reactor->getState()];
			if (revent->type == 100 && drop->getObjectID() == revent->itemid) {
				if ((drop->getPos().x >= reactor->getPos().x + revent->ltx && drop->getPos().x <= reactor->getPos().x + revent->rbx) && (drop->getPos().y >= reactor->getPos().y + revent->lty && drop->getPos().y <= reactor->getPos().y + revent->rby)) {
					ReactionTimer::Instance()->setReactionTimer(player, reactor, drop, revent->nextstate, 3000);
				}
				return;
			}
		}
	}
}

void Reactors::checkLoot(Drop *drop) {
	ReactionTimer::Instance()->stop(drop);
}
