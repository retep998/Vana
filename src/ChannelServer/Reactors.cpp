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
#include "Reactors.h"
#include "Player.h"
#include "ReactorPacket.h"
#include "InventoryPacket.h"
#include "PacketCreator.h"
#include "Maps.h"
#include "Mobs.h"
#include "Inventory.h"
#include "BufferUtilities.h"
#include "SendHeader.h"

hash_map <int, vector<ReactorEventInfo>> Reactors::reactorinfo;
hash_map <int, ReactorSpawnsInfo> Reactors::info;
hash_map <int, vector<Reactor*>> Reactors::reactors;
int Reactors::reactorscount = 0x200;

void Reactors::addReactorSpawn(int id, ReactorSpawnsInfo reactorspawns) {
	info[id] = reactorspawns;
}

void Reactors::addReactorEventInfo(int id, ReactorEventInfo revent) {
	reactorinfo[id].push_back(revent);
}

void Reactors::loadReactors() {
	for (hash_map<int, ReactorSpawnsInfo>::iterator iter = Reactors::info.begin(); iter != Reactors::info.end(); iter++) {
		for (unsigned int i=0; i<iter->second.size(); i++) {
			Reactor *reactor = new Reactor();
			Pos pos;
			pos.x = iter->second[i].x;
			pos.y = iter->second[i].y;
			reactor->setID(Reactors::reactorscount++);
			reactor->setReactorID(iter->second[i].id);
			reactor->setMapID(iter->first);
			reactor->setPos(pos);
			reactors[iter->first].push_back(reactor);
		}
	}
}

void Reactors::showReactors(Player *player) {
	for (unsigned int i=0; i<reactors[player->getMap()].size(); i++) {
		if (reactors[player->getMap()][i]->isAlive()) {
			ReactorPacket::showReactor(player, reactors[player->getMap()][i]);
		}
	}
}

Reactor* Reactors::getReactorByID(int id, int mapid) {
	for (unsigned int i=0; i<reactors[mapid].size(); i++) {
		if (reactors[mapid][i]->getID() == id) {
			return reactors[mapid][i];
		}
	}
	return NULL;
}

void Reactors::hitReactor(Player *player, unsigned char *packet) {
	int id = BufferUtilities::getInt(packet);

	Reactor* reactor = getReactorByID(id, player->getMap());

	if (reactor != NULL && reactor->isAlive()) {
		for (unsigned int i=0; i<reactorinfo[reactor->getReactorID()].size(); i++) {
			if (reactor->getState() == reactorinfo[reactor->getReactorID()][i].state) {
				if (reactorinfo[reactor->getReactorID()][i].type >= 100)
					return;
				reactor->setState(reactorinfo[reactor->getReactorID()][i].nextstate);
				ReactorPacket::triggerReactor(player, Maps::info[player->getMap()].Players, reactor);
				return;
			}
		}
		reactor->kill();
		ReactorPacket::destroyReactor(player, Maps::info[player->getMap()].Players, reactor);
	}
}

void Reactors::checkDrop(Player *player, Drop *drop) {
	if (drop->getMesos())
		return;
	for (unsigned int i=0; i<reactors[drop->getMap()].size(); i++) {
		Reactor* reactor = reactors[drop->getMap()][i];
		for (unsigned int j=0; j<reactorinfo[reactors[drop->getMap()][i]->getReactorID()].size(); j++) {
			ReactorEventInfo revent = reactorinfo[reactors[drop->getMap()][i]->getReactorID()][j];
			if (reactor->getState() == revent.state && drop->getID() == revent.itemid) {
				if ((drop->getPos().x >= reactor->getPos().x+revent.ltx && drop->getPos().x <= reactor->getPos().x+revent.rbx) && (drop->getPos().y >= reactor->getPos().y+revent.lty && drop->getPos().y <= reactor->getPos().y+revent.rby)) {
					reactor->setState(revent.nextstate);
					ReactorPacket::triggerReactor(player, Maps::info[player->getMap()].Players, reactor);
					drop->removeDrop();
					if (reactor->getReactorID() == 2201004) {
						Maps::changeMusic(reactor->getMapID(), "Bgm09/TimeAttack");
						Mobs::spawnMobPos(player, 8500000, -410, -400);
					}
				}
				return;
			}
		}
	}
}
