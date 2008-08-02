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
#include "Players.h"
#include "ReadPacket.h"
#include "LuaPortal.h"
#include "Timer.h"
#include "WorldServerConnectPlayerPacket.h"
#include <sstream>

hash_map<int, Map *> Maps::maps;

class MapTimer : public Timer::TimerHandler {
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

	hash_map <int, int> timers;
	hash_map <int, int> ctimer;
	void handle (Timer *timer, int id) {
		Maps::mapTimer(timers[id]);
	}
	void remove (int id) {
		timers.erase(id);
	}
};
MapTimer * MapTimer::singleton = 0;

void Maps::addMap(MapInfo info) {
	maps[info.id] = new Map(info);
}

void Maps::usePortal(Player *player, ReadPacket *packet) {
	packet->skipBytes(1);
	if (packet->getInt() == 0) { // Dead
		int tomap;
		if (maps.find(player->getMap()) == maps.end())
			tomap = player->getMap();
		else
			tomap = maps[player->getMap()]->getInfo().rm;
		player->acceptDeath(tomap);
		return;
	}
	string portalname = packet->getString();

	PortalInfo *portal = maps[player->getMap()]->getPortal(portalname.c_str());
	if (portal == 0) // Exit the function if portal is not found
		return;

	PortalInfo *nextportal = maps[portal->toid]->getPortal(portal->to);

	changeMap(player, portal->toid, nextportal);
}

void Maps::useScriptedPortal(Player *player, ReadPacket *packet) {
	packet->skipBytes(1);
	string portalname = packet->getString();

	PortalInfo *portal = maps[player->getMap()]->getPortal(portalname.c_str());
	if (portal == 0) // Exit the function if portal is not found
		return;

	std::ostringstream filenameStream;
	filenameStream << "scripts/portals/" << portal->script << ".lua";
	LuaPortal(filenameStream.str(), player->getPlayerid(), portal);

	PortalInfo *nextportal = 0;
	if (portal->toid >= 0 && portal->toid != 999999999) { // Only check for new portal ID if a portal script returns a valid map
		nextportal = maps[portal->toid]->getPortal(portal->to);
	}
	else if (portal->toid == 999999999) {
		std::ostringstream messageStream;
		messageStream << "This portal '" << portal->script << "' is currently unavailable.";
		PlayerPacket::showMessage(player, messageStream.str(), 5);
	}

	changeMap(player, portal->toid, nextportal);
}

void Maps::changeMap(Player *player, int mapid, PortalInfo *portal) {
	if (maps.find(mapid) == maps.end()) {
		MapPacket::portalBlocked(player);
		return;
	}
	if (portal == 0)
		portal = maps[mapid]->getSpawnPoint();

	maps[player->getMap()]->removePlayer(player);
	player->setMap(mapid);
	player->setMappos(portal->id);
	player->setPos(portal->pos);
	player->setType(0);
	WorldServerConnectPlayerPacket::updateMap(ChannelServer::Instance()->getWorldPlayer(), player->getPlayerid(), mapid);
	MapPacket::changeMap(player);
	newMap(player, mapid);
}

void Maps::mapTimer(int mapid) {
	clock_t time = clock();
	maps[mapid]->checkReactorSpawn(time);
	maps[mapid]->checkMobSpawn(time);
	maps[mapid]->clearDrops(time);
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
