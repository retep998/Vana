/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "EffectPacket.h"
#include "GameLogicUtilities.h"
#include "GmPacket.h"
#include "Instance.h"
#include "Inventory.h"
#include "MapPacket.h"
#include "MapleTvs.h"
#include "Maps.h"
#include "Mist.h"
#include "Mob.h"
#include "MobConstants.h"
#include "MobHandler.h"
#include "MobsPacket.h"
#include "NpcDataProvider.h"
#include "NpcPacket.h"
#include "PacketCreator.h"
#include "Party.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Randomizer.h"
#include "ReactorPacket.h"
#include "Reactor.h"
#include "Session.h"
#include "SummonHandler.h"
#include "TimeUtilities.h"
#include "Timer.h"
#include <ctime>
#include <functional>
#include <iostream>

using std::bind;

Map::Map(MapInfoPtr info, int32_t id) :
	m_info(info),
	m_id(id),
	m_objectIds(1000),
	m_instance(nullptr),
	m_timer(0),
	m_timerStart(0),
	m_timeMob(0),
	m_spawnMobs(-1),
	m_music(info->defaultMusic),
	m_timers(new Timer::Container)
{
	// Dynamic loading, start the map timer once the object is created
	new Timer::Timer(bind(&Map::runTimer, this),
		Timer::Id(Timer::Types::MapTimer, id, 0),
		getTimers(), 0, 10 * 1000);
}

// Map info
void Map::setMusic(const string &musicName) {
	m_music = (musicName == "default" ? getInfo()->defaultMusic : musicName);
	EffectPacket::playMusic(getId(), m_music);
}

void Map::setMobSpawning(int32_t spawn) {
	m_spawnMobs = spawn;
}

uint32_t Map::makeNpcId(uint32_t receivedId) {
	return (receivedId - NpcStart);
}

uint32_t Map::makeNpcId() {
	return (m_npcSpawns.size() - 1 + NpcStart);
}

uint32_t Map::makeReactorId(uint32_t receivedId) {
	return (receivedId - ReactorStart);
}

uint32_t Map::makeReactorId() {
	return (m_reactorSpawns.size() - 1 + ReactorStart);
}

// Data initialization
void Map::addFoothold(const FootholdInfo &foothold) {
	m_footholds.push_back(foothold);
}

void Map::addSeat(int16_t id, const SeatInfo &seat) {
	m_seats[id] = seat;
}

int32_t Map::addNpc(const NpcSpawnInfo &npc) {
	m_npcSpawns.push_back(npc);
	NpcPacket::showNpc(getId(), npc, makeNpcId());

	if (NpcDataProvider::Instance()->isMapleTv(npc.id)) {
		MapleTvs::Instance()->addMap(this);
	}

	return m_npcSpawns.size() - 1;
}

void Map::addReactorSpawn(const ReactorSpawnInfo &spawn) {
	m_reactorSpawns.push_back(spawn);
	m_reactorSpawns[m_reactorSpawns.size() - 1].spawned = true;
	Reactor *reactor = new Reactor(getId(), spawn.id, spawn.pos);
	ReactorPacket::spawnReactor(reactor);
}

void Map::addMobSpawn(const MobSpawnInfo &spawn) {
	m_mobSpawns.push_back(spawn);
	m_mobSpawns[m_mobSpawns.size() - 1].spawned = true;
	spawnMob(m_mobSpawns.size() - 1, spawn);
}

void Map::addPortal(const PortalInfo &portal) {
	if (portal.name == "sp") {
		m_spawnPoints[portal.id] = portal;
	}
	else if (portal.name == "tp") {
		// Mystic Door portals
	}
	else {
		m_portals[portal.name] = portal;
	}
}

void Map::addTimeMob(TimeMobPtr info) {
	new Timer::Timer(bind(&Map::timeMob, this, false),
		Timer::Id(Timer::Types::MapTimer, getId(), 1),
		getTimers(), TimeUtilities::nthSecondOfHour(0), 60 * 60 * 1000); // Check once per hour

	new Timer::Timer(bind(&Map::timeMob, this, true),
		Timer::Id(Timer::Types::MapTimer, getId(), 2),
		getTimers(), TimeUtilities::fromNow(3 * 1000), 0); // First check

	m_timeMobInfo = info;
}

