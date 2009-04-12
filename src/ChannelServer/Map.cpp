/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "GameConstants.h"
#include "Instance.h"
#include "MapPacket.h"
#include "MapleSession.h"
#include "MapleTVs.h"
#include "Mobs.h"
#include "MobsPacket.h"
#include "NPCPacket.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Randomizer.h"
#include "ReactorPacket.h"
#include "Reactors.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <ctime>
#include <functional>

using std::tr1::bind;

Map::Map (MapInfoPtr info) :
info(info),
spawnpoints(0),
objectids(1000),
instance(0),
timer(0),
timerstart(0)
{
	new Timer::Timer(bind(&Map::runTimer, this), // Due to dynamic loading, we can now simply start the map timer once the object is created
		Timer::Id(Timer::Types::MapTimer, info->id, 0),
		0, 0, 10000);
}

// Map Info
void Map::setMusic(const string &musicname) {
	getInfo()->musicname = musicname;
	MapPacket::setMusic(getInfo()->id, musicname);
}

// Players
void Map::addPlayer(Player *player) {
	this->players.push_back(player);
	if (info->fieldType == 82 || info->fieldType == 81) // Apple training maps/Showa spa
		MapPacket::forceMapEquip(player);
	if (player->getActiveBuffs()->getActiveSkillLevel(Jobs::SuperGM::Hide) == 0)
		MapPacket::showPlayer(player);
	if (timer > 0)
		MapPacket::showTimer(player, timer - static_cast<int32_t>(time(0) - timerstart));
	else if (instance != 0 && instance->checkInstanceTimer() > 0)
		MapPacket::showTimer(player, instance->checkInstanceTimer());
}

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i] == player) {
			this->players.erase(this->players.begin() + i);
			break;
		}
	}
	Summons::removeSummon(player, true, false, false);
	Summons::removeSummon(player, false, false, true);
	MapPacket::removePlayer(player);
	updateMobControl(player);
}

// Reactors
void Map::addReactorSpawn(ReactorSpawnInfo spawn) {
	reactorspawns.push_back(spawn);
	Reactor *reactor = new Reactor(info->id, spawn.id, spawn.pos);
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
		int32_t id = reactorrespawns[i].id;
		if ((time - reactorrespawns[i].killed) > (reactorspawns[id].time * 1000)) {
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
	int16_t x = pos.x;
	int16_t y = pos.y - 100;
	int16_t maxy = pos.y;
	bool firstcheck = true;
	for (size_t i = 0; i < footholds.size(); i++) {
		if ((x > footholds[i].pos1.x && x <= footholds[i].pos2.x) || (x > footholds[i].pos2.x && x <= footholds[i].pos1.x)) {
			int16_t cmax = (int16_t)((float)(footholds[i].pos1.y - footholds[i].pos2.y) / (footholds[i].pos1.x - footholds[i].pos2.x) * (x - footholds[i].pos1.x) + footholds[i].pos1.y);
			if ((cmax <= maxy || (maxy == pos.y && firstcheck)) && cmax >= y) {
				maxy = cmax;
				firstcheck = false;
			}
		}
	}
	return Pos(x, maxy);
}

// Portals
PortalInfo * Map::getSpawnPoint(int32_t pid) {
	int32_t id = (pid != -1 ? pid : Randomizer::Instance()->randInt(spawnpoints.size() - 1));
	return &spawnpoints[id];
}

// Mobs
void Map::addMobSpawn(MobSpawnInfo spawn) {
	mobspawns.push_back(spawn);
	spawnMob(spawn.id, spawn.pos, mobspawns.size() - 1, spawn.fh);
}

void Map::checkMobSpawn(clock_t time) {
	// (Re-)spawn Mobs
	for (size_t i = 0; i < mobrespawns.size(); i++) {
		int32_t id = mobrespawns[i].spawnid;
		if ((time - mobrespawns[i].killed) > (mobspawns[id].time * 1000)) {
			spawnMob(mobspawns[id].id, mobspawns[id].pos, id, mobspawns[id].fh);
			mobrespawns.erase(mobrespawns.begin() + i);
			i--;
		}
	}
}

void Map::spawnMob(int32_t mobid, Pos pos, int32_t spawnid, int16_t fh, Mob *owner) {
	int32_t id = objectids.next();

	Mob *mob = new Mob(id, info->id, mobid, pos, spawnid, fh);
	mobs[id] = mob;

	MobsPacket::spawnMob(0, mob, owner, (owner == 0));
	updateMobControl(mob, true);
}

Mob * Map::getMob(int32_t id, bool isMapID) {
	if (isMapID)
		return (this->mobs.find(id) != mobs.end() ? this->mobs[id] : 0);
	else {
		for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
			if (iter->second != 0) {
				if (iter->second->getMobID() == id)
					return iter->second;
			}
		}
	}
	return 0;
}

void Map::updateMobControl(Player *player) {
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0 && iter->second->getControl() == player)
			updateMobControl(iter->second);
	}
}

