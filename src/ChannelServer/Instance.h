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
#pragma once

#include "InstanceMessageConstants.h"
#include "Types.h"
#include "Variables.h"
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <vector>

using boost::scoped_ptr;
using std::string;
using std::tr1::unordered_map;
using std::vector;

class LuaInstance;
class Map;
class Party;
class Player;
class Reactor;
struct PortalInfo;

namespace Timer {
	class Container;
};

struct TimerAction {
	TimerAction() : persistent(0) { }
	int32_t counterid;
	int32_t time; // How long the timer lasts, negative integers indicate second of hour (-1 to -3600)
	int32_t persistent; // How often does it repeat?
};

class Instance {
public:
	Instance(const string &name, int32_t map, int32_t playerId, int32_t time, int32_t persistent, bool showTimer, bool appLaunch = false);
	~Instance();

	string getName() const { return m_name; }
	uint32_t getStart() const { return m_start; }
	int32_t getCounterId();
	bool getMarkedForDelete() const { return m_marked_for_delete; }
	void markForDelete();
	void instanceEnd(bool fromTimer = false);
	Variables * getVariables() const { return m_variables.get(); }

	// Players
	vector<int32_t> getAllPlayerIds();
	const string getPlayerByIndex(uint32_t index) const;
	const string getBannedPlayerByIndex(uint32_t index) const;
	void setMaxPlayers(int32_t maxplayers) { m_max_players = maxplayers; }
	void addPlayer(Player *player);
	void removePlayer(Player *player);
	void removePlayer(int32_t id);
	void removeAllPlayers();
	void setBanned(const string &name, bool isbanned);
	void addPlayerSignUp(Player *player);
	void removePlayerSignUp(const string &name);
	void moveAllPlayers(int32_t mapid, bool respectInstances, PortalInfo *portal = 0);
	bool isPlayerSignedUp(const string &name);
	bool isBanned(const string &name);
	bool instanceHasPlayers() const;
	int32_t getMaxPlayers() const { return m_max_players; }
	size_t getPlayerNum() const { return m_players.size(); }
	size_t getPlayerSignupNum() const { return m_players_order.size(); } // Number of players for the instance (squads, etc.)
	size_t getBannedPlayerNum() const { return m_banned.size(); }

	// Maps
	void addMap(Map *map);
	void addMap(int32_t mapid);
	Map * getMap(int32_t mapid);
	size_t getMapNum();
	void setResetAtEnd(bool reset) { m_reset_on_destroy = reset; }
	void respawnMobs(int32_t mapid);
	void respawnReactors(int32_t mapid);

	// Parties
	void addParty(Party *party);

	// Instance time
	bool hasInstanceTimer() const { return m_time > 0; }
	void setInstanceTimer(int32_t time, bool firstrun = false);
	void setPersistence(int32_t p) { m_persistent = p; }
	int32_t getPersistence() const { return m_persistent; }
	int32_t checkInstanceTimer();
	bool showTimer() const { return m_show_timer; }
	void showTimer(bool show, bool doit = false);

	// Timers
	void removeAllTimers();
	void removeTimer(const string &name);
	void timerEnd(const string &name, bool fromTimer = false);
	bool addTimer(const string &name, const TimerAction &timer);
	bool isTimerPersistent(const string &name);
	int32_t checkTimer(const string &name);
	Timer::Container * getTimers() const { return m_timers.get(); }

	// Lua interaction
	void sendMessage(InstanceMessages message);
	void sendMessage(InstanceMessages message, int32_t);
	void sendMessage(InstanceMessages message, int32_t, int32_t);
	void sendMessage(InstanceMessages message, int32_t, int32_t, int32_t);
	void sendMessage(InstanceMessages message, int32_t, int32_t, int32_t, int32_t);
	void sendMessage(InstanceMessages message, int32_t, int32_t, int32_t, int32_t, int32_t);
	void sendMessage(InstanceMessages message, const string &, int32_t);
private:
	scoped_ptr<Timer::Container> m_timers; // Timer container for the instance
	scoped_ptr<Variables> m_variables;
	scoped_ptr<LuaInstance> m_luainstance; // Lua instance for interacting with scripts

	unordered_map<string, TimerAction> m_timer_actions; // Timers indexed by name
	unordered_map<int32_t, Player *> m_players;

	vector<string> m_banned; // For squads
	vector<string> m_players_order; // For squads
	vector<Map *> m_maps;
	vector<Party *> m_parties;

	uint32_t m_start; // Tick count when instance started
	string m_name; // Identification for the instance
	int32_t m_max_players; // Maximum players allowed for instance
	int32_t m_time; // Instance time
	int32_t m_timer_counter; // Used for uniqueness of timer IDs
	int32_t m_persistent; // How often does instance repeat?
	bool m_show_timer; // Show timer
	bool m_reset_on_destroy; // Reset reactors when done
	bool m_marked_for_delete; // End of instance time

	LuaInstance * getLuaInstance() { return m_luainstance.get(); }
};