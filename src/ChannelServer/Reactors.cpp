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
#include "Drops.h"
#include "Inventory.h"
#include "InventoryPacket.h"
#include "LoopingId.h"
#include "LuaReactor.h"
#include "Maps.h"
#include "Mobs.h"
#include "PacketCreator.h"
#include "Player.h"
#include "Pos.h"
#include "ReactorPacket.h"
#include "ReadPacket.h"
#include "Timer/Thread.h"
#include "Timer/Timer.h"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

// Reactor class
Reactor::Reactor (int32_t mapid, int32_t reactorid, Pos pos) : mapid(mapid), reactorid(reactorid), pos(pos), alive(true), state(0) {
	Maps::maps[mapid]->addReactor(this);
}

void Reactor::setState(char state, bool is) {
	this->state = state;
	if (is)
		ReactorPacket::triggerReactor(this);
}

void Reactor::restore() {
	revive();
	setState(0, false);
	ReactorPacket::spawnReactor(this);
}

void Reactor::drop(Player *player) {
	if (Drops::dropdata.find(reactorid) != Drops::dropdata.end()) {
		Drops::doDrops(player, reactorid, pos);
	}
}

// Reactors namespace
unordered_map<int32_t, ReactorEventsInfo> Reactors::reactorinfo;
unordered_map<int32_t, int16_t> Reactors::maxstates;

void Reactors::addEventInfo(int32_t id, ReactorEventInfo revent) {
	reactorinfo[id].push_back(revent);
}

void Reactors::setMaxstates(int32_t id, int16_t state) {
	maxstates[id] = state;
}

void Reactors::hitReactor(Player *player, ReadPacket *packet) {
	int32_t id = packet->getInt() - 200;

	Reactor *reactor = Maps::maps[player->getMap()]->getReactor(id);

	if (reactor != 0 && reactor->isAlive()) {
		if (reactor->getState() < maxstates[reactor->getReactorID()]) {
			ReactorEventInfo *revent = &reactorinfo[reactor->getReactorID()][reactor->getState()];
			if (revent->nextstate < maxstates[reactor->getReactorID()]) {
				if (revent->type >= 100)
					return;
				ReactorPacket::triggerReactor(reactor);
				reactor->setState(revent->nextstate, true);
				return;
			}
			else {
				std::ostringstream filenameStream;
				filenameStream << "scripts/reactors/" << reactor->getReactorID() << ".lua";
				string filename = filenameStream.str();

				struct stat fileInfo;
				if (!stat(filename.c_str(), &fileInfo)) { // Script found
					LuaReactor(filenameStream.str(), player->getId(), id, reactor->getMapID());
				}
				else { // Default action of dropping an item
					reactor->drop(player);
				}

				reactor->setState(revent->nextstate, false);
				reactor->kill();
				Maps::maps[reactor->getMapID()]->addReactorRespawn(ReactorRespawnInfo(id, clock()));
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
					struct {
						void operator()() {
							reactor->setState(state, true);
							drop->removeDrop();
							std::ostringstream filenameStream;
							filenameStream << "scripts/reactors/" << reactor->getReactorID() << ".lua";
							LuaReactor(filenameStream.str(), player->getId(), reactor->getID(), reactor->getMapID());
							return;
						}
						Reactor *reactor;
						Drop *drop;
						Player *player;
						char state;
					} reaction = {reactor, drop, player, revent->nextstate};

					Timer::Id id(Timer::Types::ReactionTimer, (uint32_t) drop, 0);
					new Timer::Timer(reaction, id, 0, 3000, false);
				}
				return;
			}
		}
	}
}

void Reactors::checkLoot(Drop *drop) {
	Timer::Id id(Timer::Types::ReactionTimer, (uint32_t) drop, 0);
	Timer::Thread::Instance()->getContainer()->removeTimer(id);
}
