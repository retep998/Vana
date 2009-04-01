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
#include "Instance.h"
#include "Instances.h"
#include "LuaInstance.h"
#include "MapPacket.h"
#include "Map.h"
#include "Maps.h"
#include "Reactors.h"
#include "Timer/Container.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <functional>

using std::tr1::bind;

Instance::Instance(const string &name, int32_t map, int32_t playerid, int32_t time, bool persistent, bool showtimer) :
	m_name(name),
	m_max_players(0),
	m_timer_counter(0),
	m_persistent(persistent),
	m_show_timer(showtimer),
	m_timers(new Timer::Container),
	m_luainstance(new LuaInstance(name, playerid)),
	m_start(clock()),
	m_reset_on_destroy(false),
	m_marked_for_delete(false)
	{
		if (time < 0) {
			m_time = -(time + 1);
			Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
			new Timer::Timer(bind(&Instance::instanceEnd, this, true),
				id, getTimers(), Timer::Time::nthSecondOfHour(static_cast<uint16_t>(m_time)), m_persistent ? 3600000 : 0);
		}
		else if (time > 0) {
			m_time = time * 1000;
			Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
			new Timer::Timer(bind(&Instance::instanceEnd, this, true),
				id, getTimers(), Timer::Time::fromNow(m_time), m_persistent ? m_time : 0);
		}
	}
Instance::~Instance() {
	// Reactors
	if (m_reset_on_destroy) // Reset all reactors if bool is true
		resetAll();
	m_reactors.clear();

	// Maps
	for (size_t i = 0; i < getMapNum(); i++) {
		Map *map = m_maps[i];
		map->setInstance(0);
		map->clearDrops(false);
		map->killMobs(0, 0, false, false);
	}
	m_maps.clear();

	// Players
	for (unordered_map<int32_t, Player *>::iterator iter = m_players.begin(); iter != m_players.end(); iter++) {
		iter->second->setInstance(0);
	}
	m_players.clear();
	Instances::InstancePtr()->removeInstance(this);
	delete m_luainstance;
}

void Instance::deleteVariable(const string &name) {
	if (m_variables.find(name) != m_variables.end())
		m_variables.erase(name);
}

void Instance::setVariable(const string &name, const string &val) {
	m_variables[name] = val;
}

string Instance::getVariable(const string &name) {
	return (m_variables.find(name) == m_variables.end()) ? "" : m_variables[name];
}

void Instance::setBanned(const string &name, bool isbanned) {
	if (isbanned)
		m_banned.push_back(name);
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
	m_players[player->getId()] = player;
	m_players_order.push_back(player->getName());
	player->setInstance(this);
}

void Instance::removePlayer(Player *player) {
	removePlayer(player->getId());
	player->setInstance(0);
}

void Instance::removePlayer(int32_t id) {
	if (m_players.find(id) != m_players.end()) {
		m_players.erase(id);
	}
}

const string Instance::getPlayerByIndex(uint32_t index) const {
	index--;
	return m_players_order[(index > m_players_order.size() ? m_players_order.size() : index)];
}

bool Instance::instanceHasPlayers() const {
	for (size_t i = 0; i < m_maps.size(); i++) {
		if (m_maps[i]->getNumPlayers() != 0) {
			return true;
		}
	}
	return false;
}

void Instance::addReactor(Reactor *reactor) {
	m_reactors.push_back(reactor);
}

Reactor * Instance::getReactor(int32_t reactorid) {
	Reactor *reactor = 0;
	for (size_t i = 0; i < getReactorNum(); i++) {
		Reactor *treactor = m_reactors[i];
		if (treactor->getID() == reactorid) {
			reactor = treactor;
			break;
		}
	}
	return reactor;
}

size_t Instance::getReactorNum() {
	return m_reactors.size();
}

void Instance::resetAll() {
	for (size_t i = 0; i < getReactorNum(); i++) {
		Reactor *reactor = m_reactors[i];
		if (reactor->isAlive())
			reactor->kill();
		reactor->restore();
	}
}

void Instance::addMap(Map *map) {
	m_maps.push_back(map);
	map->setInstance(this);
}

