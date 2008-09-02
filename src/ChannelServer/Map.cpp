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
#include "Map.h"
#include "Drops.h"
#include "LoopingId.h"
#include "MapPacket.h"
#include "Mobs.h"
#include "MobsPacket.h"
#include "NPCPacket.h"
#include "PacketCreator.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "ReactorPacket.h"
#include "Reactors.h"
#include "Randomizer.h"
#include "SkillsPacket.h"
#include "Timer/Timer.h"
#include <ctime>
#include <functional>

using std::tr1::bind;

Map::Map (MapInfo info) :
info(info),
spawnpoints(0),
objectids(new LoopingId(1000)),
timer_started(false)
{
}

// Players
void Map::addPlayer(Player *player) {
	setTimer(); // Setup the timer if this is the first player to enter the map

	this->players.push_back(player);
	if (info.fieldType == 82)
		MapPacket::makeApple(player);
	if (player->getSkills()->getActiveSkillLevel(9101004) == 0)
		MapPacket::showPlayer(player);
}

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i] == player) {
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
	Reactor *reactor = new Reactor(info.id, spawn.id, spawn.pos);
	ReactorPacket::spawnReactor(reactor);
}

void Map::addReactor(Reactor *reactor) {
	this->reactors.push_back(reactor);
	reactor->setID(this->reactors.size() - 1 + 200);
}

void Map::addReactorRespawn(ReactorRespawnInfo respawn) {
	if (reactorspawns[respawn.id].time >= 0)
		reactorrespawns.push_back(respawn);
}

void Map::checkReactorSpawn(clock_t time) {
	for (size_t i = 0; i < reactorrespawns.size(); i++) {
		int id = reactorrespawns[i].id;
		if ((time - reactorrespawns[i].killed) > (reactorspawns[id].time * CLOCKS_PER_SEC)) {
			getReactor(id)->restore();
			reactorrespawns.erase(reactorrespawns.begin() + i);
			i--;
		}
	}
}

// Footholds
Pos Map::findFloor(Pos pos) {
	// Determines where a drop falls using the footholds data
	// to check the platforms and find the correct one.
	short x = pos.x;
	short y = pos.y - 100;
	short maxy = pos.y;
	bool firstcheck = true;
	for (size_t i = 0; i < footholds.size(); i++) {
		if ((x >= footholds[i].pos1.x && x <= footholds[i].pos2.x) || (x >= footholds[i].pos2.x && x <= footholds[i].pos1.x)) {
			short cmax = (short)((float)(footholds[i].pos1.y - footholds[i].pos2.y) / (footholds[i].pos1.x - footholds[i].pos2.x) * (x - footholds[i].pos1.x) + footholds[i].pos1.y);
			if ((cmax <= maxy || (maxy == pos.y && firstcheck)) && cmax >= y) {
				maxy = cmax;
				firstcheck = false;
			}
		}
	}
	return Pos(x, maxy);
}

// Portals
PortalInfo * Map::getSpawnPoint(int pid) {
	int id = 0;
	if (pid != -1)
		id = pid;
	else
		id = Randomizer::Instance()->randInt(spawnpoints - 1);
	return &portals[id];
}

// Mobs
void Map::addMobSpawn(MobSpawnInfo spawn) {
	mobspawns.push_back(spawn);
	spawnMob(spawn.id, spawn.pos, mobspawns.size()-1, spawn.fh);
}

void Map::checkMobSpawn(clock_t time) {
	// (Re-)spawn Mobs
	for (size_t i = 0; i < mobrespawns.size(); i++) {
		int id = mobrespawns[i].spawnid;
		if ((time - mobrespawns[i].killed) > (mobspawns[id].time * CLOCKS_PER_SEC)) {
			spawnMob(mobspawns[id].id, mobspawns[id].pos, id, mobspawns[id].fh);
			mobrespawns.erase(mobrespawns.begin()+i);
			i--;
		}
	}
}

void Map::spawnMob(int mobid, Pos pos, int spawnid, short fh) {
	int id = this->objectids->next();
	
	Mob *mob = new Mob(id, info.id, mobid, pos, spawnid, fh);
	mobs[id] = mob;

	MobsPacket::spawnMob(0, mob, false, true);
	updateMobControl(mob, true);
}

