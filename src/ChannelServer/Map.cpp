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
#include "Drop.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "GMPacket.h"
#include "Instance.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "MapleSession.h"
#include "MapleTVs.h"
#include "Maps.h"
#include "Mist.h"
#include "Mob.h"
#include "MobHandler.h"
#include "MobsPacket.h"
#include "NPCPacket.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "Players.h"
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

Map::Map(MapInfoPtr info) :
info(info),
spawnpoints(0),
objectids(1000),
instance(0),
timer(0),
timerstart(0),
poisonmists(0),
timemob(0),
webbed(0),
spawnmobs(-1),
timers(new Timer::Container)
{
	new Timer::Timer(bind(&Map::runTimer, this), // Due to dynamic loading, we can now simply start the map timer once the object is created
		Timer::Id(Timer::Types::MapTimer, info->id, 0),
		getTimers(), 0, 10000);
	if (info->timemob != 0) {
		new Timer::Timer(bind(&Map::timeMob, this, false),
			Timer::Id(Timer::Types::MapTimer, info->id, 1),
			getTimers(), Timer::Time::nthSecondOfHour(0), 60 * 60 * 1000); // Every hour, check for timeMob stuff

		// Players logging in need something else to be their first packet
		new Timer::Timer(bind(&Map::timeMob, this, true),
			Timer::Id(Timer::Types::MapTimer, info->id, 2),
			getTimers(), Timer::Time::fromNow(3 * 1000), 0);
	}
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
	else
		GmPacket::beginHide(player);
	if (timer > 0)
		MapPacket::showTimer(player, timer - static_cast<int32_t>(time(0) - timerstart));
	else if (instance != 0 && instance->showTimer() && instance->checkInstanceTimer() > 0)
		MapPacket::showTimer(player, instance->checkInstanceTimer());
}

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < this->players.size(); i++) {
		if (this->players[i] == player) {
			this->players.erase(this->players.begin() + i);
			break;
		}
	}
	Summons::removeSummon(player, true, false, false, 0);
	Summons::removeSummon(player, false, false, true, 0);
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

void Map::sendPlayersToTown(int32_t mobid, int16_t prop, int16_t count, const Pos &origin, const Pos &lt, const Pos &rb) {
	int16_t done = 0;
	string message = "";
	PortalInfo *p = 0;
	int32_t field = getInfo()->rm;
	if (BanishField *ban = SkillDataProvider::Instance()->getBanishData(mobid)) {
		field = ban->field;
		message = ban->message;
		if (ban->portal != "" && ban->portal != "sp") {
			p = Maps::getMap(field)->getPortal(ban->portal);
		}
	}
	for (size_t i = 0; i < players.size(); i++) {
		Player *toy = players[i];
		if (toy != 0) {
			if (GameLogicUtilities::isInBox(origin, lt, rb, toy->getPos()) && Randomizer::Instance()->randShort(99) < prop) {
				if (message != "") {
					PlayerPacket::showMessage(toy, message, 6);
				}
				toy->setMap(field, p);
				done++;
			}
		}
		if (count > 0 && done == count)
			break;
	}
}

void Map::buffPlayers(int32_t buffid) {
	for (size_t i = 0; i < players.size(); i++) {
		if (Player *toy = players[i]) {
			Inventory::useItem(toy, buffid);
		}
	}
}

// Reactors
void Map::addReactorSpawn(const ReactorSpawnInfo &spawn) {
	reactorspawns.push_back(spawn);
	Reactor *reactor = new Reactor(info->id, spawn.id, spawn.pos);
	ReactorPacket::spawnReactor(reactor);
}

void Map::addReactor(Reactor *reactor) {
	this->reactors.push_back(reactor);
	reactor->setId(this->reactors.size() - 1 + 200);
}

void Map::removeReactor(int32_t id) {
	reactorspawns[id].spawned = false;
	if (reactorspawns[id].time >= 0) {
		reactorspawns[id].spawnat = reactorspawns[id].time * 1000 + TimeUtilities::getTickCount();
	}
	else {
		reactorspawns[id].spawnat = -1;
	}
}

void Map::killReactors(bool showpacket) {
	Reactor *r;
	for (size_t i = 0; i < reactors.size(); i++) {
		r = reactors[i];
		if (r->isAlive()) {
			r->kill();
			if (showpacket) {
				ReactorPacket::destroyReactor(r);
			}
			removeReactor(r->getId() - 200); 
		}
	}
}

void Map::checkReactorSpawn(clock_t time, bool spawnAll) {
	for (size_t i = 0; i < reactorspawns.size(); i++) {
		clock_t spawnat = reactorspawns[i].spawnat;
		if (!reactorspawns[i].spawned && (spawnAll || (spawnat != -1 && time > spawnat))) {
			getReactor(i)->restore();
			reactorspawns[i].spawned = true;
		}
	}
}

