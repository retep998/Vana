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
#include "NPCPacket.h"
#include "Reactors.h"
#include "ReactorPacket.h"
#include "Mobs.h"
#include "MobsPacket.h"
#include "Drops.h"
#include "Timer.h"
#include "LuaPortal.h"
#include "ReadPacket.h"
#include "LoopingId.h"
#include "PacketCreator.h"
#include "WorldServerConnectPlayerPacket.h"
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

/** Map class **/
Map::Map (int mapid) : mapid(mapid), mobids(new LoopingId(100)), dropids(new LoopingId(100)) { }

// Players
void Map::addPlayer(Player *player) {
	this->players.push_back(player);
	if (info.fieldType == 82)
		MapPacket::makeApple(player);
	if (player->skills->getActiveSkillLevel(9101004) == 0)
		MapPacket::showPlayer(player);
}

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i]->getPlayerid() == player->getPlayerid()) {
			this->players.erase(this->players.begin() + i);
			break;
		}
	}
	MapPacket::removePlayer(player);
	updateMobControl();
}

// Reactors
void Map::addReactorSpawn(ReactorSpawnInfo spawn) {
	reactorspawns.push_back(spawn);
	Reactor *reactor = new Reactor(mapid, spawn.id, spawn.pos);
	ReactorPacket::spawnReactor(reactor);
}

void Map::addReactor(Reactor *reactor) {
	this->reactors.push_back(reactor);
	reactor->setID(this->reactors.size() - 1 + 200);
}

// Footholds
Pos Map::findFloor(Pos pos) {
	short x = pos.x;
	short y = pos.y - 100;
	bool first = true;
	short maxy = pos.y;
	unsigned int fh = 0;
	for (size_t i = 0; i < footholds.size(); i++) {
		if ((x > footholds[i].pos1.x && x < footholds[i].pos2.x) || (x > footholds[i].pos2.x && x < footholds[i].pos1.x)) {
			if (first) {
				maxy = (short) ( (float) ( footholds[i].pos1.y - footholds[i].pos2.y ) / ( footholds[i].pos1.x - footholds[i].pos2.x ) * x - footholds[i].pos1.x * (float) ( footholds[i].pos1.y - footholds[i].pos2.y ) / ( footholds[i].pos1.x - footholds[i].pos2.x ) + footholds[i].pos1.y );
				if (maxy >= y) {
					fh = i;
					first = false;
				}
			}
			else {
				short cmax = (short) ( (float) ( footholds[i].pos1.y - footholds[i].pos2.y ) / ( footholds[i].pos1.x - footholds[i].pos2.x ) * x - footholds[i].pos1.x * (float) ( footholds[i].pos1.y - footholds[i].pos2.y ) / ( footholds[i].pos1.x - footholds[i].pos2.x ) + footholds[i].pos1.y );
				if (cmax < maxy && cmax >= y) {
					fh = i;
					maxy = cmax;
				}
			}
		}
	}
	return Pos(x, maxy);
}

// Mobs
void Map::addMobSpawn(MobSpawnInfo spawn) {
	mobspawns.push_back(spawn);
	// Queue up spawn point for initial spawning
	respawns.push(mobspawns.size() - 1);
}

void Map::queueMobSpawn(int spawnid) {
	respawns.push(spawnid);
}

void Map::checkSpawn() {
	// (Re-)spawn Mobs
	while (!respawns.empty()) {
		int i = respawns.front();
		respawns.pop();
		new Mob(mapid, mobspawns[i].id, mobspawns[i].pos, i, mobspawns[i].fh);
	}
}

void Map::addMob(Mob *mob) {
	int id = this->mobids->next();
	mob->setID(id);
	this->mobs[id] = mob;
	MobsPacket::spawnMob(mob);
	updateMobControl(mob);
}

void Map::updateMobControl() {
	for (hash_map <int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			updateMobControl(iter->second);
	}
}

void Map::updateMobControl(Mob *mob) {
	if (players.size() > 0 && mob->getControl() == 0) {
		int maxpos = mob->getPos() - players[0]->getPos();
		int player = 0;
		for (size_t j = 0; j < players.size(); j++) {
			int curpos = mob->getPos() - players[j]->getPos();
			if (curpos < maxpos) {
				maxpos = curpos;
				player = j;
			}
		}
		mob->setControl(players[player]);
	}
	else if (players.size() == 0) {
		mob->setControl(0);
	}
}

void Map::removeMob(int id) {
	if (mobs.find(id) != mobs.end()) {
		delete mobs[id];
		this->mobs.erase(id);
	}
}

void Map::killMobs(Player *player) {
	hash_map <int, Mob *> mobs = this->mobs;
	for (hash_map <int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) { // While loops cause problems
		if (iter->second != 0)
			Mobs::dieMob(player, iter->second);
	}
}

