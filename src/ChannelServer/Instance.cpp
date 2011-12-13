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
#include "Instance.h"
#include "ChannelServer.h"
#include "Instances.h"
#include "LuaInstance.h"
#include "MapPacket.h"
#include "Map.h"
#include "Maps.h"
#include "Party.h"
#include "Player.h"
#include "Reactor.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include "TimerContainer.h"
#include "TimeUtilities.h"
#include <functional>
#include <sstream>

using std::tr1::bind;

Instance::Instance(const string &name, int32_t map, int32_t playerId, int32_t time, int32_t persistent, bool showTimer, bool appLaunch) :
	m_name(name),
	m_maxPlayers(0),
	m_timerCounter(0),
	m_persistent(persistent),
	m_showTimer(showTimer),
	m_timers(new Timer::Container),
	m_variables(new Variables),
	m_luaInstance(new LuaInstance(name, playerId)),
	m_start(TimeUtilities::getTickCount()),
	m_resetOnDestroy(true),
	m_markedForDeletion(false)
{
	if (!appLaunch) {
		std::ostringstream x;
		x << name << " started by player " << playerId;
		ChannelServer::Instance()->log(LogTypes::InstanceBegin, x.str());
	}
	setInstanceTimer(time, true);
}

Instance::~Instance() {
	// Maps
	for (size_t i = 0; i < getMapNum(); i++) {
		Map *map = m_maps[i];
		map->setInstance(nullptr);
		map->setMusic("default");
		map->clearDrops(false);
		map->killMobs(0, 0, false, false);
		map->killReactors(false);
		if (m_resetOnDestroy) { // Reset all mobs/reactors
			map->respawn();
		}
	}
	m_maps.clear();

	// Parties
	for (size_t k = 0; k < m_parties.size(); k++) {
		Party *p = m_parties[k];
		if (p != nullptr) {
			p->setInstance(nullptr);
		}
	}
	m_parties.clear();

	// Players
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		iter->second->setInstance(nullptr);
	}
	m_players.clear();
	Instances::InstancePtr()->removeInstance(this);
}

const string Instance::getBannedPlayerByIndex(uint32_t index) const {
	index--;
	return m_banned[(index > m_banned.size() ? m_banned.size() : index)];
}

void Instance::setBanned(const string &name, bool isbanned) {
	if (isbanned) {
		m_banned.push_back(name);
		removePlayerSignUp(name);
	}
	else {
		for (size_t i = 0; i < m_banned.size(); i++) {
			if (m_banned[i] == name) {
				m_banned.erase(m_banned.begin() + i);
				break;
			}
		}
	}
}

bool Instance::isBanned(const string &name) {
	for (size_t i = 0; i < m_banned.size(); i++) {
		if (m_banned[i] == name) {
			return true;
		}
	}
	return false;
}

void Instance::addPlayer(Player *player) {
	if (player != nullptr) {
		m_players[player->getId()] = player;
		player->setInstance(this);
	}
}

void Instance::removePlayer(Player *player) {
	removePlayer(player->getId());
	player->setInstance(nullptr);
}

void Instance::removePlayer(int32_t id) {
	if (m_players.find(id) != m_players.end()) {
		m_players.erase(id);
	}
}

void Instance::removeAllPlayers() {
	unordered_map<int32_t, Player *> temp = m_players;
	for (unordered_map<int32_t, Player *>::iterator iter = temp.begin(); iter != temp.end(); iter++) {
		removePlayer(iter->second);
	}
}

void Instance::addPlayerSignUp(Player *player) {
	m_playersOrder.push_back(player->getName());
}

void Instance::removePlayerSignUp(const string &name) {
	for (size_t i = 0; i < m_playersOrder.size(); i++) {
		if (m_playersOrder[i] == name) {
			m_playersOrder.erase(m_playersOrder.begin() + i);
		}
	}
}

void Instance::moveAllPlayers(int32_t mapId, bool respectInstances, PortalInfo *portal) {
	if (!Maps::getMap(mapId))
		return;

	unordered_map<int32_t, Player *> tmp = m_players; // Copy in the event that we don't respect instances
	for (unordered_map<int32_t, Player *>::iterator iter = tmp.begin(); iter != tmp.end(); iter++) {
		iter->second->setMap(mapId, portal, respectInstances);

	}
}

bool Instance::isPlayerSignedUp(const string &name) {
	for (size_t i = 0; i < m_playersOrder.size(); i++) {
		if (m_playersOrder[i] == name) {
			return true;
		}
	}
	return false;
}

vector<int32_t> Instance::getAllPlayerIds() {
	vector<int32_t> playerIds;
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		playerIds.push_back(iter->first);
	}
	return playerIds;
}

const string Instance::getPlayerByIndex(uint32_t index) const {
	index--;
	return m_playersOrder[(index > m_playersOrder.size() ? m_playersOrder.size() : index)];
}

bool Instance::instanceHasPlayers() const {
	for (size_t i = 0; i < m_maps.size(); i++) {
		if (m_maps[i]->getNumPlayers() != 0) {
			return true;
		}
	}
	return false;
}

void Instance::addMap(Map *map) {
	m_maps.push_back(map);
	map->setInstance(this);
}

void Instance::addMap(int32_t mapId) {
	Map *map = Maps::getMap(mapId);
	addMap(map);
}

Map * Instance::getMap(int32_t mapId) {
	Map *map = nullptr;
	for (size_t i = 0; i < getMapNum(); i++) {
		Map *tmap = m_maps[i];
		if (tmap->getId() == mapId) {
			map = tmap;
			break;
		}
	}
	return map;
}

size_t Instance::getMapNum() {
	return m_maps.size();
}

