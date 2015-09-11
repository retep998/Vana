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
#include "Instance.hpp"
#include "ChannelServer.hpp"
#include "Instances.hpp"
#include "LuaInstance.hpp"
#include "MapPacket.hpp"
#include "Map.hpp"
#include "Maps.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "Reactor.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include "TimerContainer.hpp"
#include "TimeUtilities.hpp"
#include <functional>
#include <sstream>
#include <utility>

namespace Vana {

Instance::Instance(const string_t &name, map_id_t map, player_id_t playerId, const duration_t &time, const duration_t &persistent, bool showTimer) :
	m_name{name},
	m_persistent{persistent},
	m_showTimer{showTimer},
	m_start{TimeUtilities::getNow()}
{
	m_variables = make_owned_ptr<Variables>();
	m_luaInstance = make_owned_ptr<LuaInstance>(name, playerId);

	if (playerId != 0) {
		ChannelServer::getInstance().log(LogType::InstanceBegin, [&](out_stream_t &log) { log << name << " started by player ID " << playerId; });
	}
	setInstanceTimer(time, true);
}

Instance::~Instance() {
	// Maps
	for (const auto &map : m_maps) {
		map->endInstance(m_resetOnDestroy);
	}
	m_maps.clear();

	// Parties
	for (const auto &party : m_parties) {
		party->setInstance(nullptr);
	}
	m_parties.clear();

	// Players
	for (const auto &kvp : m_players) {
		kvp.second->setInstance(nullptr);
	}
	m_players.clear();
	ChannelServer::getInstance().getInstances().removeInstance(this);
}

auto Instance::addPlayer(Player *player) -> void {
	if (player != nullptr) {
		m_players[player->getId()] = player;
		player->setInstance(this);
	}
}

auto Instance::removePlayer(Player *player) -> void {
	removePlayer(player->getId());
	player->setInstance(nullptr);
}

auto Instance::removePlayer(player_id_t id) -> void {
	auto kvp = m_players.find(id);
	if (kvp != std::end(m_players)) {
		m_players.erase(kvp);
	}
}

auto Instance::removeAllPlayers() -> void {
	auto copy = m_players;
	for (const auto &kvp : copy) {
		removePlayer(kvp.second);
	}
}

auto Instance::moveAllPlayers(map_id_t mapId, bool respectInstances, const PortalInfo * const portal) -> void {
	if (!Maps::getMap(mapId)) {
		return;
	}
	// Copy in the event that we don't respect instances
	auto copy = m_players;
	for (const auto &kvp : copy) {
		kvp.second->setMap(mapId, portal, respectInstances);
	}
}

auto Instance::getAllPlayerIds() -> vector_t<player_id_t> {
	vector_t<player_id_t> playerIds;
	for (const auto &kvp : m_players) {
		playerIds.push_back(kvp.first);
	}
	return playerIds;
}

auto Instance::instanceHasPlayers() const -> bool {
	for (const auto &map : m_maps) {
		if (map->getNumPlayers() != 0) {
			return true;
		}
	}
	return false;
}

auto Instance::addMap(Map *map) -> void {
	m_maps.push_back(map);
	map->setInstance(this);
}

auto Instance::addMap(map_id_t mapId) -> void {
	Map *map = Maps::getMap(mapId);
	addMap(map);
}

auto Instance::isInstanceMap(map_id_t mapId) const -> bool {
	for (const auto &map : m_maps) {
		if (map->getId() == mapId) {
			return true;
		}
	}
	return false;
}

auto Instance::addParty(Party *party) -> void {
	m_parties.push_back(party);
	party->setInstance(this);
}

auto Instance::addFutureTimer(const string_t &timerName, seconds_t time, seconds_t persistence) -> bool {
	if (timerName == "instance") {
		setInstanceTimer(time, false);
		return true;
	}

	if (m_timerActions.find(timerName) == std::end(m_timerActions)) {
		TimerAction timer;
		timer.counterId = getCounterId();
		timer.isPersistent = persistence.count() > 0;
		m_timerActions.emplace(timerName, timer);

		Timer::Id id{TimerType::InstanceTimer, timer.counterId};
		Timer::Timer::create([this, timerName](const time_point_t &now) { this->timerComplete(timerName, true); },
			id, getTimers(), time, persistence);

		return true;
	}
	return false;
}

auto Instance::addSecondOfHourTimer(const string_t &timerName, int16_t secondOfHour, seconds_t persistence) -> bool {
	if (m_timerActions.find(timerName) == std::end(m_timerActions)) {
		TimerAction timer;
		timer.counterId = getCounterId();
		timer.isPersistent = persistence.count() > 0;
		m_timerActions.emplace(timerName, timer);

		Timer::Id id{TimerType::InstanceTimer, timer.counterId};
		Timer::Timer::create([this, timerName](const time_point_t &now) { this->timerComplete(timerName, true); },
			id, getTimers(), TimeUtilities::getDistanceToNextOccurringSecondOfHour(secondOfHour), persistence);

		return true;
	}
	return false;
}

auto Instance::getTimerSecondsRemaining(const string_t &timerName) -> seconds_t {
	seconds_t timeLeft{0};
	auto kvp = m_timerActions.find(timerName);
	if (kvp != std::end(m_timerActions)) {
		auto &timer = kvp->second;
		Timer::Id id{TimerType::InstanceTimer, timer.counterId};
		timeLeft = getTimers()->getRemainingTime<seconds_t>(id);
	}
	return timeLeft;
}

auto Instance::removeTimer(const string_t &timerName) -> void {
	if (timerName == "instance") {
		instanceEnd(true, false);
		return;
	}
	removeTimer(timerName, true);
}

auto Instance::removeTimer(const string_t &timerName, bool performEvent) -> void {
	auto kvp = m_timerActions.find(timerName);
	if (kvp != std::end(m_timerActions)) {
		const TimerAction &timer = kvp->second;
		if (getTimerSecondsRemaining(timerName).count() > 0) {
			Timer::Id id{TimerType::InstanceTimer, timer.counterId};
			getTimers()->removeTimer(id);
			if (performEvent) {
				timerEnd(timerName, false);
			}
		}
		m_timerActions.erase(kvp);
	}
}

auto Instance::removeAllTimers() -> void {
	auto copy = m_timerActions;
	for (const auto &kvp : copy) {
		removeTimer(kvp.first);
	}
}

auto Instance::getInstanceSecondsRemaining() -> seconds_t {
	return getTimerSecondsRemaining("instance");
}

auto Instance::setInstanceTimer(const duration_t &time, bool firstRun) -> void {
	if (getInstanceSecondsRemaining().count() > 0) {
		removeTimer("instance", false);
	}

	if (time.count() != 0) {
		TimerAction timer;
		timer.counterId = getCounterId();
		timer.isPersistent = m_persistent.count() > 0;
		m_timerActions.emplace("instance", timer);

		Timer::Id id{TimerType::InstanceTimer, timer.counterId};
		Timer::Timer::create(
			[this](const time_point_t &now) {
				this->instanceEnd(false, true);
			},
			id,
			getTimers(),
			time,
			m_persistent);

		if (!firstRun && showTimer()) {
			showTimer(true, true);
		}
	}
}

auto Instance::beginInstance() -> Result {
	return callInstanceFunction("beginInstance");
}

auto Instance::playerDeath(player_id_t playerId) -> Result {
	return callInstanceFunction("playerDeath", playerId);
}

auto Instance::partyDisband(party_id_t partyId) -> Result {
	return callInstanceFunction("partyDisband", partyId);
}

auto Instance::timerEnd(const string_t &name, bool fromTimer) -> Result {
	return callInstanceFunction("timerEnd", name, fromTimer);
}

auto Instance::playerDisconnect(player_id_t playerId, bool isPartyLeader) -> Result {
	return callInstanceFunction("playerDisconnect", playerId, isPartyLeader);
}

auto Instance::removePartyMember(party_id_t partyId, player_id_t playerId) -> Result {
	return callInstanceFunction("partyRemoveMember", partyId, playerId);
}

auto Instance::mobDeath(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId) -> Result {
	return callInstanceFunction("mobDeath", mobId, mapMobId, mapId);
}

auto Instance::mobSpawn(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId) -> Result {
	return callInstanceFunction("mobSpawn", mobId, mapMobId, mapId);
}

auto Instance::playerChangeMap(player_id_t playerId, map_id_t newMapId, map_id_t oldMapId, bool isPartyLeader) -> Result {
	return callInstanceFunction("changeMap", playerId, newMapId, oldMapId, isPartyLeader);
}

auto Instance::friendlyMobHit(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId, int32_t mobHp, int32_t mobMaxHp) -> Result {
	return callInstanceFunction("friendlyHit", mobId, mapMobId, mapId, mobHp, mobMaxHp);
}

auto Instance::timerComplete(const string_t &name, bool fromTimer) -> void {
	timerEnd(name, fromTimer);
	if (!fromTimer || (fromTimer && !isTimerPersistent(name))) {
		removeTimer(name);
	}
}

auto Instance::instanceEnd(bool calledByLua, bool fromTimer) -> void {
	if (!calledByLua) {
		timerEnd("instance", fromTimer);
	}

	if (!fromTimer || (fromTimer && m_persistent.count() > 0)) {
		removeTimer("instance", false);
	}

	showTimer(false);
	if (m_persistent.count() == 0) {
		markForDelete();
	}
}

auto Instance::isTimerPersistent(const string_t &name) -> bool {
	auto iter = m_timerActions.find(name);
	return iter != std::end(m_timerActions) ?
		iter->second.isPersistent :
		false;
}

auto Instance::getCounterId() -> uint32_t {
	return ++m_timerCounter;
}

auto Instance::markForDelete() -> void {
	m_markedForDeletion = true;
	clearTimers();
	for (const auto &map : m_maps) {
		map->setInstance(nullptr);
	}
	for (const auto &player : m_players) {
		player.second->setInstance(nullptr);
	}
	for (const auto &party : m_parties) {
		party->setInstance(nullptr);
	}

	// TODO FIXME lua
	// TODO FIXME instance
	// All sorts of instance trickery needs to be cleaned up here...perhaps marking for deletion needs a full review
	// e.g. a crash can be caused by marking for delete and then going to a map where an instance was (e.g. Zakum signup)
}

auto Instance::respawnMobs(map_id_t mapId) -> void {
	if (mapId == Maps::NoMap) {
		for (const auto &map : m_maps) {
			map->respawn(SpawnTypes::Mob);
		}
	}
	else {
		Maps::getMap(mapId)->respawn(SpawnTypes::Mob);
	}
}

auto Instance::respawnReactors(map_id_t mapId) -> void {
	if (mapId == Maps::NoMap) {
		for (const auto &map : m_maps) {
			map->respawn(SpawnTypes::Reactor);
		}
	}
	else {
		Maps::getMap(mapId)->respawn(SpawnTypes::Reactor);
	}
}

auto Instance::showTimer(bool show, bool doIt) -> void {
	if (!show && (doIt || m_showTimer)) {
		for (const auto &map : m_maps) {
			map->send(Packets::Map::showTimer(seconds_t{0}));
		}
	}
	else if (show && (doIt || !m_showTimer)) {
		for (const auto &map : m_maps) {
			map->send(Packets::Map::showTimer(getInstanceSecondsRemaining()));
		}
	}
}

auto Instance::setPersistence(const duration_t &persistence) -> void {
	m_persistent = persistence;
}

auto Instance::getPersistence() const -> duration_t {
	return m_persistent;
}

auto Instance::showTimer() const -> bool {
	return m_showTimer;
}

}