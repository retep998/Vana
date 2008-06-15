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
#include "Maps.h"
#include "MapPacket.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "PlayersPacket.h"
#include "NPCs.h"
#include "Reactors.h"
#include "Mobs.h"
#include "Drops.h"
#include "Timer.h"
#include "LuaPortal.h"
#include "BufferUtilities.h"
#include "ReadPacket.h"
#include "LoopingId.h"
#include <iostream>
#include <sstream>
#include <string>

hash_map <int, Map *> Maps::maps;

class MapTimer: public Timer::TimerHandler {
public:
	static MapTimer * Instance() {
		if (singleton == 0)
			singleton = new MapTimer;
		return singleton;
	}
	void setMapTimer(int mapid) {
		if (ctimer.find(mapid) != ctimer.end())
			if (ctimer[mapid])
			return;
		Reactors::loadReactors(mapid);
		Maps::mapTimer(mapid);
		timers[Timer::Instance()->setTimer(10000, this, true)] = mapid;
		ctimer[mapid] = 1;
	}
private:
	static MapTimer *singleton;
	MapTimer() {};
	MapTimer(const MapTimer&);
	MapTimer& operator = (const MapTimer&);

	static hash_map <int, int> timers;
	static hash_map <int, int> ctimer;
	void handle (Timer *timer, int id) {
		Maps::mapTimer(timers[id]);
	}
	void remove (int id) {
		timers.erase(id);
	}
};
hash_map <int,int> MapTimer::timers;
hash_map <int,int> MapTimer::ctimer;
MapTimer * MapTimer::singleton = 0;

// Map class
Map::Map () : mobids(new LoopingId(100)), dropids(new LoopingId(100)) { }

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i]->getPlayerid() == player->getPlayerid()) {
			this->players.erase(this->players.begin() + i);
			break;
		}
	}
	MapPacket::removePlayer(player, this->getPlayers());
	Mobs::updateSpawn(player->getMap());
}

void Map::addReactor(Reactor *reactor) {
	this->reactors.push_back(reactor);
	reactor->setID(this->reactors.size() - 1 + 200);
}

void Map::addMob(Mob *mob) {
	int id = this->mobids->next();
	this->mobs[id] = mob;
	mob->setID(id);
}

void Map::removeMob(Mob *mob) {
	this->mobs.erase(mob->getID());
}

void Map::addDrop(Drop *drop) {
	int id = dropids->next();
	this->drops[id] = drop;
	drop->setID(id);
}

void Map::removeDrop(Drop *drop) {
	this->drops.erase(drop->getID());
}

// Maps namespace
void Maps::addMap(int id) {
	maps[id] = new Map();
}

void Maps::addPlayer(Player *player) {
	if (player->getMap() == 1 || player->getMap() == 2)
		MapPacket::makeApple(player);
	maps[player->getMap()]->addPlayer(player);
	MapPacket::showPlayers(player, maps[player->getMap()]->getPlayers());
	if (player->skills->getActiveSkillLevel(5101004) == 0)
		MapPacket::showPlayer(player, maps[player->getMap()]->getPlayers());
}

void Maps::moveMap(Player *player, ReadPacket *packet) {
	packet->skipBytes(1);
	if (packet->getInt() == 0) { // Dead
		int tomap;
		if (maps.find(player->getMap()) == maps.end())
			tomap = player->getMap();
		else
			tomap = maps[player->getMap()]->getInfo().rm;
		player->setHP(50, 0);
		changeMap(player, tomap, 0);
		return;
	}
	string portalname = packet->getString();
	PortalInfo portal;
	for (unsigned int i = 0; i < maps[player->getMap()]->portals.size(); i++)
		if (strcmp(maps[player->getMap()]->portals[i].from, portalname.c_str()) == 0) {
			portal = maps[player->getMap()]->portals[i];
			break;
		}
	int tonum = 0;
	if (maps.find(portal.toid) != maps.end()) {
		for (unsigned int i = 0; i < maps[portal.toid]->portals.size(); i++) {
			if (strcmp(portal.to, maps[portal.toid]->portals[i].from) ==0) {
				tonum = maps[portal.toid]->portals[i].id;
				break;
			}
		}
	}
	changeMap(player, portal.toid, tonum);
}

void Maps::moveMapS(Player *player, ReadPacket *packet) { // Move to map special
	packet->skipBytes(1);
	string portalname = packet->getString();

	PortalInfo portal;
	for (unsigned int i = 0; i < maps[player->getMap()]->portals.size(); i++) {
		if (strcmp(maps[player->getMap()]->portals[i].from, portalname.c_str()) == 0) {
			portal = maps[player->getMap()]->portals[i];
			break;
		}
	}

	std::ostringstream filenameStream;
	filenameStream << "scripts/portals/" << portal.script << ".lua";
	LuaPortal(filenameStream.str(), player->getPlayerid(), &portal);
	int tonum = 0;
	if (maps.find(portal.toid) != maps.end()) {
		for (unsigned int i = 0; i < maps[portal.toid]->portals.size(); i++) {
			if (strcmp(portal.to, maps[portal.toid]->portals[i].from) ==0) {
				tonum = maps[portal.toid]->portals[i].id;
				break;
			}
		}
	}
	changeMap(player, portal.toid, tonum);
}

void Maps::changeMap(Player *player, int mapid, int pos) {
	if (mapid == 999999999) {
		PlayerPacket::showMessage(player, "This portal is currently unavailable.", 5);
		MapPacket::portalBlocked(player);
		return;
	}
	else if (mapid < 0) {
		MapPacket::portalBlocked(player);
		return;
	}
	maps[player->getMap()]->removePlayer(player);
	player->setMap(mapid);
	player->setMappos(pos);
	player->setType(0);
	Pos cpos;
	if ((unsigned int)pos < maps[mapid]->portals.size()) {
		cpos.x = maps[mapid]->portals[pos].x;
		cpos.y = maps[mapid]->portals[pos].y;
	}
	else if (maps[mapid]->portals.size() > 0) {
		cpos.x = maps[mapid]->portals[0].x;
		cpos.y = maps[mapid]->portals[0].y;
	}
	else{
		cpos.x = 0;
		cpos.y = 0;
	}
	player->setPos(cpos);
	MapPacket::changeMap(player);
	newMap(player, mapid);
}

void Maps::showClock(Player *player) {
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	MapPacket::showClock(player, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void Maps::mapTimer(int mapid) {
	Mobs::checkSpawn(mapid);
	Drops::checkDrops(mapid);
}

void Maps::newMap(Player *player, int mapid) {
	Players::addPlayer(player);
	NPCs::showNPCs(player);
	Reactors::showReactors(player);
	addPlayer(player);
	Mobs::showMobs(player);
	Drops::showDrops(player);
	if (maps[mapid]->getInfo().clock)
		showClock(player);
	MapTimer::Instance()->setMapTimer(player->getMap());
}
// Change Music
void Maps::changeMusic(int mapid, const string &musicname) {
	if (Maps::maps.find(mapid) != Maps::maps.end()) {
		MapPacket::changeMusic(maps[mapid]->getPlayers(), musicname);
	}
}
