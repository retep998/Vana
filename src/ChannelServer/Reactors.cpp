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
#include "Inventory.h"
#include "BufferUtilities.h"
#include "Timer.h"
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
	void setReactionTimer(Player *player, Reactor *reactor, Drop *drop, short state, int time) {
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
		for (unsigned int i=0; i<timers.size(); i++) {
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
		short state;
		Player *player;
		Reactor *reactor;
		Drop *drop;
		int time;
	};

	static vector <RTimer> timers;
	void handle(Timer* timer, int id) {
		for (unsigned int i=0; i<timers.size(); i++) {
			if (timers[i].id == id) {
				Reactor *reactor = timers[i].reactor;
				Drop *drop = timers[i].drop;
				Player *player = timers[i].player;
				reactor->setState(timers[i].state, 1);
				drop->removeDrop();
				std::ostringstream filenameStream;
				filenameStream << "scripts/reactors/" << reactor->getReactorID() << ".lua";
				LuaReactor(filenameStream.str(), player->getPlayerid(), reactor->getID(), reactor->getMapID());
				return;
			}
		}
	}
	void remove(int id) {
		for (unsigned int i=0; i<timers.size(); i++) {
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

hash_map <int, vector<ReactorEventInfo>> Reactors::reactorinfo;
hash_map <int, short> Reactors::maxstates;
hash_map <int, ReactorSpawnsInfo> Reactors::info;
hash_map <int, vector<Reactor*>> Reactors::reactors;
int Reactors::reactorscount = 0x200;

void Reactors::addReactorSpawn(int id, ReactorSpawnsInfo reactorspawns) {
	info[id] = reactorspawns;
}

void Reactors::addReactorEventInfo(int id, ReactorEventInfo revent) {
	reactorinfo[id].push_back(revent);
}

void Reactors::setReactorMaxstates(int id, short state) {
	maxstates[id] = state;
}

void Reactors::loadReactors() {
	for (hash_map<int, ReactorSpawnsInfo>::iterator iter = Reactors::info.begin(); iter != Reactors::info.end(); iter++) {
		for (unsigned int i=0; i<iter->second.size(); i++) {
			Reactor *reactor = new Reactor();
			Pos pos;
			pos.x = iter->second[i].x;
			pos.y = iter->second[i].y;
			reactor->setID(reactorscount++);
			reactor->setReactorID(iter->second[i].id);
			reactor->setMapID(iter->first);
			reactor->setPos(pos);
			reactors[iter->first].push_back(reactor);
		}
	}
}

void Reactors::showReactors(Player *player) {
	for (unsigned int i=0; i<reactors[player->getMap()].size(); i++) {
		if (reactors[player->getMap()][i]->isAlive()) {
			ReactorPacket::showReactor(player, reactors[player->getMap()][i]);
		}
	}
}

Reactor* Reactors::getReactorByID(int id, int mapid) {
	for (unsigned int i=0; i<reactors[mapid].size(); i++) {
		if (reactors[mapid][i]->getID() == id) {
			return reactors[mapid][i];
		}
	}
	return NULL;
}

void Reactors::hitReactor(Player *player, unsigned char *packet) {
	int id = BufferUtilities::getInt(packet);

	Reactor* reactor = getReactorByID(id, player->getMap());

	if (reactor != NULL && reactor->isAlive()) {
		if (reactor->getState() < maxstates[reactor->getReactorID()]) {
			ReactorEventInfo revent = reactorinfo[reactor->getReactorID()][reactor->getState()];
			if (revent.nextstate < maxstates[reactor->getReactorID()]) {
				if (revent.type >= 100)
					return;
				reactor->setState(revent.nextstate, true);
				ReactorPacket::triggerReactor(Maps::info[player->getMap()].Players, reactor);
				return;
			}
			else {
				std::ostringstream filenameStream;
				filenameStream << "scripts/reactors/" << reactor->getReactorID() << ".lua";
				LuaReactor(filenameStream.str(), player->getPlayerid(), reactor->getID(), reactor->getMapID());
				reactor->setState(revent.nextstate, false);
				reactor->kill();
				ReactorPacket::destroyReactor(Maps::info[player->getMap()].Players, reactor);
			}
		}
	}
}

void Reactors::checkDrop(Player *player, Drop *drop) {
	if (drop->getMesos())
		return;
	for (unsigned int i=0; i<reactors[drop->getMap()].size(); i++) {
		Reactor* reactor = reactors[drop->getMap()][i];
		if (reactor->getState() < maxstates[reactor->getReactorID()]) {
			ReactorEventInfo revent = reactorinfo[reactor->getReactorID()][reactor->getState()];
			if (revent.type == 100 && drop->getID() == revent.itemid) {
				if ((drop->getPos().x >= reactor->getPos().x+revent.ltx && drop->getPos().x <= reactor->getPos().x+revent.rbx) && (drop->getPos().y >= reactor->getPos().y+revent.lty && drop->getPos().y <= reactor->getPos().y+revent.rby)) {
					ReactionTimer::Instance()->setReactionTimer(player, reactor, drop, revent.nextstate, 3000);
				}
				return;
			}
		}
	}
}

void Reactors::checkLoot(Drop *drop) {
	ReactionTimer::Instance()->stop(drop);
}

void Reactor::setState(int state, bool is) {
	this->state = state;
	if (is)
		ReactorPacket::triggerReactor(Maps::info[this->getMapID()].Players, this);
}
