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

#include "LoopingId.h"
#include "MapDataProvider.h"
#include "MapObjects.h"
#include "Mob.h"
#include "Pos.h"
#include "Types.h"
#include <boost/scoped_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <ctime>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;
using std::tr1::unordered_map;

class Drop;
class Instance;
class Mist;
class Mob;
class PacketCreator;
class Player;
class Reactor;
namespace Timer {
	class Container;
}

namespace SpawnTypes {
	enum {
		Mob = 0x01,
		Reactor = 0x02,
		All = Mob | Reactor
	};
}

class Map {
public:
	Map(MapInfoPtr info, int32_t id);

	void boatDock(bool isDocked);

	// Map info
	static uint32_t makeNpcId(uint32_t receivedId);
	static uint32_t makeReactorId(uint32_t receivedId);
	uint32_t makeNpcId();
	uint32_t makeReactorId();
	void setMusic(const string &musicname);
	void setMobSpawning(int32_t spawn);
	bool forceMapEquip() const { return (getInfo()->forceMapEquip); }
	bool hasClock() const { return (getInfo()->clock); }
	bool canVip() const { return !(getInfo()->limitations.vipRock); }
	bool canChalkboard() const { return !(getInfo()->limitations.chalkboard); }
	bool loseOnePercent() const { return (getInfo()->limitations.regularExpLoss || getInfo()->town); }
	uint8_t getContinent() const { return getInfo()->continent; }
	int32_t getForcedReturn() const { return getInfo()->forcedReturn; }
	int32_t getReturnMap() const { return getInfo()->rm; }
	const string & getMusic() const { return m_music; }
	const string & getDefaultMusic() const { return getInfo()->defaultMusic; }
	const Pos & getMapLeftTop() const { return getInfo()->lt; }
	const Pos & getMapRightBottom() const { return getInfo()->rb; }
	int32_t getObjectId() { return m_objectids.next(); }
	int32_t getId() const { return m_id; }

	// Data initialization
	void addFoothold(const FootholdInfo &foothold);
	void addSeat(int16_t id, const SeatInfo &seat);
	void addPortal(const PortalInfo &portal);
	int32_t addNpc(const NpcSpawnInfo &npc);
	void addMobSpawn(const MobSpawnInfo &spawn);
	void addReactorSpawn(const ReactorSpawnInfo &spawn);
	void addTimeMob(TimeMobPtr info);

	// Footholds
	Pos findFloor(const Pos &pos);
	Pos findRandomPos();
	int16_t getFhAtPosition(const Pos &pos);

	// Seats
	bool seatOccupied(int16_t id);
	void playerSeated(int16_t id, Player *player);

	// Portals
	PortalInfo * getPortal(const string &name);
	PortalInfo * getSpawnPoint(int8_t pid = -1);
	PortalInfo * getNearestSpawnPoint(const Pos &pos);

	// Players
	void addPlayer(Player *player);
	size_t getNumPlayers() const;
	Player * getPlayer(uint32_t i) const;
	string getPlayerNames();
	void removePlayer(Player *player);
	void dispelPlayers(int16_t prop, const Pos &origin, const Pos &lt, const Pos &rb);
	void statusPlayers(uint8_t status, uint8_t level, int16_t count, int16_t prop, const Pos &origin, const Pos &lt, const Pos &rb);
	void sendPlayersToTown(int32_t mobid, int16_t prop, int16_t count, const Pos &origin, const Pos &lt, const Pos &rb);
	void buffPlayers(int32_t buffid);

	// NPCs
	void removeNpc(uint32_t index);
	bool isValidNpcIndex(uint32_t id) const;
	NpcSpawnInfo getNpc(uint32_t id) const;