void Instance::addParty(Party *party) {
	m_parties.push_back(party);
	party->setInstance(this);
}

bool Instance::addTimer(const string &timerName, const TimerAction &timer) {
	if (m_timerActions.find(timerName) == m_timerActions.end()) {
		m_timerActions[timerName] = timer;
		Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterId);
		if (timer.time > 0) {
			// Positive, occurs in the future
			new Timer::Timer(bind(&Instance::timerEnd, this, timerName, true),
				id, getTimers(), TimeUtilities::fromNow(timer.time * 1000), timer.persistent * 1000);
		}
		else {
			// Negative, occurs nth second of hour
			new Timer::Timer(bind(&Instance::timerEnd, this, timerName, true),
				id, getTimers(), TimeUtilities::nthSecondOfHour(static_cast<uint16_t>(-(timer.time + 1))), timer.persistent * 1000);
		}
		return true;
	}
	return false;
}

int32_t Instance::checkTimer(const string &timerName) {
	int32_t timeLeft = 0;
	if (m_timerActions.find(timerName) != m_timerActions.end()) {
		TimerAction timer = m_timerActions[timerName];
		Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterId);
		timeLeft = getTimers()->checkTimer(id);
	}
	return timeLeft;
}

void Instance::removeTimer(const string &timerName) {
	if (m_timerActions.find(timerName) != m_timerActions.end()) {
		TimerAction timer = m_timerActions[timerName];
		if (checkTimer(timerName) > 0) {
			Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterId);
			getTimers()->removeTimer(id);
			sendMessage(TimerEnd, timerName, false);
		}
		m_timerActions.erase(timerName);
	}
}

void Instance::removeAllTimers() {
	for (unordered_map<string, TimerAction>::iterator iter = m_timerActions.begin(); iter != m_timerActions.end(); ++iter) {
		removeTimer(iter->first);
	}
	setInstanceTimer(0);
}

int32_t Instance::checkInstanceTimer() {
	int32_t timeLeft = 0;
	if (m_time > 0) {
		Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
		timeLeft = getTimers()->checkTimer(id);
	}
	return timeLeft;
}

void Instance::setInstanceTimer(int32_t time, bool firstRun) {
	if (checkInstanceTimer() > 0) {
		Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
		getTimers()->removeTimer(id);
	}
	if (time != 0) {
		int64_t runat = 0;
		if (time < 0) {
			m_time = -(time + 1);
			runat = TimeUtilities::nthSecondOfHour(static_cast<uint16_t>(m_time));
		}
		else if (time > 0) {
			m_time = time * 1000;
			runat = TimeUtilities::fromNow(m_time);
		}

		new Timer::Timer(bind(&Instance::instanceEnd, this, true),
			Timer::Id(Timer::Types::InstanceTimer, m_time, -1),
			getTimers(), runat, m_persistent * 1000);

		if (!firstRun && showTimer()) {
			showTimer(true, true);
		}
	}
}

void Instance::sendMessage(InstanceMessages message) {
	getLuaInstance()->run(message);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter) {
	getLuaInstance()->run(message, parameter);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter1, int32_t parameter2) {
	getLuaInstance()->run(message, parameter1, parameter2);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3) {
	getLuaInstance()->run(message, parameter1, parameter2, parameter3);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3, int32_t parameter4) {
	getLuaInstance()->run(message, parameter1, parameter2, parameter3, parameter4);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3, int32_t parameter4, int32_t parameter5) {
	getLuaInstance()->run(message, parameter1, parameter2, parameter3, parameter4, parameter5);
}

void Instance::sendMessage(InstanceMessages message, const string &parameter1, int32_t parameter2) {
	getLuaInstance()->run(message, parameter1, parameter2);
}

void Instance::timerEnd(const string &name, bool fromTimer) {
	sendMessage(TimerNaturalEnd, name, fromTimer ? 1 : 0);
	if (!fromTimer || (fromTimer && !isTimerPersistent(name))) {
		removeTimer(name);
	}
}

void Instance::instanceEnd(bool fromTimer) {
	sendMessage(InstanceTimerNaturalEnd, fromTimer ? 1 : 0);
	showTimer(false);
	if (!getPersistence()) {
		markForDelete();
	}
}

bool Instance::isTimerPersistent(const string &name) {
	return (m_timerActions.find(name) != m_timerActions.end() ? (m_timerActions[name].persistent > 0) : false);
}

int32_t Instance::getCounterId() {
	return ++m_timerCounter;
}

void Instance::markForDelete() {
	m_markedForDeletion = true;
}

void Instance::respawnMobs(int32_t mapId) {
	if (mapId == Maps::NoMap) {
		for (size_t i = 0; i < getMapNum(); i++) {
			m_maps[i]->respawn(SpawnTypes::Mob);
		}
	}
	else {
		Maps::getMap(mapId)->respawn(SpawnTypes::Mob);
	}
}

void Instance::respawnReactors(int32_t mapId) {
	if (mapId == Maps::NoMap) {
		for (size_t i = 0; i < getMapNum(); i++) {
			m_maps[i]->respawn(SpawnTypes::Reactor);
		}
	}
	else {
		Maps::getMap(mapId)->respawn(SpawnTypes::Reactor);
	}
}

void Instance::showTimer(bool show, bool doit) {
	if (!show && (doit || m_showTimer)) {
		for (size_t i = 0; i < getMapNum(); i++) {
			MapPacket::showTimer(m_maps[i]->getId(), 0);
		}
	}
	else if (show && (doit || !m_showTimer)) {
		for (size_t i = 0; i < getMapNum(); i++) {
			MapPacket::showTimer(m_maps[i]->getId(), checkInstanceTimer());
		}
	}
}