// Players
void Map::addPlayer(Player *player) {
	m_players.push_back(player);
	if (forceMapEquip()) {
		MapPacket::forceMapEquip(player);
	}
	if (!player->getActiveBuffs()->isUsingHide()) {
		MapPacket::showPlayer(player);
	}
	else {
		GmPacket::beginHide(player);
	}
	if (m_timer > 0) {
		MapPacket::showTimer(player, m_timer - static_cast<int32_t>(time(nullptr) - m_timerStart));
	}
	else if (Instance *i = getInstance()) {
		if (i->showTimer() && i->checkInstanceTimer() > 0) {
			MapPacket::showTimer(player, i->checkInstanceTimer());
		}
	}
	if (m_ship) {
		// Boat packet, need to change this section slightly...
	}
}

void Map::boatDock(bool isDocked) {
	m_ship = isDocked;
	PacketCreator packet;
	// Fill boat packet
	sendPacket(packet);
}

size_t Map::getNumPlayers() const {
	return m_players.size();
}

Player * Map::getPlayer(uint32_t index) const {
	return m_players[index];
}

string Map::getPlayerNames() {
	std::ostringstream names;
	for (size_t i = 0; i < m_players.size(); i++) {
		if (Player *player = m_players[i]) {
			names << player->getName() << " ";
		}
	}
	return names.str();
}

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players[i] == player) {
			m_players.erase(m_players.begin() + i);
			break;
		}
	}
	SummonHandler::removeSummon(player, true, false, SummonMessages::None);
	SummonHandler::removeSummon(player, false, true, SummonMessages::None);
	MapPacket::removePlayer(player);
	updateMobControl(player);
}

void Map::runFunctionPlayers(const Pos &origin, const Pos &lt, const Pos &rb, int16_t prop, function<void (Player *)> successFunc) {
	runFunctionPlayers(origin, lt, rb, prop, 0, successFunc);
}

void Map::runFunctionPlayers(const Pos &origin, const Pos &lt, const Pos &rb, int16_t prop, int16_t count, function<void (Player *)> successFunc) {
	int16_t done = 0;
	for (size_t i = 0; i < m_players.size(); ++i) {
		if (Player *toy = m_players[i]) {
			if (GameLogicUtilities::isInBox(origin, lt, rb, toy->getPos()) && Randomizer::Instance()->randShort(99) < prop) {
				successFunc(toy);
				done++;
			}
		}
		if (count > 0 && done == count) {
			break;
		}
	}
}

void Map::runFunctionPlayers(function<void (Player *)> successFunc) {
	for (size_t i = 0; i < m_players.size(); ++i) {
		if (Player *toy = m_players[i]) {
			successFunc(toy);
		}
	}
}

void Map::buffPlayers(int32_t buffId) {
	for (size_t i = 0; i < m_players.size(); ++i) {
		if (Player *toy = m_players[i]) {
			if (toy->getStats()->getHp() > 0) {
				Inventory::useItem(toy, buffId);
				EffectPacket::sendMobItemBuffEffect(toy, buffId);
			}
		}
	}
}

// Reactors
void Map::addReactor(Reactor *reactor) {
	m_reactors.push_back(reactor);
	reactor->setId(m_reactors.size() - 1 + Map::ReactorStart);
}

Reactor * Map::getReactor(uint32_t id) const {
	return (id < m_reactors.size() ? m_reactors[id] : nullptr);
}

size_t Map::getNumReactors() const {
	return m_reactors.size();
}

void Map::removeReactor(uint32_t id) {
	ReactorSpawnInfo *info = &m_reactorSpawns[id];
	if (info->time >= 0) {
		// We don't want to respawn -1s, leave that to some script
		clock_t reactorRespawn = info->time * 1000 + TimeUtilities::getTickCount();
		m_reactorRespawns.push_back(Respawnable(id, reactorRespawn));
	}
}

void Map::killReactors(bool showPacket) {
	Reactor *r;
	for (size_t i = 0; i < m_reactors.size(); i++) {
		r = m_reactors[i];
		if (r->isAlive()) {
			r->kill();
			if (showPacket) {
				ReactorPacket::destroyReactor(r);
			}

		}
	}
}

