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
#ifndef MAPS_H
#define MAPS_H

#include "NPCs.h"
#include "Pos.h"
#include <hash_map>
#include <vector>
#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace stdext;
using boost::scoped_ptr;

class Map;
class Player;
class MapTimer;
class ReadPacket;
class Mob;
class Reactor;
class Drop;
class LoopingId;
struct Equip;
struct Item;

struct FootholdInfo {
	Pos pos1;
	Pos pos2;
};
typedef vector <FootholdInfo> FootholdsInfo;

struct PortalInfo {
	int id;
	char from[20];
	int toid;
	char to[20];
	int type;
	Pos pos;
	char script[30];
};
typedef vector <PortalInfo> PortalsInfo;

struct MapInfo {
	int rm;
	int forcedReturn;
	double spawnrate;
	bool clock;
	int shipInterval;
};

namespace Maps {
	extern MapTimer *timer;
	extern hash_map <int, Map *> maps;
	void startTimer();
	void addMap(int id);
	void moveMap(Player *player, ReadPacket *packet);
	void moveMapS(Player *player, ReadPacket *packet);
	void changeMap(Player *player, int mapid, int portalid);
	void showClock(Player *player);
	void mapTimer(int mapid);
	void newMap(Player *player, int mapid);
	void changeMusic(int mapid, const string &musicname);
};

class Map {
public:
	Map(int mapid);
	// Map Info
	void setInfo(MapInfo info) {
		this->info = info;
	}
	MapInfo getInfo() {
		return this->info;
	}

	// Footholds
	void addFoothold(FootholdInfo foothold) {
		footholds.push_back(foothold);
	}
	Pos findFloor(Pos pos);

	// Portals
	void addPortal(PortalInfo portal) {
		portals.push_back(portal);
	}
	PortalInfo * getPortal(const char *from) {
		for (size_t i = 0; i < portals.size(); i++) {
			if (strcmp(portals[i].from, from) == 0)
				return &portals[i];
		}
		return 0;
	}
	PortalInfo * getPortalByID(int id) {
		return &portals[id];
	}
	size_t getNumPortals() {
		return portals.size();
	}

	// Players
	void addPlayer(Player *player);
	vector <Player *> getPlayers() {
		return this->players;
	}
	size_t getNumPlayers() {
		return this->players.size();
	}
	Player * getPlayer(unsigned int i) {
		return this->players[i];
	}
	void removePlayer(Player *player);

	// NPCs
	void addNPC(NPCInfo npc) {
		this->npcs.push_back(npc);
	}
	NPCInfo getNpc(int id) {
		return this->npcs[id];
	}

	// Mobs
	void addMob(int mobid, Pos pos, int spawnid = -1, short fh = 0);
	Mob * getMob(int id) {
		if (this->mobs.find(id) != mobs.end())
			return this->mobs[id];
		else
			return 0;
	}
	void removeMob(int id);
	void killMobs(Player *player);
	void killMobs(Player *player, int mobid);

	// Reactors
	void addReactor(Reactor *reactor);
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
	// Show all map objects
	void showObjects(Player *player);
private:
	int mapid;
	MapInfo info;
	FootholdsInfo footholds;
	PortalsInfo portals;
	vector <Player *> players;
	vector <NPCInfo> npcs;
	vector <Reactor *> reactors;
	hash_map<int, Mob *> mobs;
	scoped_ptr<LoopingId> mobids;
	hash_map<int, Drop *> drops;
	scoped_ptr<LoopingId> dropids;

	void updateMobControl();
	void updateMobControl(Mob *mob);
};

#endif
