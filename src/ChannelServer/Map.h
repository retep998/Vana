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
#ifndef MAP_H
#define MAP_H

#include "Pos.h"
#include <ctime>
#include <hash_map>
#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>

using std::string;
using std::vector;
using stdext::hash_map;
using boost::scoped_ptr;

class Drop;
class LoopingId;
class Mob;
class PacketCreator;
class Player;
class Reactor;

struct FootholdInfo {
	Pos pos1;
	Pos pos2;
};
typedef vector <FootholdInfo> FootholdsInfo;

struct PortalInfo {
	char id;
	char from[20];
	int toid;
	char to[20];
	char type;
	Pos pos;
	char script[30];
	bool onlyOnce;
};
typedef vector <PortalInfo> PortalsInfo;

struct MapInfo {
	int id;
	int rm;
	int forcedReturn;
	char fieldType;
	int fieldLimit;
	double spawnrate;
	bool clock;
	int shipInterval;
};

struct NPCSpawnInfo {
	int id;
	short x;
	short cy;
	short fh;
	short rx0;
	short rx1; 
};
typedef vector <NPCSpawnInfo> NPCSpawnsInfo;

struct ReactorSpawnInfo {
	int id;
	Pos pos;
	int time;
};
typedef vector <ReactorSpawnInfo> ReactorSpawnsInfo;

struct ReactorRespawnInfo {
	ReactorRespawnInfo(int id, clock_t killed) : id(id), killed(killed) {}
	int id;
	clock_t killed;
};
typedef vector <ReactorRespawnInfo> ReactorRespawns;

struct MobSpawnInfo {
	int id;
	Pos pos;
	short fh;
	int last;
	int time;
};
typedef vector <MobSpawnInfo> MobSpawnsInfo;

struct MobRespawnInfo {
	MobRespawnInfo(int spawnid, clock_t killed) : spawnid(spawnid), killed(killed) {}
	int spawnid;
	clock_t killed;
};
typedef vector <MobRespawnInfo> MobRespawnsInfo;

class Map {
public:
	Map(MapInfo info);
	// Map Info
	MapInfo getInfo() {
		return info;
	}

	// Footholds
	void addFoothold(FootholdInfo foothold) {
		footholds.push_back(foothold);
	}
	Pos findFloor(Pos pos);

	// Portals
	void addPortal(PortalInfo portal) {
		portals.push_back(portal);
		if (strcmp(portal.from, "sp") == 0)
			spawnpoints += 1;
	}
	PortalInfo * getPortal(const char *from) {
		for (size_t i = 0; i < portals.size(); i++) {
			if (strcmp(portals[i].from, from) == 0)
				return &portals[i];
		}
		return 0;
	}
	PortalInfo * getSpawnPoint();

	// Players
	void addPlayer(Player *player);
	size_t getNumPlayers() {
		return this->players.size();
	}
	Player * getPlayer(unsigned int i) {
		return this->players[i];
	}
	void removePlayer(Player *player);

	// NPCs
	void addNPC(NPCSpawnInfo npc) {
		this->npcs.push_back(npc);
	}
	NPCSpawnInfo getNpc(int id) {
		return this->npcs[id];
	}

	// Mobs
	void addMobSpawn(MobSpawnInfo spawn);
	void checkMobSpawn(clock_t time);
	void addMob(Mob *mob);
	Mob * getMob(int id) {
		if (this->mobs.find(id) != mobs.end())
			return this->mobs[id];
		else
			return 0;
	}
	void removeMob(int id, int spawnid);
	void killMobs(Player *player);
	void killMobs(Player *player, int mobid);

	// Reactors
	void addReactorSpawn(ReactorSpawnInfo spawn);
	void addReactor(Reactor *reactor);
	void addReactorRespawn(ReactorRespawnInfo respawn);
	void checkReactorSpawn(clock_t time);
	Reactor * getReactor(int id) {
		if ((unsigned int)id < this->reactors.size())
			return this->reactors[id];
		else
			return 0;
	}
	size_t getNumReactors() {
		return this->reactors.size();
	}

	// Drops
	void addDrop(Drop *drop);
	Drop * getDrop(int id) {
		if (this->drops.find(id) != this->drops.end())
			return this->drops[id];
		else
			return 0;
	}
	void removeDrop(int id) {
		if (drops.find(id) != drops.end())
			drops.erase(id);
	}
	void clearDrops();
	void clearDrops(int time);
	// Timer stuff
	void setTimer();
	void runTimer();
	// Show all map objects
	void showObjects(Player *player);
	// Packet Stuff
	void sendPacket(PacketCreator &packet, Player *player = 0);
	void showMessage(string &message, char type);
private:
	MapInfo info;
	FootholdsInfo footholds;
	PortalsInfo portals;
	char spawnpoints;
	vector <Player *> players;
	NPCSpawnsInfo npcs;
	ReactorSpawnsInfo reactorspawns;
	vector <Reactor *> reactors;
	ReactorRespawns reactorrespawns;
	MobSpawnsInfo mobspawns;
	MobRespawnsInfo mobrespawns;
	hash_map<int, Mob *> mobs;
	hash_map<int, Drop *> drops;
	scoped_ptr<LoopingId> objectids;
	bool timer_started;

	void updateMobControl();
	void updateMobControl(Mob *mob, bool spawn = false);
};

#endif