// Footholds
Pos Map::findFloor(const Pos &pos) {
	// Determines where a drop falls using the footholds data
	// to check the platforms and find the correct one.
	int16_t x = pos.x;
	int16_t y = pos.y - 100;
	int16_t yMax = pos.y;
	bool firstCheck = true;
	for (size_t i = 0; i < m_footholds.size(); i++) {
		Pos &pos1 = m_footholds[i].pos1;
		Pos &pos2 = m_footholds[i].pos2;
		if ((x > pos1.x && x <= pos2.x) || (x > pos2.x && x <= pos1.x)) {
			int16_t cMax = (int16_t)((float)(pos1.y - pos2.y) / (pos1.x - pos2.x) * (x - pos1.x) + pos1.y);
			if ((cMax <= yMax || (yMax == pos.y && firstCheck)) && cMax >= y) {
				yMax = cMax;
				firstCheck = false;
			}
		}
	}
	return Pos(x, yMax);
}

Pos Map::findRandomPos() {
	const Pos &lt = getMapLeftTop();
	const Pos &rb = getMapRightBottom();
	int16_t xMin = (lt.x != 0 ? lt.x : 0x8000);
	int16_t xMax = (rb.x != 0 ? rb.x : 0x7FFF);
	int16_t yMin = (lt.y != 0 ? lt.y : 0x8000);
	int16_t yMax = (rb.y != 0 ? rb.y : 0x7FFF);
	int16_t xPos = 0;
	int16_t yPos = 0;
	int16_t xTemp = 0;
	int16_t yTemp = 0;
	Pos pos(0, 0);
	Pos tPos;
	while (pos.x == 0 && pos.y == 0) {
		xTemp = Randomizer::Instance()->randShort(xMax, xMin);
		yTemp = Randomizer::Instance()->randShort(yMax, yMin);
		tPos.x = xTemp;
		tPos.y = yTemp;
		tPos = findFloor(tPos);
		if (tPos.y != yTemp) {
			pos.x = tPos.x;
			pos.y = tPos.y;
		}
	}
	return pos;
}

int16_t Map::getFhAtPosition(const Pos &pos) {
	int16_t foothold = 0;
	for (size_t i = 0; i < m_footholds.size(); i++) {
		FootholdInfo &cur = m_footholds[i];
		if (((pos.x > cur.pos1.x && pos.x <= cur.pos2.x) || (pos.x > cur.pos2.x && pos.x <= cur.pos1.x)) && ((pos.y > cur.pos1.y && pos.y <= cur.pos2.y) || (pos.y > cur.pos2.y && pos.y <= cur.pos1.y))) {
			foothold = cur.id;
			break;
		}
	}
	return foothold;
}

// Portals
PortalInfo * Map::getPortal(const string &name) {
	return m_portals.find(name) != m_portals.end() ? &m_portals[name] : nullptr;
}

PortalInfo * Map::getSpawnPoint(int8_t portalId) {
	int8_t id = (portalId != -1 ? portalId : Randomizer::Instance()->randChar(m_spawnPoints.size() - 1));
	return &m_spawnPoints[id];
}

PortalInfo * Map::getNearestSpawnPoint(const Pos &pos) {
	int8_t id = -1;
	int32_t distance = 200000;
	for (unordered_map<int8_t, PortalInfo>::iterator i = m_spawnPoints.begin(); i != m_spawnPoints.end(); ++i) {
		int32_t cmp = i->second.pos - pos;
		if (cmp < distance) {
			id = i->first;
			distance = cmp;
		}
	}
	return getSpawnPoint(id);
}

// NPCs
void Map::removeNpc(uint32_t index) {
	if (isValidNpcIndex(index)) {
		NpcSpawnInfo npc = m_npcSpawns[index];
		NpcPacket::showNpc(getId(), npc, makeNpcId(), false);
		m_npcSpawns.erase(m_npcSpawns.begin() + index);
	}
}

bool Map::isValidNpcIndex(uint32_t id) const {
	return (id < m_npcSpawns.size());
}

NpcSpawnInfo Map::getNpc(uint32_t id) const {
	return m_npcSpawns[id];
}

