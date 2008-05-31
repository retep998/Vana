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
#include "Player.h"
#include "PlayerPacket.h"
#include "MapPacket.h"
#include "PlayersPacket.h"
#include "NPCs.h"
#include "Reactors.h"
#include "Mobs.h"
#include "Drops.h"
#include "Timer.h"
#include "LuaPortal.h"
#include "BufferUtilities.h"
#include <iostream>
#include <sstream>
#include <string>

hash_map <int, MapInfo> Maps::info;

class MapTimer: public Timer::TimerHandler {
public:
	static MapTimer * Instance() {
		if (singleton == 0)
			singleton = new MapTimer;
		return singleton;
	}
	void setMapTimer(int mapid){
		if(ctimer.find(mapid) != ctimer.end())
			if(ctimer[mapid])
			return;
		Maps::mapTimer(mapid);
		timers[Timer::Instance()->setTimer(10000, this)] = mapid;
		ctimer[mapid] = 1;
	}
	void next(int mapid, int is){
		if(is)
			timers[Timer::Instance()->setTimer(10000, this)] = mapid;
		else
			ctimer[mapid] = 0;
	}
private:
	static MapTimer *singleton;
	MapTimer() {};
	MapTimer(const MapTimer&);
	MapTimer& operator=(const MapTimer&);

	static hash_map <int, int> timers;
	static hash_map <int, int> ctimer;
	void handle (Timer* timer, int id) {
		Maps::mapTimer(timers[id]);
	}
	void remove (int id){
		timers.erase(id);
	}
};
hash_map <int,int> MapTimer::timers;
hash_map <int,int> MapTimer::ctimer;
MapTimer * MapTimer::singleton = 0;

void Maps::addMap(int id, MapInfo map){
	info[id] = map;
}

void Maps::addPlayer(Player* player) {
	if(player->getMap() == 1 || player->getMap() == 2)
		MapPacket::makeApple(player);
	info[player->getMap()].Players.push_back(player);
	MapPacket::showPlayers(player, info[player->getMap()].Players);
	if (player->skills->getActiveSkillLevel(5101004) == 0)
		MapPacket::showPlayer(player, info[player->getMap()].Players);
}

void Maps::removePlayer(Player* player){
	for(unsigned int i=0; i<info[player->getMap()].Players.size(); i++) {
		if(info[player->getMap()].Players[i]->getPlayerid() == player->getPlayerid()) {
			info[player->getMap()].Players.erase(info[player->getMap()].Players.begin()+i);
		}
	}
	MapPacket::removePlayer(player, info[player->getMap()].Players);
	for (hash_map<int, Mob *>::iterator iter = Mobs::mobs[player->getMap()].begin(); iter != Mobs::mobs[player->getMap()].end(); iter++) {
		if(iter->second->getControl() == player) {
			iter->second->setControl(NULL);
		}
	}
	Mobs::updateSpawn(player->getMap());
}

void Maps::moveMap(Player* player, unsigned char* packet){
	if(BufferUtilities::getInt(packet+1) == 0){
		int tomap;
		if(info.find(player->getMap())==info.end())
			tomap = player->getMap();
		else
			tomap = info[player->getMap()].rm;
		player->setHP(50, 0);
		changeMap(player, tomap, 0); // TODO - Random
		return;
	}
	int portalsize = packet[5];
	char portalname[10];
	BufferUtilities::getString(packet+7, portalsize, portalname);;   
	PortalInfo portal;
	for(unsigned int i=0; i<info[player->getMap()].Portals.size(); i++)
		if(strcmp(info[player->getMap()].Portals[i].from, portalname) == 0){
			portal = info[player->getMap()].Portals[i];
			break;
		}
	int tonum = 0;
	if(info.find(portal.toid) != info.end()){
		for(unsigned int i=0; i<info[portal.toid].Portals.size(); i++){
			if(strcmp(portal.to, info[portal.toid].Portals[i].from) ==0){
				tonum = info[portal.toid].Portals[i].id;
				break;
			}
		}
	}
	changeMap(player, portal.toid, tonum);
}

void Maps::moveMapS(Player* player, unsigned char* packet){ // Move to map special
	char portalname[10];
	int namelen = packet[1];
	BufferUtilities::getString(packet+3, namelen, portalname);

	PortalInfo portal;
	for (unsigned int i=0; i<info[player->getMap()].Portals.size(); i++) {
		if (strcmp(info[player->getMap()].Portals[i].from, portalname) == 0) {
			portal = info[player->getMap()].Portals[i];
			break;
		}
	}

	std::ostringstream filenameStream;
	filenameStream << "scripts/portals/" << portal.script << ".lua";
	LuaPortal(filenameStream.str(), player->getPlayerid(), &portal);
	int tonum = 0;
	if(info.find(portal.toid) != info.end()){
		for(unsigned int i=0; i<info[portal.toid].Portals.size(); i++){
			if(strcmp(portal.to, info[portal.toid].Portals[i].from) ==0){
				tonum = info[portal.toid].Portals[i].id;
				break;
			}
		}
	}
	changeMap(player, portal.toid, tonum);
}

void Maps::changeMap(Player* player, int mapid, int pos){
	if (mapid == 999999999) {
		PlayerPacket::showMessage(player, "This portal is currently unavailable.", 5);
		MapPacket::portalBlocked(player);
		return;
	}
	else if (mapid < 0) {
		MapPacket::portalBlocked(player);
		return;
	}
	removePlayer(player);
	player->setMap(mapid);
	player->setMappos(pos);
	player->setType(0);
	Pos cpos;
	if((unsigned int)pos<info[mapid].Portals.size()){
		cpos.x = info[mapid].Portals[pos].x;
		cpos.y = info[mapid].Portals[pos].y;
	}
	else if(info[mapid].Portals.size() > 0){
		cpos.x = info[mapid].Portals[0].x;
		cpos.y = info[mapid].Portals[0].y;
	}
	else{
		cpos.x = 0;
		cpos.y = 0;
	}
	player->setPos(cpos);
	MapPacket::changeMap(player);
	newMap(player, mapid);
}

void Maps::showClock(Player* player){
	time_t rawtime;
	struct tm timeinfo;
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	MapPacket::showClock(player, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void Maps::mapTimer(int mapid){
	if(info[mapid].Players.size() == 0){
		MapTimer::Instance()->next(mapid, 0);
		return;
	}
	else
		MapTimer::Instance()->next(mapid, 1);
	Mobs::checkSpawn(mapid);
	Drops::checkDrops(mapid);
}

void Maps::newMap(Player* player, int mapid){
	Players::addPlayer(player);
	NPCs::showNPCs(player);
	Reactors::showReactors(player);
	addPlayer(player);
	Mobs::showMobs(player);
	Drops::showDrops(player);
	if(info[mapid].clock)
		showClock(player);
	MapTimer::Instance()->setMapTimer(player->getMap());
}
// Change Music
void Maps::changeMusic(int mapid, std::string musicname){
	if(Maps::info.find(mapid) != Maps::info.end()){
		MapPacket::changeMusic(info[mapid].Players, musicname);
	}
}
