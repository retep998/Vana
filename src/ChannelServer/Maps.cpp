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
#include <ctime>
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
		// In our current skill system, active skills are retained through death
		// While not the most efficient way to do this, it works and until we fix the skill system, it'll have to do
		// Active skills do not get deleted, merely set back to level 0 upon stopping
		// Therefore, we hackishly trigger stopping of each active skill ID
		// TODO: Fix skill system
		unsigned int i = player->skills->getActiveSkillsNum();
		while (i > 0) {
			int skillid = player->skills->getActiveSkillsID(i - 1); // Indexes are 0-based
			if (skillid > 0)
				Skills::stopSkill(player, skillid);
			i--;
		}
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
	maps[mapid]->checkSpawn(clock());
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