// Mobs
int32_t Map::spawnMob(int32_t mobId, const Pos &pos, int16_t fh, Mob *owner, int8_t summonEffect) {
	int32_t id = getObjectId();

	Mob *mob = new Mob(id, getId(), mobId, pos, fh);
	m_mobs[id] = mob;
	if (summonEffect != 0) {
		mob->setOwner(owner);
		owner->addSpawn(id, mob);
	}
	MobsPacket::spawnMob(nullptr, mob, summonEffect, owner, (owner == nullptr));
	updateMobControl(mob, true);
	return id;
}

int32_t Map::spawnMob(int32_t spawnId, const MobSpawnInfo &info) {
	int32_t id = getObjectId();

	Mob *mob = new Mob(id, getId(), info.id, info.pos, spawnId, info.facesRight, info.foothold);
	m_mobs[id] = mob;
	MobsPacket::spawnMob(nullptr, mob, 0, nullptr, true);
	updateMobControl(mob, true);
	return id;
}

int32_t Map::spawnShell(int32_t mobId, const Pos &pos, int16_t fh) {
	int32_t id = getObjectId();

	Mob *mob = new Mob(id, getId(), mobId, pos, fh, Mobs::ControlStatus::ControlNone);
	m_mobs[id] = mob;
	updateMobControl(mob, true);
	return id;
}

Mob * Map::getMob(int32_t id, bool isMapId) {
	if (isMapId) {
		return (m_mobs.find(id) != m_mobs.end() ? m_mobs[id] : nullptr);
	}
	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); ++iter) {
		if (iter->second != nullptr && iter->second->getMobId() == id) {
			return iter->second;
		}
	}
	return nullptr;
}

void Map::updateMobControl(Player *player) {
	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); ++iter) {
		if (iter->second != nullptr && iter->second->getControl() == player) {
			updateMobControl(iter->second);
		}
	}
}

void Map::updateMobControl(Mob *mob, bool spawn, Player *display) {
	int32_t maxPos = 200000;
	Player *p = nullptr;
	if (mob->getControlStatus() != Mobs::ControlStatus::ControlNone) {
		for (size_t j = 0; j < m_players.size(); ++j) {
			Player *test = m_players[j];
			if (!(test->getActiveBuffs()->isUsingHide())) {
				int32_t curPos = mob->getPos() - test->getPos();
				if (curPos < maxPos) {
					maxPos = curPos;
					p = test;
					break;
				}
			}
		}
	}
	mob->setControl(p, spawn, display);
}

void Map::removeMob(int32_t id, int32_t spawnId) {
	if (m_mobs.find(id) != m_mobs.end()) {
		if (spawnId >= 0 && m_mobSpawns[spawnId].time != -1) {
			// Add spawn point to respawns if mob was spawned by a spawn point.
			clock_t spawnTime = m_mobSpawns[spawnId].time * 1000 * (Randomizer::Instance()->randInt(200, 100)) / 100;
			// Randomly spawn between 1x and 2x the spawn time
			spawnTime += TimeUtilities::getTickCount();
			m_mobRespawns.push_back(Respawnable(spawnId, spawnTime));
			m_mobSpawns[spawnId].spawned = false;
		}
		m_mobs.erase(id);
		if (m_timeMob == id) {
			m_timeMob = 0;
		}
	}
}

int32_t Map::killMobs(Player *player, int32_t mobId, bool playerkill, bool showPacket) {
	unordered_map<int32_t, Mob *> mobs = m_mobs;
	int32_t mobSkilled = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); ++iter) {
		if (iter->second != nullptr) {
			if ((mobId > 0 && iter->second->getMobId() == mobId) || mobId == 0) {
				if (playerkill && player != nullptr) {
					if (!iter->second->isSponge()) {
						// Sponges will be taken care of by their parts
						iter->second->applyDamage(player->getId(), iter->second->getHp());
					}
				}
				else {
					iter->second->die(showPacket);
				}
				mobSkilled++;
			}
		}
	}
	return mobSkilled;
}

int32_t Map::countMobs(int32_t mobId) {
	unordered_map<int32_t, Mob *> mobs = m_mobs;
	int32_t mobCount = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); ++iter) {
		if (iter->second != nullptr) {
			if ((mobId > 0 && iter->second->getMobId() == mobId) || mobId == 0) {
				mobCount++;
			}
		}
	}
	return mobCount;
}

void Map::healMobs(int32_t hp, int32_t mp, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobMap = m_mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobMap.begin(); iter != mobMap.end(); ++iter) {
		if (iter->second != nullptr && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->skillHeal(hp, mp);
		}
	}
}

