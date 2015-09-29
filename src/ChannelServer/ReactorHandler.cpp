/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "ReactorHandler.hpp"
#include "Common/FileUtilities.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/PacketReader.hpp"
#include "Common/Point.hpp"
#include "Common/ReactorDataProvider.hpp"
#include "Common/ScriptDataProvider.hpp"
#include "Common/TimeUtilities.hpp"
#include "Common/Timer.hpp"
#include "Common/TimerThread.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Drop.hpp"
#include "ChannelServer/LuaReactor.hpp"
#include "ChannelServer/Maps.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/Reactor.hpp"
#include "ChannelServer/ReactorPacket.hpp"
#include <functional>
#include <iostream>
#include <sstream>

namespace Vana {
namespace ChannelServer {

auto ReactorHandler::hitReactor(Player *player, PacketReader &reader) -> void {
	map_object_t id = Map::makeReactorId(reader.get<map_object_t>());

	Map *map = player->getMap();
	Reactor *reactor = map->getReactor(id);

	if (reactor != nullptr && reactor->isAlive()) {
		auto &data = ChannelServer::getInstance().getReactorDataProvider().getReactorData(reactor->getReactorId(), true);
		if (reactor->getState() < data.maxStates - 1) {
			const auto &reactorEvent = data.states.at(reactor->getState())[0]; // There's only one way to hit something
			if (reactorEvent.nextState < data.maxStates - 1) {
				if (reactorEvent.type == 100) {
					return;
				}
				map->send(Packets::triggerReactor(reactor));
				reactor->setState(reactorEvent.nextState, true);
				return;
			}
			else {
				auto &channel = ChannelServer::getInstance();
				string_t filename = channel.getScriptDataProvider().getScript(&channel, reactor->getReactorId(), ScriptTypes::Reactor);

				if (FileUtilities::fileExists(filename)) {
					LuaReactor{filename, player->getId(), id, reactor->getMapId()};
				}
				else {
					// Default action of dropping an item
					reactor->drop(player);
				}

				reactor->setState(reactorEvent.nextState, false);
				reactor->kill();
				map->removeReactor(id);
				map->send(Packets::destroyReactor(reactor));
			}
		}
	}
}

auto ReactorHandler::touchReactor(Player *player, PacketReader &reader) -> void {
	size_t id = Map::makeReactorId(reader.get<map_object_t>());
	bool isTouching = reader.get<bool>();

	Map *map = player->getMap();
	Reactor *reactor = map->getReactor(id);

	if (reactor != nullptr && reactor->isAlive()) {
		int8_t newState = reactor->getState() + (isTouching ? 1 : -1);
		map->send(Packets::triggerReactor(reactor));
		reactor->setState(newState, true);
	}
}

struct Reaction {
	auto operator()(const time_point_t &now) -> void {
		reactor->setState(state, true);
		drop->removeDrop();
		auto &channel = ChannelServer::getInstance();
		string_t filename = channel.getScriptDataProvider().getScript(&channel, reactor->getReactorId(), ScriptTypes::Reactor);
		// TODO FIXME reactor
		// Not sure if this reactor identifier dispatch is correct
		LuaReactor{filename, player->getId(), static_cast<map_object_t>(Map::makeReactorId(reactor->getId())), reactor->getMapId()};
	}

	Reactor *reactor = nullptr;
	Drop *drop = nullptr;
	Player *player = nullptr;
	int8_t state = 0;
};

auto ReactorHandler::checkDrop(Player *player, Drop *drop) -> void {
	Reactor *reactor;
	Map *map = drop->getMap();
	for (size_t i = 0; i < map->getNumReactors(); ++i) {
		reactor = map->getReactor(i);
		auto &data = ChannelServer::getInstance().getReactorDataProvider().getReactorData(reactor->getReactorId(), true);
		if (reactor->getState() < data.maxStates - 1) {
			for (const auto &reactorEvent : data.states.at(reactor->getState())) {
				if (reactorEvent.type == 100 && drop->getObjectId() == reactorEvent.itemId) {
					if (reactorEvent.dimensions.move(reactor->getPos()).contains(drop->getPos())) {
						Reaction reaction;
						reaction.reactor = reactor;
						reaction.drop = drop;
						reaction.player = player;
						reaction.state = reactorEvent.nextState;

						Vana::Timer::Id id{TimerType::ReactionTimer, drop->getId()};
						Vana::Timer::Timer::create(reaction, id, nullptr, seconds_t{3});
					}
					return;
				}
			}
		}
	}
}

auto ReactorHandler::checkLoot(Drop *drop) -> void {
	Vana::Timer::Id id{TimerType::ReactionTimer, drop->getId()};
	Vana::Timer::TimerThread::getInstance().getTimerContainer()->removeTimer(id);
}

}
}