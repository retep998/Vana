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

#include "LoopingId.hpp"
#include "MapDataProvider.hpp"
#include "MapObjects.hpp"
#include "Mob.hpp"
#include "Pos.hpp"
#include "Rect.hpp"
#include "TimerContainerHolder.hpp"
#include "Types.hpp"
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class Drop;
class Instance;
class Mist;
class Mob;
class PacketCreator;
class Player;
class Reactor;

namespace SpawnTypes {
	enum {
		Mob = 0x01,
		Reactor = 0x02,
		All = Mob | Reactor
	};
}

enum class FindFloorResult {
	Found,
	NotFound
};

class Map : public TimerContainerHolder {
	NONCOPYABLE(Map);
	NO_DEFAULT_CONSTRUCTOR(Map);
public:
	Map(ref_ptr_t<MapInfo> info, int32_t id);

	auto boatDock(bool isDocked) -> void;

	// Map info
	static auto makeNpcId(uint32_t receivedId) -> uint32_t;
	static auto makeReactorId(uint32_t receivedId) -> uint32_t;
	auto makeNpcId() -> uint32_t;
	auto makeReactorId() -> uint32_t;
	auto setMusic(const string_t &musicName) -> void;
	auto setMobSpawning(int32_t spawn) -> void;
	auto canVip() const -> bool { return !m_info->limitations.vipRock; }
	auto canChalkboard() const -> bool { return !m_info->limitations.chalkboard; }
	auto loseOnePercent() const -> bool { return m_info->limitations.regularExpLoss || m_info->town || m_mobs.size() == 0; }
	auto getContinent() const -> uint8_t { return m_info->continent; }
	auto getForcedReturn() const -> int32_t { return m_info->forcedReturn; }
	auto getReturnMap() const -> int32_t { return m_info->rm; }
	auto getObjectId() -> int32_t { return m_objectIds.next(); }
	auto getId() const -> int32_t { return m_id; }

	// Footholds
	auto findFloor(const Pos &pos, Pos &ret, int16_t yMod = 0) -> FindFloorResult;
	auto getFhAtPosition(const Pos &pos) -> int16_t;

	// Seats
	auto seatOccupied(int16_t id) -> bool;
	auto playerSeated(int16_t id, Player *player) -> void;

	// Portals
	auto getPortal(const string_t &name) -> PortalInfo *;
	auto getSpawnPoint(int8_t portalId = -1) -> PortalInfo *;
	auto getNearestSpawnPoint(const Pos &pos) -> PortalInfo *;
	auto getPortalNames() const -> vector_t<string_t>;

	// Players
	auto addPlayer(Player *player) -> void;
	auto getNumPlayers() const -> size_t;
	auto getPlayer(uint32_t i) const -> Player *;
	auto getPlayerNames() -> string_t;
	auto removePlayer(Player *player) -> void;
	auto checkPlayerEquip(Player *player) -> void;
	auto runFunctionPlayers(const Rect &dimensions, int16_t prop, function_t<void(Player *)> successFunc) -> void;
	auto runFunctionPlayers(const Rect &dimensions, int16_t prop, int16_t count, function_t<void(Player *)> successFunc) -> void;
	auto runFunctionPlayers(function_t<void(Player *)> successFunc) -> void;
	auto gmHideChange(Player *player) -> void;

	// NPCs
	auto addNpc(const NpcSpawnInfo &npc) -> int32_t;
	auto removeNpc(uint32_t index) -> void;
	auto isValidNpcIndex(uint32_t id) const -> bool;
	auto getNpc(uint32_t id) const -> NpcSpawnInfo;

	// Mobs
	auto addWebbedMob(int32_t mapMobId) -> void;
	auto removeWebbedMob(int32_t mapMobId) -> void;
	auto mobDeath(ref_ptr_t<Mob> mob, bool fromExplosion) -> void;
	auto healMobs(int32_t hp, int32_t mp, const Rect &dimensions) -> void;
	auto statusMobs(vector_t<StatusInfo> &statuses, const Rect &dimensions) -> void;
	auto spawnZakum(const Pos &pos, int16_t foothold = 0) -> void;
	auto convertShellToNormal(ref_ptr_t<Mob> mob) -> void;
	auto mobDeath(ref_ptr_t<Mob> mob) -> void;
	auto spawnMob(int32_t mobId, const Pos &pos, int16_t foothold = 0, ref_ptr_t<Mob> owner = nullptr, int8_t summonEffect = 0) -> ref_ptr_t<Mob>;
	auto spawnMob(int32_t spawnId, const MobSpawnInfo &info) -> ref_ptr_t<Mob>;
	auto killMobs(Player *player, int32_t mobId = 0) -> int32_t;
	auto countMobs(int32_t mobId = 0) -> int32_t;
	auto getMob(int32_t mapMobId) -> ref_ptr_t<Mob>;
	auto runFunctionMobs(function_t<void(ref_ptr_t<const Mob>)> func) -> void;
	auto switchController(ref_ptr_t<Mob> mob, Player *newController) -> void;
	auto mobSummonSkillUsed(ref_ptr_t<Mob> mob, MobSkillLevelInfo *skill) -> void;