void Map::statusMobs(vector<StatusInfo> &statuses, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobMap = m_mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobMap.begin(); iter != mobMap.end(); ++iter) {
		if (iter->second != nullptr && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->addStatus(0, statuses);
		}
	}
}

void Map::spawnZakum(const Pos &pos, int16_t fh) {
	int32_t partId = 0;
	Mob *p = nullptr;
	Mob *body = getMob(spawnShell(Mobs::ZakumBody1, pos, fh));
	int32_t parts[8] = {
		Mobs::ZakumArm1, Mobs::ZakumArm2, Mobs::ZakumArm3,
		Mobs::ZakumArm4, Mobs::ZakumArm5, Mobs::ZakumArm6,
		Mobs::ZakumArm7, Mobs::ZakumArm8
	};
	for (int32_t g = 0; g < 8; g++) {
		partId = spawnMob(parts[g], pos, fh);
		p = getMob(partId);
		p->setOwner(body);
		body->addSpawn(partId, p);
	}
}

void Map::addWebbedMob(Mob *mob) {
	m_webbed[mob->getId()] = mob;
}

void Map::removeWebbedMob(int32_t id) {
	m_webbed.erase(id);
}

unordered_map<int32_t, Mob *> Map::getMobs() const {
	return m_mobs;
}

// Drops
void Map::addDrop(Drop *drop) {
	boost::recursive_mutex::scoped_lock l(m_dropsMutex);
	int32_t id = getObjectId();
	drop->setId(id);
	drop->setPos(findFloor(drop->getPos()));
	m_drops[id] = drop;
}

void Map::removeDrop(int32_t id) {
	boost::recursive_mutex::scoped_lock l(m_dropsMutex);
	if (m_drops.find(id) != m_drops.end()) {
		m_drops.erase(id);
	}
}

Drop * Map::getDrop(int32_t id) {
	boost::recursive_mutex::scoped_lock l(m_dropsMutex);
	return (m_drops.find(id) != m_drops.end() ? m_drops[id] : nullptr);
}

void Map::clearDrops(bool showPacket) {
	boost::recursive_mutex::scoped_lock l(m_dropsMutex);
	unordered_map<int32_t, Drop *> drops = m_drops;
	for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); ++iter) {
		iter->second->removeDrop(showPacket);
	}
}

// Seats
bool Map::seatOccupied(int16_t id) {
	bool occupied = true;
	if (m_seats.find(id) != m_seats.end()) {
		occupied = (m_seats[id].occupant != nullptr);
	}
	return occupied;
}

void Map::playerSeated(int16_t id, Player *player) {
	if (m_seats.find(id) != m_seats.end()) {
		m_seats[id].occupant = player;
	}
}

// Mists
void Map::addMist(Mist *mist) {
	mist->setId(getMistId());

	if (mist->isPoison() && !mist->isMobMist()) {
		m_poisonMists[mist->getId()] = mist;
	}
	else {
		m_mists[mist->getId()] = mist;
	}

	new Timer::Timer(bind(&Map::removeMist, this, mist),
		Timer::Id(Timer::Types::MistTimer, mist->getId(), 0),
		getTimers(), TimeUtilities::fromNow(mist->getTime() * 1000));

	MapPacket::spawnMist(getId(), mist);
}

Mist * Map::getMist(int32_t id) {
	if (m_mists.find(id) != m_mists.end()) {
		return m_mists[id];
	}
	return (m_poisonMists.find(id) != m_poisonMists.end() ? m_poisonMists[id] : nullptr);
}

void Map::removeMist(Mist *mist) {
	int32_t id = mist->getId();
	if (mist->isPoison() && !mist->isMobMist()) {
		m_poisonMists.erase(id);
	}
	else {
		m_mists.erase(id);
	}
	delete mist;
	MapPacket::removeMist(getId(), id);
}

void Map::clearMists(bool showPacket) {
	unordered_map<int32_t, Mist *>::iterator iter;
	unordered_map<int32_t, Mist *> mistlist = m_mists;
	for (iter = mistlist.begin(); iter != mistlist.end(); ++iter) {
		removeMist(iter->second);
	}
	mistlist = m_poisonMists;
	for (iter = mistlist.begin(); iter != mistlist.end(); ++iter) {
		removeMist(iter->second);
	}
}

