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
#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

class Map;
class Player;
class MapTimer;
class ReadPacket;
class Mob;
class Reactor;
class Drop;
class LoopingId;

struct PortalInfo {
	int id;
	char from[20];
	int toid;
	char to[20];
	int type;
	short x;
	short y;
	char script[30];
};
typedef vector<PortalInfo> PortalsInfo;

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
	void addPlayer(Player *player);
	void moveMap(Player *player, ReadPacket *packet);
	void moveMapS(Player *player, ReadPacket *packet);
	void changeMap(Player *player, int mapid, int pos);
	void showClock(Player *player);
	void mapTimer(int mapid);
	void newMap(Player *player, int mapid);
	void changeMusic(int mapid, std::string musicname);
};

class Map {
public:
	Map();
	// Map Info
	void setInfo(MapInfo info) {
		this->info = info;
	}
	MapInfo getInfo() {
		return this->info;
	}
	// Players
	void addPlayer(Player *player) {
		this->players.push_back(player);
	}
	vector <Player *> getPlayers() {
		return this->players;
	}
	void removePlayer(Player *player);
	// NPCs
	void addNPC(NPCInfo npc) {
		this->npcs.push_back(npc);
	}
	NPCInfo getNpc(int id) {
		return this->npcs[id];
	}
	vector <NPCInfo> getNpcs() {
		return this->npcs;
	}
	// Mobs
	void addMob(Mob *mob);
	Mob * getMob(int id) {
		if (this->mobs.find(id) != mobs.end())
			return this->mobs[id];
		else
			return 0;
	}
	hash_map <int, Mob *> getMobs() {
		return this->mobs;
	}
	void removeMob(Mob *mob);
	// Reactors
	void addReactor(Reactor *reactor);
	Reactor * getReactor(int id) {
		if ((unsigned int)id < this->reactors.size())
			return this->reactors[id];
		else
			return 0;
	}
	vector <Reactor *> getReactors() {
		return this->reactors;
	}
	// Drops
	void addDrop(Drop *drop);
	Drop * getDrop(int id) {
		if (this->drops.find(id) != this->drops.end())
			return this->drops[id];
		else
			return 0;
	}
	hash_map <int, Drop *> getDrops() {
		return this->drops;
	}
	void removeDrop(Drop *drop);
	// Portals
	PortalsInfo portals;
private:
	MapInfo info;
	vector <Player *> players;
	vector <NPCInfo> npcs;
	vector <Reactor *> reactors;
	hash_map <int, Mob *> mobs;
	LoopingId *mobids;
	hash_map <int, Drop *> drops;
	LoopingId *dropids;
};

#endif
