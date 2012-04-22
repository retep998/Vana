/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "ReactorHandler.h"
#include "Drop.h"
#include "FileUtilities.h"
#include "GameLogicUtilities.h"
#include "LuaReactor.h"
#include "Maps.h"
#include "PacketReader.h"
#include "Player.h"
#include "Pos.h"
#include "Reactor.h"
#include "ReactorDataProvider.h"
#include "ReactorPacket.h"
#include "ScriptDataProvider.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include "TimerThread.h"
#include <functional>
#include <iostream>
#include <sstream>

using std::string;

void ReactorHandler::hitReactor(Player *player, PacketReader &packet) {
	uint32_t id = Map::makeReactorId(packet.get<uint32_t>());

	Reactor *reactor = Maps::getMap(player->getMap())->getReactor(id);

	if (reactor != nullptr && reactor->isAlive()) {
		ReactorData *data = ReactorDataProvider::Instance()->getReactorData(reactor->getReactorId(), true);
		if (data == nullptr) {
			// Not sure how this would happen, but whatever
			return;
		}
		if (reactor->getState() < (data->maxStates - 1)) {
			ReactorStateInfo *reactorEvent = &(data->states[reactor->getState()][0]); // There's only one way to hit something
			if (reactorEvent->nextState < (data->maxStates - 1)) {
				if (reactorEvent->type == 100) {
					return;
				}
				ReactorPacket::triggerReactor(reactor);
				reactor->setState(reactorEvent->nextState, true);
				return;
			}
			else {
				const string &filename = ScriptDataProvider::Instance()->getScript(reactor->getReactorId(), ScriptTypes::Reactor);

				if (FileUtilities::fileExists(filename)) {
					LuaReactor(filename, player->getId(), id, reactor->getMapId());
				}
				else {
					// Default action of dropping an item
					reactor->drop(player);
				}

				reactor->setState(reactorEvent->nextState, false);
				reactor->kill();
				Maps::getMap(reactor->getMapId())->removeReactor(id);
				ReactorPacket::destroyReactor(reactor);
			}
		}
	}
}

void ReactorHandler::touchReactor(Player *player, PacketReader &packet) {
	uint32_t id = Map::makeReactorId(packet.get<uint32_t>());
	bool isTouching = packet.getBool();

	Reactor *reactor = Maps::getMap(player->getMap())->getReactor(id);

	if (reactor != nullptr && reactor->isAlive()) {
		int8_t newState = reactor->getState() + (isTouching ? 1 : -1);
		ReactorPacket::triggerReactor(reactor);
		reactor->setState(newState, true);
	}
}

struct Reaction {
	void operator()() {
		reactor->setState(state, true);
		drop->removeDrop();
		const string &filename = ScriptDataProvider::Instance()->getScript(reactor->getReactorId(), ScriptTypes::Reactor);
		LuaReactor(filename, player->getId(), Map::makeReactorId(reactor->getId()), reactor->getMapId());
		return;
	}
	Reactor *reactor;
	Drop *drop;
	Player *player;
	int8_t state;
};

void ReactorHandler::checkDrop(Player *player, Drop *drop) {
	Reactor *reactor;
	Map *map = Maps::getMap(drop->getMap());
	for (size_t i = 0; i < map->getNumReactors(); ++i) {
		reactor = map->getReactor(i);
		ReactorData *data = ReactorDataProvider::Instance()->getReactorData(reactor->getReactorId(), true);
		if (data == nullptr) {
			// Not sure how this would happen, but whatever
			continue;
		}
		if (reactor->getState() < (data->maxStates - 1)) {
			ReactorStateInfo *reactorEvent;
			for (uint8_t j = 0; j < data->states[reactor->getState()].size(); ++j) {
				reactorEvent = &(data->states[reactor->getState()][j]);
				if (reactorEvent->type == 100 && drop->getObjectId() == reactorEvent->itemId) {
					if (GameLogicUtilities::isInBox(reactor->getPos(), reactorEvent->lt, reactorEvent->rb, drop->getPos())) {
						Reaction reaction;
						reaction.reactor = reactor;
						reaction.drop = drop;
						reaction.player = player;
						reaction.state = reactorEvent->nextState;

						Timer::Id id(Timer::Types::ReactionTimer, drop->getId(), 0);
						new Timer::Timer(reaction, id, 0, TimeUtilities::fromNow(3000));
					}
					return;
				}
			}
		}
	}
}

void ReactorHandler::checkLoot(Drop *drop) {
	Timer::Id id(Timer::Types::ReactionTimer, drop->getId(), 0);
	Timer::Thread::Instance()->getContainer()->removeTimer(id);
}