// Timer stuff
void Map::respawn(int8_t types) {
	if (types & SpawnTypes::Mob) {
		m_mobRespawns.clear();
		MobSpawnInfo *info;
		for (size_t i = 0; i < m_mobSpawns.size(); ++i) {
			info = &m_mobSpawns[i];
			if (!info->spawned) {
				info->spawned = true;
				spawnMob(i, *info);
			}
		}
	}
	if (types & SpawnTypes::Reactor) {
		m_reactorRespawns.clear();
		Reactor *cur;
		for (size_t i = 0; i < m_reactors.size(); ++i) {
			cur = m_reactors[i];
			if (!cur->isAlive()) {
				m_reactorSpawns[i].spawned = true;
				cur->restore();
			}
		}
	}
}

void Map::checkSpawn(clock_t time) {
	Respawnable *respawn;

	for (size_t i = 0; i < m_mobRespawns.size(); ++i) {
		respawn = &m_mobRespawns[i];
		if (time > respawn->spawnAt) {
			m_mobSpawns[respawn->spawnId].spawned = true;
			spawnMob(respawn->spawnId, m_mobSpawns[respawn->spawnId]);

			m_mobRespawns.erase(m_mobRespawns.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < m_reactorRespawns.size(); ++i) {
		respawn = &m_reactorRespawns[i];
		if (time > respawn->spawnAt) {
			m_reactorSpawns[respawn->spawnId].spawned = true;
			getReactor(respawn->spawnId)->restore();

			m_reactorRespawns.erase(m_reactorRespawns.begin() + i);
			i--;
		}
	}
}

void Map::checkShadowWeb() {
	if (m_webbed.size() > 0) {
		for (unordered_map<int32_t, Mob *>::iterator iter = m_webbed.begin(); iter != m_webbed.end(); ++iter) {
			iter->second->applyWebDamage();
		}
	}
}

void Map::checkMists() {
	if (m_poisonMists.size() == 0) {
		return;
	}

	Mob *mob = nullptr;
	Mist *mist = nullptr;
	unordered_map<int32_t, Mist *>::iterator miter;

	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); ++iter) {
		mob = iter->second;
		if (mob == nullptr || mob->hasStatus(StatusEffects::Mob::Poison) || mob->getHp() == 1) {
			continue;
		}
		for (miter = m_poisonMists.begin(); miter != m_poisonMists.end(); ++miter) {
			mist = miter->second;
			if (GameLogicUtilities::isInBox(mist->getOrigin(), mist->getSkillLt(), mist->getSkillRb(), mob->getPos())) {
				bool poisoned = (MobHandler::handleMobStatus(mist->getOwnerId(), mob, mist->getSkillId(), mist->getSkillLevel(), 0, 0) > 0);
				if (poisoned) {
					// Mob is poisoned, don't need to check any more mists
					break;
				}
			}
		}
	}
}

void Map::clearDrops(clock_t time) {
	// Clear drops based on how long they have been in the map
	boost::recursive_mutex::scoped_lock l(m_dropsMutex);
	time -= 180000; // Drops disappear after 3 minutes
	unordered_map<int32_t, Drop *> drops = m_drops;
	for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); ++iter) {
		if (iter->second != nullptr) {
			if (iter->second->getDropped() < time) {
				iter->second->removeDrop();
			}
		}
	}
}

void Map::runTimer() {
	clock_t time = TimeUtilities::getTickCount();
	checkSpawn(time);
	clearDrops(time);
	checkMists();
	if (TimeUtilities::getSecond() % 3 == 0) {
		checkShadowWeb();
	}
}

void Map::timeMob(bool firstLoad) {
	int32_t cHour = TimeUtilities::getHour(false);
	TimeMob *tm = getTimeMob();
	if (firstLoad) {
		if (cHour >= tm->startHour && cHour < tm->endHour) {
			Pos &p = findRandomPos();
			m_timeMob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0);
			showMessage(tm->message, PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		if (cHour == tm->startHour) {
			Pos &p = findRandomPos();
			m_timeMob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0);
			showMessage(tm->message, PlayerPacket::NoticeTypes::Blue);
		}
		else if (cHour == tm->endHour && m_timeMob != 0) {
			Mob *m = getMob(m_timeMob);
			m->applyDamage(0, m->getHp());
		}
	}
}