void Map::killMobs(Player *player, int mobid) {
	hash_map <int, Mob *> mobs = this->mobs;
	for (hash_map <int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			if (iter->second->getMobID() == mobid)
				Mobs::dieMob(player, iter->second);
	}
}

// Drops
void Map::addDrop(Drop *drop) {
	int id = dropids->next();
	drop->setID(id);
	drop->setPos(findFloor(drop->getPos()));
	this->drops[id] = drop;
}

void Map::clearDrops() { // Clear all drops
	hash_map <int, Drop *> drops = this->drops;
	for (hash_map <int, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0) // Check just in case drop is removed by timer
			iter->second->removeDrop();
	}
}

void Map::clearDrops(int time) { // Clear drops based on how long they have been in the map
	time -= 60000;
	hash_map <int, Drop *> drops = this->drops;
	for (hash_map <int, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0)
			if (iter->second->getDropped() < time)
				iter->second->removeDrop();
	}
}

void Map::showObjects(Player *player) { // Show all Map Objects
	// Players
	MapPacket::showPlayers(player, this->players);
	// NPCs
	for (size_t i = 0; i < npcs.size(); i++) {
		NPCPacket::showNPC(player, npcs[i], i);
	}
	// Reactors
	for (size_t i = 0; i < reactors.size(); i++) {
		ReactorPacket::showReactor(player, reactors[i]);
	}
	// Mobs
	updateMobControl();
	for (hash_map <int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			MobsPacket::showMob(player, iter->second);
	}
	// Drops
	for (hash_map <int, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0)
			iter->second->showDrop(player);
	}
	if (this->info.clock)
		Maps::showClock(player);
}

void Map::sendPacket(Packet &packet, Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i] != player) {
			packet.send(this->players[i]);
		}
	}
}

/** Maps namespace **/
void Maps::addMap(int id) {
	maps[id] = new Map(id);
}

void Maps::moveMap(Player *player, ReadPacket *packet) {
	packet->skipBytes(1);
	if (packet->getInt() == 0) { // Dead
		int tomap;
		if (maps.find(player->getMap()) == maps.end())
			tomap = player->getMap();
		else
			tomap = maps[player->getMap()]->getInfo().rm;
		player->setHP(50, false);
		changeMap(player, tomap, 0);
		return;
	}
	string portalname = packet->getString();

	PortalInfo *portal = maps[player->getMap()]->getPortal(portalname.c_str());
	if (portal == 0) // Exit the function if portal is not found
		return;

	int tonum = 0;
	PortalInfo * nextportal = maps[portal->toid]->getPortal(portal->to);
	if (nextportal != 0)
		tonum = nextportal->id;

	changeMap(player, portal->toid, tonum);
}

void Maps::moveMapS(Player *player, ReadPacket *packet) { // Move to map special
	packet->skipBytes(1);
	string portalname = packet->getString();

	PortalInfo *portal = maps[player->getMap()]->getPortal(portalname.c_str());
	if (portal == 0) // Exit the function if portal is not found
		return;

	std::ostringstream filenameStream;
	filenameStream << "scripts/portals/" << portal->script << ".lua";
	LuaPortal(filenameStream.str(), player->getPlayerid(), portal);

	int tonum = 0;
	if (portal->toid >= 0 && portal->toid != 999999999) { // Only check for new portal ID if a portal script returns a valid map
		PortalInfo *nextportal = maps[portal->toid]->getPortal(portal->to);
		if (nextportal != 0) {
			tonum = nextportal->id;
		}
	}

	changeMap(player, portal->toid, tonum);
}

void Maps::changeMap(Player *player, int mapid, int portalid) {
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
	player->setMappos(portalid);
	player->setType(0);

	Pos pos;
	if ((unsigned int)portalid < maps[mapid]->getNumPortals()) {
		pos = maps[mapid]->getPortalByID(portalid)->pos;
	}
	else if (maps[mapid]->getNumPortals() > 0) {
		pos = maps[mapid]->getPortalByID(0)->pos;
	}
	player->setPos(pos);
	WorldServerConnectPlayerPacket::updateMap(ChannelServer::Instance()->getWorldPlayer(), player->getPlayerid(), mapid);
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
	maps[mapid]->checkSpawn();
	maps[mapid]->clearDrops(clock());
}

void Maps::newMap(Player *player, int mapid) {
	Players::addPlayer(player);
	maps[mapid]->addPlayer(player);
	maps[mapid]->showObjects(player);
	MapTimer::Instance()->setMapTimer(player->getMap());
}
// Change Music
void Maps::changeMusic(int mapid, const string &musicname) {
	if (Maps::maps.find(mapid) != Maps::maps.end()) {
		MapPacket::changeMusic(mapid, musicname);
	}
}
