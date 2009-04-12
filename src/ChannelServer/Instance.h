/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef INSTANCE_H
#define INSTANCE_H

#include "InstanceMessageConstants.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <vector>

class LuaInstance;
class Map;
class Party;
class Player;
class Reactor;

using std::tr1::unordered_map;
using std::string;
using std::vector;

namespace Timer {
	class Container;
};

struct TimerAction {
	int32_t counterid;
	int32_t time; // How long the timer lasts, negative integers indicate second of hour (-1 to -3601)
	bool persistent; // Does it repeat?
};

class Instance {
public:
	Instance(const string &name, int32_t map, int32_t playerid, int32_t time, bool persistent, bool showtimer);
	~Instance();

	clock_t getStart() const { return m_start; }
	string getName() const { return m_name; }
	bool getMarkedForDelete() const { return m_marked_for_delete; }
	void setMarkedForDelete(bool mark) { m_marked_for_delete = mark; }
	void instanceEnd(bool fromTimer = false);
	int32_t getCounterId();

	// Variables
	void deleteVariable(const string &name);
	void setVariable(const string &name, const string &val);
	string getVariable(const string &name);

	// Players
	const string getPlayerByIndex(uint32_t index) const;
	void setMaxPlayers(int32_t maxplayers) { m_max_players = maxplayers; }
	void addPlayer(Player *player);
	void removePlayer(Player *player);
	void removePlayer(int32_t id);
	void removeAllPlayers();
	void setBanned(const string &name, bool isbanned);
	void addPlayerSignUp(Player *player);
	void removePlayerSignUp(const string &name);
	void moveAllPlayers(int32_t mapid);
	bool isPlayerSignedUp(const string &name);
	bool isBanned(const string &name);
	bool instanceHasPlayers() const;
	int32_t getMaxPlayers() const { return m_max_players; }
	size_t getPlayerNum() const { return m_players.size(); }
	size_t getPlayerSignupNum() const { return m_players_order.size(); } // Number of players for the instance (squads, etc.)

	// Maps
	void addMap(Map *map);
	void addMap(int32_t mapid);
	Map * getMap(int32_t mapid);
	size_t getMapNum();

	// Reactors
	void addReactor(Reactor *reactor);
	void resetAll();
	void setResetAtEnd(bool reset) { m_reset_on_destroy = reset; }
	Reactor * getReactor(int32_t reactorid);
	size_t getReactorNum();

	// Parties
	void addParty(Party *party);

	// Timers
	void removeTimer(const string &name);
	void setInstanceTimer(int32_t time);
	void setPersistence(bool p) { m_persistent = p; }
	void timerEnd(const string &name, bool fromTimer = false);
	bool addTimer(const string &name, const TimerAction &timer);
	bool hasInstanceTimer() const { return m_time > 0; }
	bool getPersistence() const { return m_persistent; }
	int32_t checkTimer(const string &name);
	int32_t checkInstanceTimer();
	Timer::Container * getTimers() const { return m_timers.get(); }

	// Lua interaction
	void sendMessage(InstanceMessages message);
	void sendMessage(InstanceMessages message, int32_t);
	void sendMessage(InstanceMessages message, int32_t, int32_t);
	void sendMessage(InstanceMessages message, int32_t, int32_t, int32_t);
	void sendMessage(InstanceMessages message, const string &, int32_t);
private:
	boost::scoped_ptr<Timer::Container> m_timers; // Timer container for the instance
	unordered_map<string, TimerAction> m_timer_actions; // Timers indexed by name
	unordered_map<string, string> m_variables; // Instance variables
	unordered_map<int32_t, Player *> m_players;
	vector<string> m_banned; // For squads
	vector<string> m_players_order; // For squads
	vector<Reactor *> m_reactors;
	vector<Map *> m_maps;
	vector<Party *> m_parties;
	LuaInstance *m_luainstance; // Lua instance for interacting with scripts
	clock_t m_start; // Clock time when instance started
	string m_name; // Identification for the instance
	int32_t m_max_players; // Maximum players allowed for instance
	int32_t m_time; // Instance time
	int32_t m_timer_counter; // Used for uniqueness of timer IDs
	bool m_persistent; // Does instance repeat?
	bool m_show_timer; // Show timer
	bool m_reset_on_destroy; // Reset reactors when done
	bool m_marked_for_delete; // End of instance time
};
#endif