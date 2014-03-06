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
#pragma once

#include "TimerContainerHolder.hpp"
#include "Types.hpp"
#include "Variables.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class LuaInstance;
class Map;
class Party;
class Player;
class Reactor;
struct PortalInfo;

namespace Timer {
	struct Id;
}

struct TimerAction {
	int32_t counterId = 0;
	int32_t time = 0; // How long the timer lasts, negative integers indicate second of hour (-1 to -3600)
	int32_t persistent = 0; // How often does it repeat?
};

class Instance : public TimerContainerHolder {
	NONCOPYABLE(Instance);
	NO_DEFAULT_CONSTRUCTOR(Instance);
public:
	Instance(const string_t &name, map_id_t map, player_id_t playerId, const duration_t &time, const duration_t &persistent, bool showTimer);
	~Instance();

	auto getName() const -> string_t { return m_name; }
	auto getStart() const -> const time_point_t & { return m_start; }
	auto getCounterId() -> int32_t;
	auto getMarkedForDelete() const -> bool { return m_markedForDeletion; }
	auto markForDelete() -> void;
	auto instanceEnd(bool fromTimer = false) -> void;
	auto getVariables() const -> Variables * { return m_variables.get(); }

	// Players
	auto getAllPlayerIds() -> vector_t<player_id_t>;
	auto getPlayerByIndex(uint32_t index) const -> const string_t;
	auto getBannedPlayerByIndex(uint32_t index) const -> const string_t;
	auto setMaxPlayers(int32_t maxPlayers) -> void { m_maxPlayers = maxPlayers; }
	auto addPlayer(Player *player) -> void;
	auto removePlayer(Player *player) -> void;
	auto removePlayer(player_id_t id) -> void;
	auto removeAllPlayers() -> void;
	auto setBanned(const string_t &name, bool isBanned) -> void;
	auto addPlayerSignUp(Player *player) -> void;
	auto removePlayerSignUp(const string_t &name) -> void;
	auto moveAllPlayers(map_id_t mapId, bool respectInstances, PortalInfo *portal = nullptr) -> void;
	auto isPlayerSignedUp(const string_t &name) -> bool;
	auto isBanned(const string_t &name) -> bool;
	auto instanceHasPlayers() const -> bool;
	auto getMaxPlayers() const -> int32_t { return m_maxPlayers; }
	auto getPlayerNum() const -> size_t { return m_players.size(); }
	auto getPlayerSignupNum() const -> size_t { return m_playersOrder.size(); } // Number of players for the instance (squads, etc.)
	auto getBannedPlayerNum() const -> size_t { return m_banned.size(); }

	// Maps
	auto addMap(Map *map) -> void;
	auto addMap(map_id_t mapId) -> void;
	auto isInstanceMap(map_id_t mapId) const -> bool;
	auto setResetAtEnd(bool reset) -> void { m_resetOnDestroy = reset; }
	auto respawnMobs(map_id_t mapId) -> void;
	auto respawnReactors(map_id_t mapId) -> void;

	// Parties
	auto addParty(Party *party) -> void;

	// Instance time
	auto hasInstanceTimer() const -> bool { return m_time.count() > 0; }
	auto setInstanceTimer(const duration_t &time, bool firstRun = false) -> void;
	auto setPersistence(const duration_t &persistence) -> void { m_persistent = persistence; }
	auto getPersistence() const -> duration_t { return m_persistent; }
	auto checkInstanceTimer() -> seconds_t;
	auto showTimer() const -> bool { return m_showTimer; }
	auto showTimer(bool show, bool doIt = false) -> void;

	// Timers
	auto removeAllTimers() -> void;
	auto removeTimer(const string_t &name) -> void;
	auto timerComplete(const string_t &name, bool fromTimer = false) -> void;
	auto addTimer(const string_t &name, const TimerAction &timer) -> bool;
	auto isTimerPersistent(const string_t &name) -> bool;
	auto getTimerSecondsRemaining(const string_t &name) -> seconds_t;

	// Lua interaction
	auto beginInstance() -> Result;
	auto playerDeath(player_id_t playerId) -> Result;
	auto instanceTimerEnd(bool fromTimer) -> Result;
	auto partyDisband(party_id_t partyId) -> Result;
	auto timerEnd(const string_t &name, bool fromTimer) -> Result;
	auto playerDisconnect(player_id_t playerId, bool isPartyLeader) -> Result;
	auto removePartyMember(party_id_t partyId, player_id_t playerId) -> Result;
	auto mobDeath(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId) -> Result;
	auto mobSpawn(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId) -> Result;
	auto playerChangeMap(player_id_t playerId, map_id_t newMapId, map_id_t oldMapId, bool isPartyLeader) -> Result;
	auto friendlyMobHit(mob_id_t mobId, map_object_t mapMobId, map_id_t mapId, int32_t mobHp, int32_t mobMaxHp) -> Result;
private:
	auto getLuaInstance() -> LuaInstance * { return m_luaInstance.get(); }
	auto getTimerId() const -> Timer::Id;

	bool m_showTimer = false;
	bool m_resetOnDestroy = true;
	bool m_markedForDeletion = false;
	int32_t m_maxPlayers = 0;
	int32_t m_timerCounter = 0;
	time_point_t m_start;
	seconds_t m_time;
	duration_t m_persistent;
	string_t m_name;
	owned_ptr_t<Variables> m_variables;
	owned_ptr_t<LuaInstance> m_luaInstance; // Lua instance for interacting with scripts
	vector_t<string_t> m_banned; // For squads
	vector_t<string_t> m_playersOrder; // For squads
	vector_t<Map *> m_maps;
	vector_t<Party *> m_parties;
	hash_map_t<string_t, TimerAction> m_timerActions; // Timers indexed by name
	hash_map_t<player_id_t, Player *> m_players;
};