void Instance::addMap(int32_t mapid) {
	Map *map = Maps::getMap(mapid);
	addMap(map);
}

Map * Instance::getMap(int32_t mapid) {
	Map *map = 0;
	for (size_t i = 0; i < getMapNum(); i++) {
		Map *tmap = m_maps[i];
		if (tmap->getInfo()->id == mapid) {
			map = tmap;
			break;
		}
	}
	return map;
}

size_t Instance::getMapNum() {
	return m_maps.size();
}

void Instance::setPlayerId(int32_t id) {
	m_luainstance->setVariable("playerid", id);
}

bool Instance::addTimer(const string &timername, const TimerAction &timer) {
	if (m_timer_actions.find(timername) == m_timer_actions.end()) {
		m_timer_actions[timername] = timer;
		Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterid);
		if (timer.time > 0) { // Positive, occurs in the future
			new Timer::Timer(bind(&Instance::timerEnd, this, timername, true),
				id, getTimers(), Timer::Time::fromNow(timer.time * 1000));
		}
		else { // Negative, occurs nth second of hour
			new Timer::Timer(bind(&Instance::timerEnd, this, timername, true),
				id, getTimers(), Timer::Time::nthSecondOfHour(static_cast<uint16_t>(-(timer.time + 1))));
		}
		return true;
	}
	return false;
}

int32_t Instance::checkTimer(const string &timername) {
	int32_t timeleft = 0;
	if (m_timer_actions.find(timername) != m_timer_actions.end()) {
		TimerAction timer = m_timer_actions[timername];
		Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterid);
		timeleft = getTimers()->checkTimer(id);
	}
	return timeleft;
}

void Instance::removeTimer(const string &timername) {
	if (m_timer_actions.find(timername) != m_timer_actions.end()) {
		TimerAction timer = m_timer_actions[timername];
		if (checkTimer(timername) > 0) {
			Timer::Id id(Timer::Types::InstanceTimer, timer.time, timer.counterid);
			getTimers()->removeTimer(id);
			sendMessage(TIMER_END, timername, false);
		}
		m_timer_actions.erase(timername);
	}
}

int32_t Instance::checkInstanceTimer() {
	int32_t timeleft = 0;
	if (m_time > 0) {
		Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
		timeleft = getTimers()->checkTimer(id) / 1000;
	}
	return timeleft;
}

void Instance::setInstanceTimer(int32_t time) {
	if (checkInstanceTimer() > 0) {
		Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
		getTimers()->removeTimer(id);
	}
	if (time < 0) {
		m_time = -(time + 1);
		Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
		new Timer::Timer(bind(&Instance::instanceEnd, this, true),
			id, getTimers(), Timer::Time::nthSecondOfHour(static_cast<uint16_t>(m_time)), m_persistent ? 3600000 : 0);
	}
	else if (time > 0) {
		m_time = time * 1000;
		Timer::Id id(Timer::Types::InstanceTimer, m_time, -1);
		new Timer::Timer(bind(&Instance::instanceEnd, this, true),
			id, getTimers(), Timer::Time::fromNow(m_time), m_persistent ? m_time : 0);
	}
}

void Instance::sendMessage(InstanceMessages message) {
	m_luainstance->run(message);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter) {
	m_luainstance->run(message, parameter);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter1, int32_t parameter2) {
	m_luainstance->run(message, parameter1, parameter2);
}

void Instance::sendMessage(InstanceMessages message, int32_t parameter1, int32_t parameter2, int32_t parameter3) {
	m_luainstance->run(message, parameter1, parameter2, parameter3);
}

void Instance::sendMessage(InstanceMessages message, const string &parameter1, int32_t parameter2) {
	m_luainstance->run(message, parameter1, parameter2);
}

void Instance::timerEnd(const string &name, bool fromTimer) {
	sendMessage(TIMER_NATURAL_END, name, fromTimer ? 1 : 0);
	removeTimer(name);
}

void Instance::instanceEnd(bool fromTimer) {
	sendMessage(INSTANCETIMER_NATURAL_END, fromTimer ? 1 : 0);
	if (!getPersistence()) {
		setMarkedForDelete(true);
	}
}

int32_t Instance::getCounterId() {
	return ++m_timer_counter;
}