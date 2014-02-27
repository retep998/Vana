/*
Copyright (C) 2008-2014 Vana Development Team

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

Instance::Instance(const string_t &name, map_id_t map, player_id_t playerId, const duration_t &time, const duration_t &persistent, bool showTimer, bool appLaunch) :
	m_name(name),
	m_persistent(persistent),
	m_showTimer(showTimer),
	m_start(TimeUtilities::getNow())
{
	m_variables = make_owned_ptr<Variables>();
	m_luaInstance = make_owned_ptr<LuaInstance>(name, playerId);

	if (!appLaunch) {
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
	Instances::getInstance().removeInstance(this);
}

auto Instance::getBannedPlayerByIndex(uint32_t index) const -> const string_t {
	--index;
	return m_banned[(index > m_banned.size() ? m_banned.size() : index)];
}

auto Instance::setBanned(const string_t &name, bool isBanned) -> void {
	if (isBanned) {
		m_banned.push_back(name);
		removePlayerSignUp(name);
	}
	else {
		for (size_t i = 0; i < m_banned.size(); ++i) {
			if (m_banned[i] == name) {
				m_banned.erase(std::begin(m_banned) + i);
				break;
			}
		}
	}
}

auto Instance::isBanned(const string_t &name) -> bool {
	for (const auto &bannedName : m_banned) {
		if (bannedName == name) {
			return true;
		}
	}
	return false;
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

auto Instance::addPlayerSignUp(Player *player) -> void {
	m_playersOrder.push_back(player->getName());
}

auto Instance::removePlayerSignUp(const string_t &name) -> void {
	for (size_t i = 0; i < m_playersOrder.size(); ++i) {
		if (m_playersOrder[i] == name) {
			m_playersOrder.erase(std::begin(m_playersOrder) + i);
		}
	}
}

auto Instance::moveAllPlayers(map_id_t mapId, bool respectInstances, PortalInfo *portal) -> void {
	if (!Maps::getMap(mapId)) {
		return;
	}
	// Copy in the event that we don't respect instances
	auto copy = m_players;
	for (const auto &kvp : copy) {
		kvp.second->setMap(mapId, portal, respectInstances);
	}
}

auto Instance::isPlayerSignedUp(const string_t &name) -> bool {
	for (const auto &playerName : m_playersOrder) {
		if (playerName == name) {
			return true;
		}
	}
	return false;
}

auto Instance::getAllPlayerIds() -> vector_t<player_id_t> {
	vector_t<player_id_t> playerIds;
	for (const auto &kvp : m_players) {
		playerIds.push_back(kvp.first);
	}
	return playerIds;
}

auto Instance::getPlayerByIndex(uint32_t index) const -> const string_t {
	--index;
	return m_playersOrder[(index > m_playersOrder.size() ? m_playersOrder.size() : index)];
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

auto Instance::addTimer(const string_t &timerName, const TimerAction &timer) -> bool {
	if (m_timerActions.find(timerName) == std::end(m_timerActions)) {
		m_timerActions.emplace(timerName, timer);

		Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterId);
		if (timer.time > 0) {
			// Positive, occurs in the future
			Timer::Timer::create([this, timerName](const time_point_t &now) { this->timerComplete(timerName, true); },
				id, getTimers(), seconds_t(timer.time), seconds_t(timer.persistent));
		}
		else {
			// Negative, occurs nth second of hour
			Timer::Timer::create([this, timerName](const time_point_t &now) { this->timerComplete(timerName, true); },
				id, getTimers(), TimeUtilities::getDistanceToNextOccurringSecondOfHour(static_cast<uint16_t>(-(timer.time + 1))), seconds_t(timer.persistent));
		}
		return true;
	}
	return false;
}

auto Instance::getTimerSecondsRemaining(const string_t &timerName) -> seconds_t {
	seconds_t timeLeft(0);
	auto kvp = m_timerActions.find(timerName);
	if (kvp != std::end(m_timerActions)) {
		auto &timer = kvp->second;
		Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterId);
		timeLeft = getTimers()->getRemainingTime<seconds_t>(id);
	}
	return timeLeft;
}

auto Instance::removeTimer(const string_t &timerName) -> void {
	auto kvp = m_timerActions.find(timerName);
	if (kvp != std::end(m_timerActions)) {
		const TimerAction &timer = kvp->second;
		if (getTimerSecondsRemaining(timerName).count() > 0) {
			Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterId);
			getTimers()->removeTimer(id);
			timerComplete(timerName, false);
		}
		m_timerActions.erase(kvp);
	}
}

auto Instance::removeAllTimers() -> void {
	for (const auto &kvp : m_timerActions) {
		removeTimer(kvp.first);
	}
	setInstanceTimer(seconds_t(0));
}

auto Instance::checkInstanceTimer() -> seconds_t {
	seconds_t timeLeft(0);
	if (m_time.count() > 0) {
		Timer::Id id = getTimerId();
		timeLeft = getTimers()->getRemainingTime<seconds_t>(id);
	}
	return timeLeft;
}

auto Instance::setInstanceTimer(const duration_t &time, bool firstRun) -> void {
	if (checkInstanceTimer().count() > 0) {
		Timer::Id id = getTimerId();
		getTimers()->removeTimer(id);
	}
	if (time.count() != 0) {
		m_time = duration_cast<seconds_t>(time);

		Timer::Timer::create([this](const time_point_t &now) { this->instanceEnd(true); },
			getTimerId(),
			getTimers(), m_time, m_persistent);

		if (!firstRun && showTimer()) {
			showTimer(true, true);
		}
	}
}

auto Instance::beginInstance() -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("beginInstance")) {
		return Result::Failure;
	}
	return luaInst->call("beginInstance");
}

auto Instance::playerDeath(player_id_t playerId) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("playerDeath")) {
		return Result::Failure;
	}
	return luaInst->call("playerDeath", playerId);
}

auto Instance::instanceTimerEnd(bool fromTimer) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("instanceTimerEnd")) {
		return Result::Failure;
	}
	return luaInst->call("instanceTimerEnd", fromTimer);
}

auto Instance::partyDisband(party_id_t partyId) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("partyDisband")) {
		return Result::Failure;
	}
	return luaInst->call("partyDisband", partyId);
}

auto Instance::timerEnd(const string_t &name, bool fromTimer) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("timerEnd")) {
		return Result::Failure;
	}
	return luaInst->call("timerEnd", name, fromTimer);
}

auto Instance::playerDisconnect(player_id_t playerId, bool isPartyLeader) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("playerDisconnect")) {
		return Result::Failure;
	}
	return luaInst->call("playerDisconnect", playerId, isPartyLeader);
}

auto Instance::removePartyMember(party_id_t partyId, player_id_t playerId) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("partyRemoveMember")) {
		return Result::Failure;
	}
	return luaInst->call("partyRemoveMember", partyId, playerId);
}

auto Instance::mobDeath(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("mobDeath")) {
		return Result::Failure;
	}
	return luaInst->call("mobDeath", mobId, mapMobId, mapId);
}

auto Instance::mobSpawn(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("mobSpawn")) {
		return Result::Failure;
	}
	return luaInst->call("mobSpawn", mobId, mapMobId, mapId);
}

auto Instance::playerChangeMap(player_id_t playerId, map_id_t newMapId, map_id_t oldMapId, bool isPartyLeader) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("changeMap")) {
		return Result::Failure;
	}
	return luaInst->call("changeMap", playerId, newMapId, oldMapId, isPartyLeader);
}

auto Instance::friendlyMobHit(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId, int32_t mobHp, int32_t mobMaxHp) -> Result {
	auto luaInst = getLuaInstance();
	if (!luaInst->exists("friendlyHit")) {
		return Result::Failure;
	}
	return luaInst->call("friendlyHit", mobId, mapMobId, mapId, mobHp, mobMaxHp);
}

auto Instance::timerComplete(const string_t &name, bool fromTimer) -> void {
	timerEnd(name, fromTimer);
	if (!fromTimer || (fromTimer && !isTimerPersistent(name))) {
		removeTimer(name);
	}
}

auto Instance::instanceEnd(bool fromTimer) -> void {
	instanceTimerEnd(fromTimer);
	showTimer(false);
	if (getPersistence().count() == 0) {
		markForDelete();
	}
}

auto Instance::isTimerPersistent(const string_t &name) -> bool {
	return m_timerActions.find(name) != std::end(m_timerActions) ? (m_timerActions[name].persistent > 0) : false;
}

auto Instance::getCounterId() -> int32_t {
	return ++m_timerCounter;
}

auto Instance::markForDelete() -> void {
	m_markedForDeletion = true;
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
			map->send(MapPacket::showTimer(seconds_t(0)));
		}
	}
	else if (show && (doIt || !m_showTimer)) {
		for (const auto &map : m_maps) {
			map->send(MapPacket::showTimer(checkInstanceTimer()));
		}
	}
}

auto Instance::getTimerId() const -> Timer::Id {
	return Timer::Id(Timer::Types::InstanceTimer, static_cast<int32_t>(m_time.count()), -1);
}