	// Reactors
	auto addReactor(Reactor *reactor) -> void;
	auto removeReactor(uint32_t id) -> void;
	auto killReactors(bool showPacket = true) -> void;
	auto getReactor(uint32_t id) const -> Reactor *;
	auto getNumReactors() const -> size_t;

	// Drops
	auto addDrop(Drop *drop) -> void;
	auto getDrop(int32_t id) -> Drop *;
	auto removeDrop(int32_t id) -> void;
	auto clearDrops(bool showPacket = true) -> void;

	// Mists
	auto addMist(Mist *mist) -> void;

	// Timer stuff
	auto setMapTimer(const seconds_t &timer) -> void;
	auto respawn(int8_t types = SpawnTypes::All) -> void;

	// Show all map objects
	auto showObjects(Player *player) -> void;

	// Packet stuff
	auto sendPacket(PacketCreator &packet, Player *sender = nullptr) -> void;
	auto showMessage(const string_t &message, int8_t type) -> void;

	// Instance
	auto setInstance(Instance *instance) -> void { m_instance = instance; }
	auto endInstance(bool reset) -> void;
	auto getInstance() const -> Instance * { return m_instance; }

	// Weather cash item
	auto createWeather(Player *player, bool adminWeather, int32_t time, int32_t itemId, const string_t &message) -> bool;
private:
	auto getMistId() -> int32_t { return m_mistIds.next(); }
	static const uint32_t NpcStart = 100;
	static const uint32_t ReactorStart = 200;

	friend class MapDataProvider;
	auto addFoothold(const FootholdInfo &foothold) -> void;
	auto addSeat(const SeatInfo &seat) -> void;
	auto addPortal(const PortalInfo &portal) -> void;
	auto addMobSpawn(const MobSpawnInfo &spawn) -> void;
	auto addReactorSpawn(const ReactorSpawnInfo &spawn) -> void;
	auto addTimeMob(ref_ptr_t<TimeMob> info) -> void;
	auto checkSpawn(time_point_t time) -> void;
	auto checkShadowWeb() -> void;
	auto checkMists() -> void;
	auto clearDrops(time_point_t time) -> void;
	auto timeMob(bool firstLoad = true) -> void;
	auto spawnShell(int32_t mobId, const Pos &pos, int16_t foothold) -> ref_ptr_t<Mob>;
	auto updateMobControl(Player *player) -> void;
	auto updateMobControl(ref_ptr_t<Mob> mob, bool spawn = false, Player *display = nullptr) -> void;
	auto mapTick(const time_point_t &now) -> void;
	auto getTimeMobId() const -> int32_t { return m_timeMob; }
	auto getTimeMob() const -> TimeMob * { return m_timeMobInfo.get(); }
	auto getMist(int32_t id) -> Mist *;
	auto findController(ref_ptr_t<Mob> mob) -> Player *;
	auto clearMists(bool showPacket = true) -> void;
	auto removeMist(Mist *mist) -> void;
	auto findRandomFloorPos() -> Pos;
	auto findRandomFloorPos(const Rect &area) -> Pos;
	auto buffPlayers(int32_t buffId) -> void;

	// Longer-lived data
	bool m_ship = false;
	int32_t m_id = 0;
	int32_t m_timeMob = 0;
	int32_t m_spawnMobs = -1;
	int32_t m_emptyMapTicks = 0;
	int32_t m_minSpawnCount = 0;
	int32_t m_maxSpawnCount = 0;
	Instance *m_instance = nullptr;
	seconds_t m_timer = seconds_t(0);
	time_point_t m_timerStart = seconds_t(0);
	time_point_t m_lastSpawn = seconds_t(0);
	string_t m_music;
	LoopingId<int32_t> m_objectIds;
	LoopingId<int32_t> m_mistIds;
	recursive_mutex_t m_dropsMutex;
	ref_ptr_t<MapInfo> m_info;
	ref_ptr_t<TimeMob> m_timeMobInfo;
	vector_t<FootholdInfo> m_footholds;
	vector_t<ReactorSpawnInfo> m_reactorSpawns;
	vector_t<NpcSpawnInfo> m_npcSpawns;
	vector_t<MobSpawnInfo> m_mobSpawns;
	ord_map_t<int16_t, SeatInfo> m_seats;
	hash_map_t<string_t, PortalInfo> m_portals;
	hash_map_t<int8_t, PortalInfo> m_spawnPoints;
	hash_map_t<string_t, Pos> m_reactorPositions;

	// Shorter-lived objects
	vector_t<Player *> m_players;
	vector_t<Reactor *> m_reactors;
	vector_t<Respawnable> m_mobRespawns;
	vector_t<Respawnable> m_reactorRespawns;
	hash_map_t<int32_t, view_ptr_t<Mob>> m_webbed;
	hash_map_t<int32_t, ref_ptr_t<Mob>> m_mobs;
	hash_map_t<int32_t, Player *> m_playersWithoutProtectItem;
	hash_map_t<int32_t, Drop *> m_drops;
	hash_map_t<int32_t, Mist *> m_poisonMists;
	hash_map_t<int32_t, Mist *> m_mists;
};