void Map::updateMobControl(Mob *mob, bool spawn) {
	if (players.size() > 0 && mob->getControl() == 0) {
		int32_t maxpos = mob->getPos() - players[0]->getPos();
		int32_t player = 0;
		for (size_t j = 0; j < players.size(); j++) {
			int32_t curpos = mob->getPos() - players[j]->getPos();
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

void Map::removeMob(int32_t id, int32_t spawnid) {
	if (mobs.find(id) != mobs.end()) {
		if (spawnid > -1 && mobspawns[spawnid].time > -1) // Add spawn point to respawns if mob was spawned by a spawn point.
			mobrespawns.push_back(MobRespawnInfo(spawnid, TimeUtilities::clock_in_ms()));
		this->mobs.erase(id);
	}
}

int32_t Map::killMobs(Player *player, int32_t mobid, bool playerkill, bool showpacket) {
	unordered_map<int32_t, Mob *> mobs = this->mobs;
	int32_t mobskilled = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) { // While loops cause problems
		if (iter->second != 0) {
			if ((mobid > 0 && iter->second->getMobID() == mobid) || mobid == 0) {
				if (playerkill && player != 0) {
					iter->second->applyDamage(player->getId(), iter->second->getHP());
				}
				else
					iter->second->die(showpacket);
				mobskilled++;
			}
		}
	}
	return mobskilled;
}

int32_t Map::countMobs(int32_t mobid) {
	unordered_map<int32_t, Mob *> mobs = this->mobs;
	int32_t mobcount = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0) {
			if ((mobid > 0 && iter->second->getMobID() == mobid) || mobid == 0)
				mobcount++;
		}
	}
	return mobcount;
}

// Drops
void Map::addDrop(Drop *drop) {
	boost::recursive_mutex::scoped_lock l(drops_mutex);
	int32_t id = objectids.next();
	drop->setID(id);
	drop->setPos(findFloor(drop->getPos()));
	this->drops[id] = drop;
}

void Map::clearDrops(bool showPacket) { // Clear all drops
	boost::recursive_mutex::scoped_lock l(drops_mutex);
	unordered_map<int32_t, Drop *> drops = this->drops;
	for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0) // Check just in case drop is removed by timer
			iter->second->removeDrop(showPacket);
	}
}

void Map::clearDrops(clock_t time) { // Clear drops based on how long they have been in the map
	boost::recursive_mutex::scoped_lock l(drops_mutex);
	time -= 180000; // Drops disappear after 3 minutes
	unordered_map<int32_t, Drop *> drops = this->drops;
	for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
		if (iter->second != 0) {
			if (iter->second->getDropped() < time)
				iter->second->removeDrop();
		}
	}
}

void Map::runTimer() {
	clock_t time = TimeUtilities::clock_in_ms();
	checkReactorSpawn(time);
	checkMobSpawn(time);
	clearDrops(time);
}

void Map::setMapTimer(int32_t t) {
	if (t > 0 && timer != 0)
		return;
	timer = t;
	timerstart = time(0);
	MapPacket::showTimer(info->id, t);
	if (t > 0) {
		new Timer::Timer(bind(&Map::setMapTimer, this, 0),
			Timer::Id(Timer::Types::MapTimer, info->id, 25),
			0, Timer::Time::fromNow(t * 1000));
	}
}

void Map::showObjects(Player *player) { // Show all Map Objects
	// Music
	if (info->musicname.size() > 0)
		MapPacket::setMusic(info->id, info->musicname);

	// MapleTV messengers
	if (MapleTVs::Instance()->isMapleTVMap(info->id) && MapleTVs::Instance()->hasMessage()) {
		PacketCreator packet;
		MapleTVs::Instance()->getMapleTVEntryPacket(packet);
		player->getSession()->send(packet);
	}

	// Players
	for (size_t i = 0; i < players.size(); i++) {
		if (player != players[i] && players[i]->getActiveBuffs()->getActiveSkillLevel(Jobs::SuperGM::Hide) == 0) {
			PacketCreator packet = MapPacket::playerPacket(players[i]);
			player->getSession()->send(packet);
			Summons::showSummons(players[i], player);
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
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != 0) {
			MobsPacket::spawnMob(player, iter->second, 0, false, true);
			updateMobControl(iter->second);
		}
	}
	// Drops
	{
		boost::recursive_mutex::scoped_lock l(drops_mutex);
		for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
			if (iter->second != 0) {
				iter->second->showDrop(player);
			}
		}
	}

	if (player->getParty()) {
		player->getParty()->showHPBar(player);
		player->getParty()->receiveHPBar(player);
	}

	if (info->clock) {
		time_t rawtime;
		time(&rawtime);
		struct tm *timeinfo = localtime(&rawtime);
		MapPacket::showClock(player, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	}
}

void Map::sendPacket(PacketCreator &packet, Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i] != player) {
			this->players[i]->getSession()->send(packet);
		}
	}
}

void Map::showMessage(string &message, int8_t type) {
	for (size_t i = 0; i < players.size(); i++)
		PlayerPacket::showMessage(players[i], message, type);
}