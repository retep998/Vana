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
#include "GameLogicUtilities.h"
#include "Instance.h"
#include "MapPacket.h"
#include "MapleSession.h"
#include "MapleTVs.h"
#include "Mist.h"
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
timerstart(0),
poisonmists(0)
{
	new Timer::Timer(bind(&Map::runTimer, this), // Due to dynamic loading, we can now simply start the map timer once the object is created
		Timer::Id(Timer::Types::MapTimer, info->id, 0),
		getTimers(), 0, 10000);
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
	if (!player->getActiveBuffs()->isUsingHide())
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

void Map::dispelPlayers(int16_t prop, const Pos &origin, const Pos &lt, const Pos &rb) {
	for (size_t i = 0; i < players.size(); i++) {
		Player *dispelee = players[i];
		if (dispelee != 0 && GameLogicUtilities::isInBox(origin, lt, rb, dispelee->getPos()) && Randomizer::Instance()->randShort(99) < prop) {
			dispelee->getActiveBuffs()->dispelBuffs();
		}
	}
}

void Map::statusPlayers(uint8_t status, uint8_t level, int16_t count, int16_t prop, const Pos &origin, const Pos &lt, const Pos &rb) {
	int16_t done = 0;
	for (size_t i = 0; i < players.size(); i++) {
		Player *toy = players[i];
		if (toy != 0) {
			if (GameLogicUtilities::isInBox(origin, lt, rb, toy->getPos()) && Randomizer::Instance()->randShort(99) < prop) {
				toy->getActiveBuffs()->addDebuff(status, level);
				done++;
			}
		}
		if (count > 0 && done == count)
			break;
	}
}

// Reactors
void Map::addReactorSpawn(ReactorSpawnInfo spawn) {
	reactorspawns.push_back(spawn);
	Reactor *reactor = new Reactor(info->id, spawn.id, spawn.pos);
	ReactorPacket::spawnReactor(reactor);
}

void Map::addReactor(Reactor *reactor) {
	this->reactors.push_back(reactor);
	reactor->setId(this->reactors.size() - 1 + 200);
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

int16_t Map::getFhAtPosition(Pos pos) {
	int16_t foothold = 0;
	for (size_t i = 0; i < footholds.size(); i++) {
		FootholdInfo cur = footholds[i];
		if (((pos.x > cur.pos1.x && pos.x <= cur.pos2.x) || (pos.x > cur.pos2.x && pos.x <= cur.pos1.x)) && ((pos.y > cur.pos1.x && pos.y <= cur.pos2.x) || (pos.y > cur.pos2.x && pos.y <= cur.pos1.x))) {
			foothold = cur.id;
			break;
		}
	}
	return foothold;
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
		if (mobrespawns[i].spawnat < time) {
			spawnMob(mobspawns[id].id, mobspawns[id].pos, id, mobspawns[id].fh);
			mobrespawns.erase(mobrespawns.begin() + i);
			i--;
		}
	}
}

int32_t Map::spawnMob(int32_t mobid, Pos pos, int32_t spawnid, int16_t fh, Mob *owner, int8_t summoneffect) {
	int32_t id = objectids.next();

	Mob *mob = new Mob(id, info->id, mobid, pos, spawnid, fh);
	mobs[id] = mob;
	if (summoneffect != 0) {
		mob->setOwner(owner);
		owner->addSpawn(id, mob);
	}
	MobsPacket::spawnMob(0, mob, summoneffect, owner, (owner == 0));
	updateMobControl(mob, true);
	return id;
}

Mob * Map::getMob(int32_t id, bool isMapId) {
	if (isMapId)
		return (this->mobs.find(id) != mobs.end() ? this->mobs[id] : 0);
	else {
		for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
			if (iter->second != 0) {
				if (iter->second->getMobId() == id)
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
	int32_t maxpos = 200000;
	Player *p = 0;
	for (size_t j = 0; j < players.size(); j++) {
		Player *test = players[j];
		if (!(test->getActiveBuffs()->isUsingHide())) {
			int32_t curpos = mob->getPos() - test->getPos();
			if (curpos < maxpos) {
				maxpos = curpos;
				p = test;
				break;
			}
		}
	}
	mob->setControl(p);
}

void Map::removeMob(int32_t id, int32_t spawnid) {
	if (mobs.find(id) != mobs.end()) {
		if (spawnid > -1 && mobspawns[spawnid].time > -1) { // Add spawn point to respawns if mob was spawned by a spawn point.
			clock_t spawntime = mobspawns[spawnid].time * 1000 * (Randomizer::Instance()->randInt(100) + 100) / 100; // Randomly spawn between 1x and 2x the spawn time
			mobrespawns.push_back(MobRespawnInfo(spawnid, TimeUtilities::getTickCount() + spawntime));
		}
		this->mobs.erase(id);
	}
}

int32_t Map::killMobs(Player *player, int32_t mobid, bool playerkill, bool showpacket) {
	unordered_map<int32_t, Mob *> mobs = this->mobs;
	int32_t mobskilled = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) { // While loops cause problems
		if (iter->second != 0) {
			if ((mobid > 0 && iter->second->getMobId() == mobid) || mobid == 0) {
				if (playerkill && player != 0) {
					if (iter->second->getMobId() != Mobs::HorntailSponge) { // This will be taken care of by its parts
						iter->second->applyDamage(player->getId(), iter->second->getHp());
					}
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
			if ((mobid > 0 && iter->second->getMobId() == mobid) || mobid == 0)
				mobcount++;
		}
	}
	return mobcount;
}

void Map::healMobs(int32_t hp, int32_t mp, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobmap = this->mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
		if (iter->second != 0 && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->skillHeal(hp, mp);
		}
	}
}

void Map::statusMobs(const vector<StatusInfo> &statuses, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobmap = this->mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
		if (iter->second != 0 && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->addStatus(0, statuses);
		}
	}
}

void Map::checkShadowWeb() {
	unordered_map<int32_t, Mob *> mobmap = this->mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
		if (iter->second != 0 && iter->second->hasStatus(StatusEffects::Mob::ShadowWeb)) {
			iter->second->applyWebDamage();
		}
	}
}

// Drops
void Map::addDrop(Drop *drop) {
	boost::recursive_mutex::scoped_lock l(drops_mutex);
	int32_t id = objectids.next();
	drop->setId(id);
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

// Mists
void Map::addMist(Mist *mist) {
	mists[mist->getId()] = mist;

	Timer::Id id(Timer::Types::MistTimer, mist->getId(), 0);
	new Timer::Timer(bind(&Map::removeMist, this, mist->getId()),
		id, getTimers(), Timer::Time::fromNow(mist->getTime() * 1000));

	MapPacket::spawnMist(info->id, mist);
	if (mist->isPoison() && !mist->isMobMist())
		poisonmists++;
}

Mist * Map::getMist(int32_t id) {
	return (mists.find(id) != mists.end() ? mists[id] : 0);
}

void Map::removeMist(int32_t id) {
	if (mists.find(id) != mists.end()) {
		Mist *f = mists[id];
		if (f->isPoison() && !f->isMobMist())
			poisonmists--;
		mists.erase(id);
		delete f;
		MapPacket::removeMist(info->id, id);
	}
}

void Map::checkMists() {
	if (getPoisonMistCount() == 0) // Only player -> mob poison mists matter for checking, client does the rest
		return;
	for (unordered_map<int32_t, Mist *>::iterator miter = mists.begin(); miter != mists.end(); miter++) {
		Mist *mist = miter->second;
		if (!mist->isPoison())
			continue;
		for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
			Mob *mob = iter->second;
			if (mob == 0 || mob->hasStatus(StatusEffects::Mob::Poison))
				continue;
			if (GameLogicUtilities::isInBox(mist->getOrigin(), mist->getSkillLt(), mist->getSkillRb(), mob->getPos())) {
				Player *p = Players::Instance()->getPlayer(mist->getOwnerId());
				Mobs::handleMobStatus(p, mob, mist->getSkillId(), mist->getSkillLevel(), 0, 0);
			}
		}
	}
}
void Map::clearMists(bool showPacket) {
	unordered_map<int32_t, Mist *> mistlist = mists;
	for (unordered_map<int32_t, Mist *>::iterator iter = mistlist.begin(); iter != mistlist.end(); iter++) {
		removeMist(iter->first);
	}
}

int32_t Map::getPoisonMistCount() {
	return poisonmists;
}

// Timer stuff
void Map::runTimer() {
	clock_t time = TimeUtilities::getTickCount();
	checkReactorSpawn(time);
	checkMobSpawn(time);
	clearDrops(time);
	checkMists();
	if (TimeUtilities::getSecond() % 3 == 0) {
		checkShadowWeb();
	}
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
			getTimers(), Timer::Time::fromNow(t * 1000));
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
		if (player != players[i] && !players[i]->getActiveBuffs()->isUsingHide()) {
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
	// Mists
	for (unordered_map<int32_t, Mist *>::iterator iter = mists.begin(); iter != mists.end(); iter++) {
		if (iter->second != 0) {
			MapPacket::showMist(player, iter->second);
		}
	}

	if (player->getParty()) {
		player->getParty()->showHpBar(player);
		player->getParty()->receiveHpBar(player);
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

int32_t Map::checkTimer(uint32_t type, uint32_t id1, uint32_t id2) {
	return (getTimers()->checkTimer(Timer::Id(type, id1, id2)) / 1000);
}