void Map::updateMobControl() {
	for (unordered_map<int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			updateMobControl(iter->second);
	}
}

void Map::updateMobControl(Mob *mob, bool spawn) {
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

void Map::removeMob(int id, int spawnid) {
	if (mobs.find(id) != mobs.end()) {
		if (spawnid > -1 && mobspawns[spawnid].time > -1) // Add spawn point to respawns if mob was spawned by a spawn point.
			mobrespawns.push_back(MobRespawnInfo(spawnid, clock()));
		this->mobs.erase(id);
	}
}

void Map::killMobs(Player *player) {
	unordered_map<int, Mob *> mobs = this->mobs;
	for (unordered_map<int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) { // While loops cause problems
		if (iter->second != 0)
			iter->second->die(player);
	}
}

void Map::killMobs(Player *player, int mobid) {
	unordered_map<int, Mob *> mobs = this->mobs;
	for (unordered_map<int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			if (iter->second->getMobID() == mobid)
				iter->second->die(player);
	}
}

void Map::killMobs() {
	unordered_map<int, Mob *> mobs = this->mobs;
	for (unordered_map<int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) { // Remove 'em, no EXP, no summoning
		if (iter->second != 0)
			iter->second->die();
	}
}

// Drops
void Map::addDrop(Drop *drop) {
	int id = objectids->next();
	drop->setID(id);
	drop->setPos(findFloor(drop->getPos()));
	this->drops[id] = drop;
}

void Map::clearDrops(bool showPacket) { // Clear all drops
	unordered_map<int, Drop *> drops = this->drops;
	for (unordered_map<int, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0) // Check just in case drop is removed by timer
			iter->second->removeDrop(showPacket);
	}
}

void Map::clearDrops(int time) { // Clear drops based on how long they have been in the map
	time -= 180000;
	unordered_map<int, Drop *> drops = this->drops;
	for (unordered_map<int, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0)
			if (iter->second->getDropped() < time)
				iter->second->removeDrop();
	}
}

void Map::setTimer() {
	if (!timer_started) {
		new Timer::Timer(bind(&Map::runTimer, this),
			Timer::Id(Timer::Types::MapTimer, info.id, 0),
			0, 10000, true);
	}
	timer_started = true;
}

void Map::runTimer() {
	clock_t time = clock();
	checkReactorSpawn(time);
	checkMobSpawn(time);
	clearDrops(time);
}

void Map::showObjects(Player *player) { // Show all Map Objects
	// Players
	for (size_t i = 0; i < players.size(); i++) {
		if (player != players[i] && players[i]->getSkills()->getActiveSkillLevel(9101004) == 0) {
			PacketCreator packet = MapPacket::playerPacket(players[i]);
			player->getPacketHandler()->send(packet);
			// Bug in global; would be fixed here:
			// Hurricane/Pierce do not display properly if using when someone enters the map
			// Berserk does not display properly either - players[i]->getActiveBuffs()->getBerserk()
		}
	}
	// NPCs
	for (size_t i = 0; i < npcs.size(); i++) {
		NPCPacket::showNPC(player, npcs[i], i);
	}
	// Reactors
	for (size_t i = 0; i < reactors.size(); i++) {
		if (reactors[i]->isAlive())
			ReactorPacket::showReactor(player, reactors[i]);
	}
	// Mobs
	for (unordered_map<int, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0)
			MobsPacket::spawnMob(player, iter->second, false, false, true);
	}
	updateMobControl();
	// Drops
	for (unordered_map<int, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0)
			iter->second->showDrop(player);
	}
	if (info.clock) {
		time_t rawtime;
		struct tm timeinfo;
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		MapPacket::showClock(player, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
	}
}

void Map::sendPacket(PacketCreator &packet, Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i] != player) {
			this->players[i]->getPacketHandler()->send(packet);
		}
	}
}

void Map::showMessage(string &message, char type) {
	for (size_t i = 0; i < players.size(); i++)
		PlayerPacket::showMessage(players[i], message, type);
}