void Map::setMapTimer(int32_t t) {
	if (t > 0 && m_timer != 0) {
		return;
	}
	m_timer = t;
	m_timerStart = time(nullptr);
	MapPacket::showTimer(getId(), t);
	if (t > 0) {
		new Timer::Timer(bind(&Map::setMapTimer, this, 0),
			Timer::Id(Timer::Types::MapTimer, getId(), 25),
			getTimers(), TimeUtilities::fromNow(t * 1000));
	}
}

void Map::showObjects(Player *player) {
	// Music
	if (getMusic() != getDefaultMusic())
		EffectPacket::playMusic(player, getMusic());

	// MapleTV messengers
	if (MapleTvs::Instance()->isMapleTvMap(getId()) && MapleTvs::Instance()->hasMessage()) {
		PacketCreator packet;
		MapleTvs::Instance()->getMapleTvEntryPacket(packet);
		player->getSession()->send(packet);
	}

	// Players
	for (size_t i = 0; i < m_players.size(); i++) {
		if (player != m_players[i] && !m_players[i]->getActiveBuffs()->isUsingHide()) {
			PacketCreator packet = MapPacket::playerPacket(m_players[i]);
			player->getSession()->send(packet);
			SummonHandler::showSummons(m_players[i], player);
			// Bug in global; would be fixed here:
			// Hurricane/Pierce do not display properly if using when someone enters the map
			// Berserk does not display properly either - players[i]->getActiveBuffs()->getBerserk()
		}
	}

	// NPCs
	for (size_t i = 0; i < m_npcSpawns.size(); i++) {
		NpcPacket::showNpc(player, m_npcSpawns[i], i + Map::NpcStart);
	}

	// Reactors
	for (size_t i = 0; i < m_reactors.size(); i++) {
		if (m_reactors[i]->isAlive()) {
			ReactorPacket::showReactor(player, m_reactors[i]);
		}
	}

	// Mobs
	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); ++iter) {
		if (iter->second != nullptr) {
			if (iter->second->getControlStatus() == Mobs::ControlStatus::ControlNone) {
				updateMobControl(iter->second, true, player);
			}
			else {
				MobsPacket::spawnMob(player, iter->second, 0, nullptr, false, true);
				updateMobControl(iter->second);
			}
		}
	}

	// Drops
	{
		boost::recursive_mutex::scoped_lock l(m_dropsMutex);
		for (unordered_map<int32_t, Drop *>::iterator iter = m_drops.begin(); iter != m_drops.end(); ++iter) {
			if (iter->second != nullptr) {
				iter->second->showDrop(player);
			}
		}
	}

	// Mists
	for (unordered_map<int32_t, Mist *>::iterator iter = m_mists.begin(); iter != m_mists.end(); ++iter) {
		if (iter->second != nullptr) {
			MapPacket::showMist(player, iter->second);
		}
	}

	if (Party *party = player->getParty()) {
		party->showHpBar(player);
		party->receiveHpBar(player);
	}

	if (hasClock()) {
		time_t rawTime;
		time(&rawTime);
		struct tm *timeInfo = localtime(&rawTime);
		MapPacket::showClock(player, timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
	}
}

void Map::sendPacket(PacketCreator &packet, Player *player) {
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players[i] != player) {
			m_players[i]->getSession()->send(packet);
		}
	}
}

void Map::showMessage(const string &message, int8_t type) {
	for (size_t i = 0; i < m_players.size(); i++) {
		PlayerPacket::showMessage(m_players[i], message, type);
	}
}

bool Map::createWeather(Player *player, bool adminWeather, int32_t time, int32_t itemId, const string &message) {
	Timer::Id timerId(Timer::Types::WeatherTimer, 0, 0); // Just to check if there's already a weather item running and adding a new one
	if (getTimers()->checkTimer(timerId) != 0) {
		// Hacking
		return false;
	}

	MapPacket::changeWeather(getId(), adminWeather, itemId, message);
	new Timer::Timer(bind(&MapPacket::changeWeather, getId(), adminWeather, 0, ""),
		timerId, getTimers(), TimeUtilities::fromNow(time * 1000));
	return true;
}