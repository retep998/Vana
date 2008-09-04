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
#include "LuaPortal.h"
#include "MapPacket.h"
#include "Pets.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "ReadPacket.h"
#include "Timer/Timer.h"
#include "WorldServerConnectPlayerPacket.h"
#include <sstream>

unordered_map<int, Map *> Maps::maps;

void Maps::addMap(MapInfo info) {
	maps[info.id] = new Map(info);
}

void Maps::usePortal(Player *player, PortalInfo *portal) {
	bool scripted = (strlen(portal->script) != 0);
	if (scripted) {
		// Scripted portal
		std::ostringstream filenameStream;
		filenameStream << "scripts/portals/" << portal->script << ".lua";
		LuaPortal(filenameStream.str(), player->getId(), portal);
	}

	PortalInfo *nextportal = 0;
	if (portal->toid >= 0 && portal->toid != 999999999) { // Only check for new portal ID if a portal script returns a valid map
		nextportal = maps[portal->toid]->getPortal(portal->to);
	}
	else if (portal->toid == 999999999) {
		std::ostringstream messageStream;
		if (scripted) {
			messageStream << "This portal '" << portal->script << "' is currently unavailable.";
		}
		else {
			messageStream << "This portal is currently unavailable.";
		}
		PlayerPacket::showMessage(player, messageStream.str(), 5);
	}

	changeMap(player, portal->toid, nextportal);
}

void Maps::usePortal(Player *player, ReadPacket *packet) {
	packet->skipBytes(1);
	if (packet->getInt() == 0) { // Dead
		player->acceptDeath();
		return;
	}
	string portalname = packet->getString();

	PortalInfo *portal = maps[player->getMap()]->getPortal(portalname.c_str());
	if (portal == 0) // Exit the function if portal is not found
		return;

	usePortal(player, portal);
}

void Maps::useScriptedPortal(Player *player, ReadPacket *packet) {
	packet->skipBytes(1);
	string portalname = packet->getString();

	PortalInfo *portal = maps[player->getMap()]->getPortal(portalname.c_str());
	if (portal == 0) // Exit the function if portal is not found
		return;

	usePortal(player, portal);
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
	player->setPos(Pos(portal->pos.x, portal->pos.y - 40));
	player->setStance(0);
	player->setFH(0);
	for (char i = 0; i < 3; i++) {
		if (player->getPets()->getSummoned(i)) {
			player->getPets()->getPet(player->getPets()->getSummoned(i))->setPos(portal->pos);
		}
	}
	WorldServerConnectPlayerPacket::updateMap(ChannelServer::Instance()->getWorldPlayer(), player->getId(), mapid);
	MapPacket::changeMap(player);
	newMap(player, mapid);
}

void Maps::newMap(Player *player, int mapid) {
	Players::Instance()->addPlayer(player);
	maps[mapid]->addPlayer(player);
	maps[mapid]->showObjects(player);
	Pets::showPets(player);
	// Bug in global - would be fixed here:
	// Berserk doesn't display properly when switching maps with it activated - client displays, but no message is sent to any client
	// player->getActiveBuffs()->checkBerserk(true) would override the default of only displaying changes
}

// Change Music
void Maps::changeMusic(int mapid, const string &musicname) {
	if (Maps::maps.find(mapid) != Maps::maps.end()) {
		MapPacket::changeMusic(mapid, musicname);
	}
}