// Footholds
Pos Map::findFloor(const Pos &pos) {
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

Pos Map::findRandomPos() {
	int16_t min_x = (info->lt.x != 0 ? info->lt.x : 0x8000);
	int16_t max_x = (info->rb.x != 0 ? info->rb.x : 0x7FFF);
	int16_t min_y = (info->lt.y != 0 ? info->lt.y : 0x8000);
	int16_t max_y = (info->rb.y != 0 ? info->rb.y : 0x7FFF);
	int16_t posx = 0;
	int16_t posy = 0;
	int16_t tempx = 0;
	int16_t tempy = 0;
	Pos pos(0, 0);
	Pos tpos;
	while (pos.x == 0 && pos.y == 0) {
		tempx = Randomizer::Instance()->randShort(max_x - min_x) + min_x;
		tempy = Randomizer::Instance()->randShort(max_y - min_y) + min_y;
		tpos.x = tempx;
		tpos.y = tempy;
		tpos = findFloor(tpos);
		if (tpos.y != tempy) {
			pos.x = tpos.x;
			pos.y = tpos.y;
		}
	}
	return pos;
}

int16_t Map::getFhAtPosition(const Pos &pos) {
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
void Map::addPortal(const PortalInfo &portal) {
	if (portal.name == "sp")
		spawnpoints[portal.id] = portal;
	else
		portals[portal.name] = portal;
}

PortalInfo * Map::getPortal(const string &name) {
	return portals.find(name) != portals.end() ? &portals[name] : 0;
}

PortalInfo * Map::getSpawnPoint(int8_t pid) {
	int8_t id = (pid != -1 ? pid : Randomizer::Instance()->randChar(spawnpoints.size() - 1));
	return &spawnpoints[id];
}

PortalInfo * Map::getNearestSpawnPoint(const Pos &pos) {
	int8_t id = 0;
	int32_t distance = 200000;
	for (unordered_map<int8_t, PortalInfo>::iterator i = spawnpoints.begin(); i != spawnpoints.end(); i++) {
		int32_t cmp = i->second.pos - pos;
		if (cmp < distance) {
			id = i->first;
			distance = cmp;
		}
	}
	return (id == 0 ? getSpawnPoint() : &spawnpoints[id]);
}

// NPCs
int32_t Map::addNPC(const NPCSpawnInfo &npc) {
	npcs.push_back(npc);
	NPCPacket::showNPC(getInfo()->id, npc, npcs.size() - 1);

	if (MapleTVs::Instance()->isMapleTVNPC(npc.id))
		MapleTVs::Instance()->addMap(this);

	return npcs.size() - 1;
}

void Map::removeNPC(int32_t index) {
	int32_t size = npcs.size();
	if (index >= 0 && index < size) {
		NPCSpawnInfo npc = npcs[index];
		NPCPacket::showNPC(getInfo()->id, npc, size - 1, false);
		npcs.erase(npcs.begin() + index);
	}
}

// Mobs
void Map::addMobSpawn(const MobSpawnInfo &spawn) {
	mobspawns.push_back(spawn);
	spawnMob(mobspawns.size() - 1, spawn);
}

void Map::checkMobSpawn(clock_t time, bool spawnAll) {
	// (Re-)spawn Mobs
	for (size_t i = 0; spawnmobs != 0 && i < mobspawns.size(); i++) {
		if (spawnmobs > 0 && mobspawns[i].id != spawnmobs) {
			continue;
		}
		int32_t spawnat = mobspawns[i].spawnat;
		if (!mobspawns[i].spawned && (spawnAll || (spawnat != -1 && spawnat < time))) {
			spawnMob(i, mobspawns[i]);
			mobspawns[i].spawned = true;
		}
	}
}

int32_t Map::spawnMob(int32_t mobid, const Pos &pos, int16_t fh, Mob *owner, int8_t summoneffect) {
	int32_t id = objectids.next();

	Mob *mob = new Mob(id, info->id, mobid, pos, fh);
	mobs[id] = mob;
	if (summoneffect != 0) {
		mob->setOwner(owner);
		owner->addSpawn(id, mob);
	}
	MobsPacket::spawnMob(0, mob, summoneffect, owner, (owner == 0));
	updateMobControl(mob, true);
	return id;
}

int32_t Map::spawnMob(int32_t spawnid, const MobSpawnInfo &info) {
	int32_t id = objectids.next();

	Mob *mob = new Mob(id, this->info->id, info.id, info.pos, spawnid, info.facesleft, info.fh);
	mobs[id] = mob;
	MobsPacket::spawnMob(0, mob, 0, 0, true);
	updateMobControl(mob, true);
	return id;
}

int32_t Map::spawnShell(int32_t mobid, const Pos &pos, int16_t fh) {
	int32_t id = objectids.next();

	Mob *mob = new Mob(id, info->id, mobid, pos, fh, 5);
	mobs[id] = mob;
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

void Map::updateMobControl(Mob *mob, bool spawn, Player *display) {
	int32_t maxpos = 200000;
	Player *p = 0;
	if (mob->getControlStatus() != Mobs::ControlStatus::ControlNone) {
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
	}
	mob->setControl(p, spawn, display);
}

void Map::removeMob(int32_t id, int32_t spawnid) {
	if (mobs.find(id) != mobs.end()) {
		if (spawnid > -1) {
			if (mobspawns[spawnid].time > -1) { // Add spawn point to respawns if mob was spawned by a spawn point.
				clock_t spawntime = TimeUtilities::getTickCount() + mobspawns[spawnid].time * 1000 * (Randomizer::Instance()->randInt(100) + 100) / 100; // Randomly spawn between 1x and 2x the spawn time
				mobspawns[spawnid].spawnat = spawntime;
			}
			mobspawns[spawnid].spawned = false;
		}
		this->mobs.erase(id);
		if (timemob == id) {
			timemob = 0;
		}
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

void Map::statusMobs(vector<StatusInfo> &statuses, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobmap = this->mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
		if (iter->second != 0 && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->addStatus(0, statuses);
		}
	}
}

void Map::checkShadowWeb() {
	int32_t wcount = getWebbedCount();
	if (wcount > 0) {
		unordered_map<int32_t, Mob *> mobmap = this->mobs;
		int32_t done = 0;
		for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
			if (iter->second != 0 && iter->second->hasStatus(StatusEffects::Mob::ShadowWeb)) {
				iter->second->applyWebDamage();
				done++;
			}
			if (done == wcount) {
				break;
			}
		}
	}
	else if (wcount < 0) { // ??
		setWebbedCount(0);
	}
}

void Map::spawnZakum(const Pos &pos, int16_t fh) {
	int32_t pid = 0;
	Mob *p = 0;
	Mob *body = getMob(spawnShell(Mobs::ZakumBody1, pos, fh));
	int32_t parts[8] = {
		Mobs::ZakumArm1, Mobs::ZakumArm2, Mobs::ZakumArm3,
		Mobs::ZakumArm4, Mobs::ZakumArm5, Mobs::ZakumArm6,
		Mobs::ZakumArm7, Mobs::ZakumArm8
	};
	for (int32_t g = 0; g < 8; g++) {
		pid = spawnMob(parts[g], pos, fh);
		p = getMob(pid);
		p->setOwner(body);
		body->addSpawn(pid, p);
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
	Mob *mob = 0;
	Mist *mist = 0;
	unordered_map<int32_t, Mist *>::iterator miter;

	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); ++iter) {
		mob = iter->second;
		if (mob == 0 || mob->hasStatus(StatusEffects::Mob::Poison) || mob->getHp() == 1)
			continue;
		for (miter = mists.begin(); miter != mists.end(); ++miter) {
			mist = miter->second;
			if (!mist->isPoison())
				continue;
			if (GameLogicUtilities::isInBox(mist->getOrigin(), mist->getSkillLt(), mist->getSkillRb(), mob->getPos())) {
				bool poisoned = (MobHandler::handleMobStatus(mist->getOwnerId(), mob, mist->getSkillId(), mist->getSkillLevel(), 0, 0) > 0);
				if (poisoned) // Mob is poisoned, don't need to check any more mists
					break;
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

void Map::timeMob(bool firstLoad) {
	int32_t chour = TimeUtilities::getHour();

	if (firstLoad && (chour >= info->starthour && chour < info->endhour)) {
		Pos p = findRandomPos();
		timemob = spawnMob(info->timemob, p, getFhAtPosition(p), 0, 0);
		showMessage(info->message, 6);
	}
	else {
		if (chour == info->starthour) {
			Pos p = findRandomPos();
			timemob = spawnMob(info->timemob, p, getFhAtPosition(p), 0, 0);
			showMessage(info->message, 6);
		}
		else if (chour == info->endhour && timemob != 0) {
			Mob *m = getMob(timemob);
			m->applyDamage(0, m->getHp());
		}
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
			if (iter->second->getControlStatus() == Mobs::ControlStatus::ControlNone) {
				updateMobControl(iter->second, true, player);
			}
			else {
				MobsPacket::spawnMob(player, iter->second, 0, 0, false, true);
				updateMobControl(iter->second);
			}
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

void Map::showMessage(const string &message, int8_t type) {
	for (size_t i = 0; i < players.size(); i++)
		PlayerPacket::showMessage(players[i], message, type);
}

bool Map::seatOccupied(int16_t id) {
	bool occupied = true;
	if (seats.find(id) != seats.end()) {
		occupied = (seats[id].occupant != 0);
	}
	return occupied;
}

void Map::playerSeated(int16_t id, Player *player) {
	if (seats.find(id) != seats.end()) {
		seats[id].occupant = player;
	}
}