	// Mobs
	void addWebbedMob(Mob *mob);
	void removeWebbedMob(int32_t id);
	void removeMob(int32_t id, int32_t spawnid);
	void healMobs(int32_t hp, int32_t mp, const Pos &origin, const Pos &lt, const Pos &rb);
	void statusMobs(vector<StatusInfo> &statuses, const Pos &origin, const Pos &lt, const Pos &rb);
	void spawnZakum(const Pos &pos, int16_t fh = 0);
	void updateMobControl(Mob *mob, bool spawn = false, Player *display = nullptr);
	int32_t spawnShell(int32_t mobid, const Pos &pos, int16_t fh);
	int32_t spawnMob(int32_t mobid, const Pos &pos, int16_t fh = 0, Mob *owner = nullptr, int8_t summonEffect = 0);
	int32_t spawnMob(int32_t spawnid, const MobSpawnInfo &info);
	int32_t killMobs(Player *player, int32_t mobid = 0, bool playerkill = true, bool showpacket = true);
	int32_t countMobs(int32_t mobid = 0);
	Mob * getMob(int32_t id, bool isMapId = true);
	unordered_map<int32_t, Mob *> getMobs() const;


	// Reactors
	void addReactor(Reactor *reactor);
	void removeReactor(uint32_t id);
	void killReactors(bool showpacket = true);
	Reactor * getReactor(uint32_t id) const;
	size_t getNumReactors() const;

	// Drops
	void addDrop(Drop *drop);
	Drop * getDrop(int32_t id);
	void removeDrop(int32_t id);
	void clearDrops(bool showPacket = true);

	// Mists
	void addMist(Mist *mist);
	void removeMist(Mist *mist);
	void clearMists(bool showPacket = true);
	Mist * getMist(int32_t id);

	// Timer stuff
	void respawn(int8_t types = SpawnTypes::All);
	void checkSpawn(clock_t time);
	void checkShadowWeb();
	void checkMists();
	void clearDrops(clock_t time);
	void runTimer();
	void timeMob(bool firstLoad = true);
	void setMapTimer(int32_t t);
	Timer::Container * getTimers() const { return m_timers.get(); }

	// Show all map objects
	void showObjects(Player *player);

	// Packet stuff
	void sendPacket(PacketCreator &packet, Player *player = nullptr);
	void showMessage(const string &message, int8_t type);

	// Instance
	void setInstance(Instance *instance) { m_instance = instance; }
	Instance * getInstance() const { return m_instance; }

	// Weather cash item
	bool createWeather(Player *player, bool adminWeather, int32_t time, int32_t itemId, const string &message);
private:
	int32_t getMistId() { return m_mistids.next(); }
	static const uint32_t NpcStart = 100;
	static const uint32_t ReactorStart = 200;

	void updateMobControl(Player *player);
	int32_t getTimeMobId() const { return m_timemob; }
	MapInfo * getInfo() const { return m_info.get(); }
	TimeMob * getTimeMob() const { return m_time_mob.get(); }

	// Data
	bool m_ship;
	int32_t m_id;
	int32_t m_timer;
	int32_t m_timemob;
	int32_t m_spawnmobs;
	time_t m_timerstart;
	string m_music;
	Instance *m_instance;
	LoopingId m_objectids;
	LoopingId m_mistids;
	MapInfoPtr m_info;
	TimeMobPtr m_time_mob;
	boost::recursive_mutex m_drops_mutex;
	boost::shared_ptr<Timer::Container> m_timers;
	vector<FootholdInfo> m_footholds;
	vector<ReactorSpawnInfo> m_reactor_spawns;
	vector<NpcSpawnInfo> m_npc_spawns;
	vector<MobSpawnInfo> m_mob_spawns;
	map<int16_t, SeatInfo> m_seats;
	unordered_map<string, PortalInfo> m_portals;
	unordered_map<int8_t, PortalInfo> m_spawn_points;
	unordered_map<string, Pos> m_reactor_positions;

	// Shorter-lived objects
	vector<Player *> m_players;
	vector<Reactor *> m_reactors;
	vector<Respawnable> m_mob_respawns;
	vector<Respawnable> m_reactor_respawns;
	unordered_map<int32_t, Mob *> m_webbed;
	unordered_map<int32_t, Mob *> m_mobs;
	unordered_map<int32_t, Drop *> m_drops;
	unordered_map<int32_t, Mist *> m_poison_mists;
	unordered_map<int32_t, Mist *> m_mists;
};