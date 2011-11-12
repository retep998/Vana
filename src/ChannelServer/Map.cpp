/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "InventoryPacket.h"
#include "Kite.h"
#include "MapPacket.h"
#include "MapleSession.h"
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
#include "PlayerDataProvider.h"
#include "PlayerNpcDataProvider.h"
#include "PlayerPacket.h"
#include "Randomizer.h"
#include "ReactorPacket.h"
#include "Reactor.h"
#include "Summons.h"
#include "Timer/Time.h"
#include "Timer/Timer.h"
#include <ctime>
#include <functional>
#include <utility>

using std::make_pair;
using std::tr1::bind;

Map::Map(MapInfoPtr info, int32_t id) :
m_info(info),
m_id(id),
m_objectids(1000),
m_instance(nullptr),
m_timer(0),
m_timerstart(0),
m_timemob(0),
m_spawnmobs(-1),
m_music(info->defaultMusic),
m_timers(new Timer::Container),
m_weatherItemid(0),
m_jukeboxItemid(0)
{
	// Dynamic loading, start the map timer once the object is created
	new Timer::Timer(bind(&Map::runTimer, this),
		Timer::Id(Timer::Types::MapTimer, id, 0),
		getTimers(), 0, 10 * 1000);
}

// Map info
void Map::setMusic(const string &musicname) {
	m_music = (musicname == "default" ? getInfo()->defaultMusic : musicname);
	EffectPacket::playMusic(getId(), m_music);
}

void Map::setMobSpawning(int32_t spawn) {
	m_spawnmobs = spawn;
}

uint32_t Map::makeNpcId(uint32_t receivedId) {
	return (receivedId - NpcStart);
}

uint32_t Map::makeNpcId() {
	return (m_npc_spawns.size() - 1 + NpcStart);
}

uint32_t Map::makeReactorId(uint32_t receivedId) {
	return (receivedId - ReactorStart);
}

uint32_t Map::makeReactorId() {
	return (m_reactor_spawns.size() - 1 + ReactorStart);
}

// Data initialization
void Map::addFoothold(const FootholdInfo &foothold) {
	m_footholds.push_back(foothold);
}

void Map::addSeat(int16_t id, const SeatInfo &seat) {
	m_seats[id] = seat;
}

int32_t Map::addNpc(const NpcSpawnInfo &npc) {
	m_npc_spawns.push_back(npc);
	NpcPacket::showNpc(getId(), npc, makeNpcId());

	if (NpcDataProvider::Instance()->isMapleTv(npc.id))
		MapleTvs::Instance()->addMap(this);

	return m_npc_spawns.size() - 1;
}

void Map::addReactorSpawn(const ReactorSpawnInfo &spawn) {
	m_reactor_spawns.push_back(spawn);
	m_reactor_spawns[m_reactor_spawns.size() - 1].spawned = true;
	Reactor *reactor = new Reactor(getId(), spawn.id, spawn.pos);
	ReactorPacket::spawnReactor(reactor);
}

void Map::addMobSpawn(const MobSpawnInfo &spawn) {
	m_mob_spawns.push_back(spawn);
	m_mob_spawns[m_mob_spawns.size() - 1].spawned = true;
	spawnMob(m_mob_spawns.size() - 1, spawn);
}

void Map::addPortal(const PortalInfo &portal) {
	if (portal.name == "sp") {
		m_spawn_points[portal.id] = portal;
	}
	else if (portal.name == "tp") {
		m_door_points.push_back(make_pair(portal, false));
	}
	else {
		m_portals[portal.name] = portal;
	}
}

void Map::addTimeMob(TimeMobPtr info) {
	new Timer::Timer(bind(&Map::timeMob, this, false),
		Timer::Id(Timer::Types::MapTimer, getId(), 1),
		getTimers(), Timer::Time::nthSecondOfHour(0), 60 * 60 * 1000); // Check once per hour

	new Timer::Timer(bind(&Map::timeMob, this, true),
		Timer::Id(Timer::Types::MapTimer, getId(), 2),
		getTimers(), Timer::Time::fromNow(3 * 1000), 0); // First check

	m_time_mob = info;
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
		MapPacket::showTimer(player, m_timer - static_cast<int32_t>(time(0) - m_timerstart));
	}
	else if (Instance *i = getInstance()) {
		if (i->showTimer() && i->checkInstanceTimer() > 0) {
			MapPacket::showTimer(player, m_instance->checkInstanceTimer());
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
	string names = "";
	for (size_t i = 0; i < m_players.size(); i++) {
		if (Player *player = m_players[i]) {
			names += player->getName() + " ";
		}
	}
	return names;
}

void Map::removePlayer(Player *player) {
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players[i] == player) {
			m_players.erase(m_players.begin() + i);
			break;
		}
	}
	Summons::removeSummon(player, true, false, SummonMessages::None);
	Summons::removeSummon(player, false, true, SummonMessages::None);
	MapPacket::removePlayer(player);
	updateMobControl(player);

	removeKitesPlayer(player->getId());
}

void Map::runFunctionPlayers(function<void (Player *)> successFunc, const Pos &origin, const Pos &lt, const Pos &rb, int16_t prop, int16_t count) {
	int16_t done = 0;
	for (size_t i = 0; i < m_players.size(); i++) {
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
	for (size_t i = 0; i < m_players.size(); i++) {
		if (Player *toy = m_players[i]) {
			successFunc(toy);
		}
	}
}

void Map::buffPlayers(int32_t buffid) {
	for (size_t i = 0; i < m_players.size(); i++) {
		if (Player *toy = m_players[i]) {
			if (toy->getStats()->getHp() > 0) {
				Inventory::useItem(toy, buffid);
				EffectPacket::sendMobItemBuffEffect(toy, buffid);
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
	ReactorSpawnInfo *info = &m_reactor_spawns[id];
	if (info->time >= 0) {
		// We don't want to respawn -1s, leave that to some script
		clock_t reactorRespawn = info->time * 1000 + TimeUtilities::getTickCount();
		m_reactor_respawns.push_back(Respawnable(id, reactorRespawn));
	}
}

void Map::killReactors(bool showpacket) {
	Reactor *r;
	for (size_t i = 0; i < m_reactors.size(); i++) {
		r = m_reactors[i];
		if (r->isAlive()) {
			r->kill();
			if (showpacket) {
				ReactorPacket::destroyReactor(r);
			}
			removeReactor(makeReactorId(r->getId()));
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
	for (size_t i = 0; i < m_footholds.size(); i++) {
		FootholdInfo &fh = m_footholds[i];
		if ((x > fh.pos1.x && x <= fh.pos2.x) || (x > fh.pos2.x && x <= fh.pos1.x)) {
			int16_t cmax = (int16_t)((float)(fh.pos1.y - fh.pos2.y) / (fh.pos1.x - fh.pos2.x) * (x - fh.pos1.x) + fh.pos1.y);
			if ((cmax <= maxy || (maxy == pos.y && firstcheck)) && cmax >= y) {
				maxy = cmax;
				firstcheck = false;
			}
		}
	}
	return Pos(x, maxy);
}

Pos Map::findRandomPos() {
	const Pos &lt = getMapLeftTop();
	const Pos &rb = getMapRightBottom();
	int16_t min_x = (lt.x != 0 ? lt.x : 0x8000);
	int16_t max_x = (rb.x != 0 ? rb.x : 0x7FFF);
	int16_t min_y = (lt.y != 0 ? lt.y : 0x8000);
	int16_t max_y = (rb.y != 0 ? rb.y : 0x7FFF);
	int16_t posx = 0;
	int16_t posy = 0;
	int16_t tempx = 0;
	int16_t tempy = 0;
	Pos pos(0, 0);
	Pos tpos;
	while (pos.x == 0 && pos.y == 0) {
		tempx = Randomizer::Instance()->randShort(max_x, min_x);
		tempy = Randomizer::Instance()->randShort(max_y, min_y);
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

PortalInfo * Map::getSpawnPoint(int8_t pid) {
	int8_t id = (pid != -1 ? pid : Randomizer::Instance()->randChar(m_spawn_points.size() - 1));
	return &m_spawn_points[id];
}

pair<int32_t, PortalInfo> Map::getOpenDoorIndexAndPoint() {
	PortalInfo garbage;
	pair<int32_t, PortalInfo> portal = make_pair(-1, garbage);
	pair<PortalInfo, bool> cur;
	for (size_t i = 0; i < m_door_points.size(); i++) {
		cur = m_door_points[i];
		if (!cur.second) {
			portal.first = i;
			portal.second = cur.first;
			cur.second = true;
			m_door_points[i] = cur;
			break;
		}
	}
	return portal;
}

PortalInfo * Map::getNearestSpawnPoint(const Pos &pos) {
	int8_t id = -1;
	int32_t distance = 200000;
	for (unordered_map<int8_t, PortalInfo>::iterator i = m_spawn_points.begin(); i != m_spawn_points.end(); i++) {
		int32_t cmp = i->second.pos - pos;
		if (cmp < distance) {
			id = i->first;
			distance = cmp;
		}
	}
	return getSpawnPoint(id);
}

void Map::addDoor(Door *door) {
	m_doors[door->getId()] = door;
}

void Map::removeDoor(boost::int32_t index, Door *door) {
	if (index != -1)
		m_door_points[index].second = false;
	m_doors.erase(door->getId());
}

// NPCs
void Map::removeNpc(uint32_t index) {
	if (isValidNpcIndex(index)) {
		NpcSpawnInfo npc = m_npc_spawns[index];
		NpcPacket::showNpc(getId(), npc, makeNpcId(), false);
		m_npc_spawns.erase(m_npc_spawns.begin() + index);
	}
}

bool Map::isValidNpcIndex(uint32_t id) {
	return (id < m_npc_spawns.size());
}

NpcSpawnInfo Map::getNpc(uint32_t id) const {
	return m_npc_spawns[id];
}

// Mobs
int32_t Map::spawnMob(int32_t mobid, const Pos &pos, int16_t fh, Mob *owner, int8_t summoneffect) {
	int32_t id = getObjectId();

	Mob *mob = new Mob(id, getId(), mobid, pos, fh);
	m_mobs[id] = mob;
	if (summoneffect != 0) {
		mob->setOwner(owner);
		owner->addSpawn(id, mob);
	}
	MobsPacket::spawnMob(nullptr, mob, summoneffect, owner, (owner == nullptr));
	updateMobControl(mob, true);
	return id;
}

int32_t Map::spawnMob(int32_t spawnid, const MobSpawnInfo &info) {
	int32_t id = getObjectId();

	Mob *mob = new Mob(id, getId(), info.id, info.pos, spawnid, info.facesRight, info.foothold);
	m_mobs[id] = mob;
	MobsPacket::spawnMob(nullptr, mob, 0, nullptr, true);
	updateMobControl(mob, true);
	return id;
}

int32_t Map::spawnShell(int32_t mobid, const Pos &pos, int16_t fh) {
	int32_t id = getObjectId();

	Mob *mob = new Mob(id, getId(), mobid, pos, fh, Mobs::ControlStatus::ControlNone);
	m_mobs[id] = mob;
	updateMobControl(mob, true);
	return id;
}

Mob * Map::getMob(int32_t id, bool isMapId) {
	if (isMapId)
		return (m_mobs.find(id) != m_mobs.end() ? m_mobs[id] : nullptr);

	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); iter++) {
		if (iter->second != nullptr && iter->second->getMobId() == id) {
			return iter->second;
		}
	}
	return nullptr;
}

void Map::updateMobControl(Player *player) {
	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); iter++) {
		if (iter->second != nullptr && iter->second->getControl() == player) {
			updateMobControl(iter->second);
		}
	}
}

void Map::updateMobControl(Mob *mob, bool spawn, Player *display) {
	int32_t maxpos = 200000;
	Player *p = nullptr;
	if (mob->getControlStatus() != Mobs::ControlStatus::ControlNone) {
		for (size_t j = 0; j < m_players.size(); j++) {
			Player *test = m_players[j];
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
	if (m_mobs.find(id) != m_mobs.end()) {
		if (spawnid >= 0 && m_mob_spawns[spawnid].time != -1) {
			// Add spawn point to respawns if mob was spawned by a spawn point.
			clock_t spawnTime = m_mob_spawns[spawnid].time * 1000 * (Randomizer::Instance()->randInt(200, 100) / 100);
			// Randomly spawn between 1x and 2x the spawn time
			spawnTime += TimeUtilities::getTickCount();
			m_mob_respawns.push_back(Respawnable(spawnid, spawnTime));
			m_mob_spawns[spawnid].spawned = false;
		}
		m_mobs.erase(id);
		if (m_timemob == id) {
			m_timemob = 0;
		}
	}
}

int32_t Map::killMobs(Player *player, int32_t mobid, bool playerkill, bool showpacket) {
	unordered_map<int32_t, Mob *> mobs = m_mobs;
	int32_t mobskilled = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) { // While loops cause problems
		if (iter->second != nullptr) {
			if ((mobid > 0 && iter->second->getMobId() == mobid) || mobid == 0) {
				if (playerkill && player != nullptr) {
					if (iter->second->getMobId() != Mobs::HorntailSponge) { // This will be taken care of by its parts
						iter->second->applyDamage(player->getId(), iter->second->getHp());
					}
				}
				else {
					iter->second->die(showpacket);
				}
				mobskilled++;
			}
		}
	}
	return mobskilled;
}

int32_t Map::countMobs(int32_t mobid) {
	unordered_map<int32_t, Mob *> mobs = m_mobs;
	int32_t mobcount = 0;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobs.begin(); iter != mobs.end(); iter++) {
		if (iter->second != nullptr) {
			if ((mobid > 0 && iter->second->getMobId() == mobid) || mobid == 0) {
				mobcount++;
			}
		}
	}
	return mobcount;
}

void Map::healMobs(int32_t hp, int32_t mp, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobmap = m_mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
		if (iter->second != nullptr && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->skillHeal(hp, mp);
		}
	}
}

void Map::statusMobs(vector<StatusInfo> &statuses, const Pos &origin, const Pos &lt, const Pos &rb) {
	unordered_map<int32_t, Mob *> mobmap = m_mobs;
	for (unordered_map<int32_t, Mob *>::iterator iter = mobmap.begin(); iter != mobmap.end(); iter++) {
		if (iter->second != nullptr && GameLogicUtilities::isInBox(origin, lt, rb, iter->second->getPos())) {
			iter->second->addStatus(0, statuses);
		}
	}
}

void Map::spawnZakum(const Pos &pos, int16_t fh) {
	int32_t pid = 0;
	Mob *p = nullptr;
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
	boost::recursive_mutex::scoped_lock l(m_drops_mutex);
	int32_t id = getObjectId();
	drop->setId(id);
	drop->setPos(findFloor(drop->getPos()));
	m_drops[id] = drop;
}

void Map::removeDrop(int32_t id) {
	boost::recursive_mutex::scoped_lock l(m_drops_mutex);
	if (m_drops.find(id) != m_drops.end()) {
		m_drops.erase(id);
	}
}

Drop * Map::getDrop(int32_t id) {
	boost::recursive_mutex::scoped_lock l(m_drops_mutex);
	return (m_drops.find(id) != m_drops.end() ? m_drops[id] : nullptr);
}

void Map::clearDrops(bool showPacket) {
	boost::recursive_mutex::scoped_lock l(m_drops_mutex);
	unordered_map<int32_t, Drop *> drops = m_drops;
	for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
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
		m_poison_mists[mist->getId()] = mist;
	}
	else {
		m_mists[mist->getId()] = mist;
	}

	new Timer::Timer(bind(&Map::removeMist, this, mist),
		Timer::Id(Timer::Types::MistTimer, mist->getId(), 0),
		getTimers(), Timer::Time::fromNow(mist->getTime() * 1000));

	MapPacket::spawnMist(getId(), mist);
}

Mist * Map::getMist(int32_t id) {
	if (m_mists.find(id) != m_mists.end()) {
		return m_mists[id];
	}
	return (m_poison_mists.find(id) != m_poison_mists.end() ? m_poison_mists[id] : nullptr);
}

void Map::removeMist(Mist *mist) {
	int32_t id = mist->getId();
	if (mist->isPoison() && !mist->isMobMist()) {
		m_poison_mists.erase(id);
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
	for (iter = mistlist.begin(); iter != mistlist.end(); iter++) {
		removeMist(iter->second);
	}
	mistlist = m_poison_mists;
	for (iter = mistlist.begin(); iter != mistlist.end(); iter++) {
		removeMist(iter->second);
	}
}

// Timer stuff
void Map::respawn(int8_t types) {
	if (types & SpawnTypes::Mob) {
		m_mob_respawns.clear();
		MobSpawnInfo *info;
		for (size_t i = 0; i < m_mob_spawns.size(); i++) {
			info = &m_mob_spawns[i];
			if (!info->spawned) {
				info->spawned = true;
				spawnMob(i, *info);
			}
		}
	}
	if (types & SpawnTypes::Reactor) {
		m_reactor_respawns.clear();
		Reactor *cur;
		for (size_t i = 0; i < m_reactors.size(); i++) {
			cur = m_reactors[i];
			if (!cur->isAlive()) {
				m_reactor_spawns[i].spawned = true;
				cur->restore();
			}
		}
	}
}

void Map::checkSpawn(clock_t time) {
	Respawnable *respawn;

	for (size_t i = 0; i < m_mob_respawns.size(); i++) {
		respawn = &m_mob_respawns[i];
		if (time > respawn->spawnAt) {
			m_mob_spawns[respawn->spawnId].spawned = true;
			spawnMob(respawn->spawnId, m_mob_spawns[respawn->spawnId]);

			m_mob_respawns.erase(m_mob_respawns.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < m_reactor_respawns.size(); i++) {
		respawn = &m_reactor_respawns[i];
		if (time > respawn->spawnAt) {
			m_reactor_spawns[respawn->spawnId].spawned = true;
			getReactor(respawn->spawnId)->restore();

			m_reactor_respawns.erase(m_reactor_respawns.begin() + i);
			i--;
		}
	}
}

void Map::checkShadowWeb() {
	if (m_webbed.size() > 0) {
		for (unordered_map<int32_t, Mob *>::iterator iter = m_webbed.begin(); iter != m_webbed.end(); iter++) {
			iter->second->applyWebDamage();
		}
	}
}

void Map::checkMists() {
	if (m_poison_mists.size() == 0) {
		return;
	}

	Mob *mob = nullptr;
	Mist *mist = nullptr;
	unordered_map<int32_t, Mist *>::iterator miter;

	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); ++iter) {
		mob = iter->second;
		if (mob == nullptr || mob->hasStatus(StatusEffects::Mob::Poison) || mob->getHp() == 1)
			continue;
		for (miter = m_poison_mists.begin(); miter != m_poison_mists.end(); ++miter) {
			mist = miter->second;
			if (GameLogicUtilities::isInBox(mist->getOrigin(), mist->getSkillLt(), mist->getSkillRb(), mob->getPos())) {
				bool poisoned = (MobHandler::handleMobStatus(PlayerDataProvider::Instance()->getPlayer(mist->getOwnerId()), mob, mist->getSkillId(), mist->getSkillLevel(), 0, 0) > 0);
				if (poisoned) // Mob is poisoned, don't need to check any more mists
					break;
			}
		}
	}
}

void Map::clearDrops(clock_t time) { // Clear drops based on how long they have been in the map
	boost::recursive_mutex::scoped_lock l(m_drops_mutex);
	time -= 180000; // Drops disappear after 3 minutes
	unordered_map<int32_t, Drop *> drops = m_drops;
	for (unordered_map<int32_t, Drop *>::iterator iter = drops.begin(); iter != drops.end(); iter++) {
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
	int32_t chour = TimeUtilities::getHour(false);
	TimeMob *tm = getTimeMob();
	if (firstLoad) {
		if (chour >= tm->startHour && chour < tm->endHour) {
			Pos p = findRandomPos();
			m_timemob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0);
			showMessage(tm->message, PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		if (chour == tm->startHour) {
			Pos p = findRandomPos();
			m_timemob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0);
			showMessage(tm->message, PlayerPacket::NoticeTypes::Blue);
		}
		else if (chour == tm->endHour && m_timemob != 0) {
			Mob *m = getMob(m_timemob);
			m->applyDamage(0, m->getHp());
		}
	}
}

void Map::setMapTimer(int32_t t) {
	if (t > 0 && m_timer != 0)
		return;
	m_timer = t;
	m_timerstart = time(0);
	MapPacket::showTimer(getId(), t);
	if (t > 0) {
		new Timer::Timer(bind(&Map::setMapTimer, this, 0),
			Timer::Id(Timer::Types::MapTimer, getId(), 25),
			getTimers(), Timer::Time::fromNow(t * 1000));
	}
}

void Map::showObjects(Player *player) { // Show all Map Objects
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
			Summons::showSummons(m_players[i], player);
			// Bug in global; would be fixed here:
			// Hurricane/Pierce do not display properly if using when someone enters the map
			// Berserk does not display properly either - players[i]->getActiveBuffs()->getBerserk()
		}
	}

	// NPCs
	for (size_t i = 0; i < m_npc_spawns.size(); i++) {
		NpcPacket::showNpc(player, m_npc_spawns[i], i + Map::NpcStart);
	}

	// Player NPCs
	PlayerNpcDataProvider::Instance()->makePacket(m_npc_spawns, player);

	// Reactors
	for (size_t i = 0; i < m_reactors.size(); i++) {
		if (m_reactors[i]->isAlive())
			ReactorPacket::showReactor(player, m_reactors[i]);
	}

	// Mobs
	for (unordered_map<int32_t, Mob *>::iterator iter = m_mobs.begin(); iter != m_mobs.end(); iter++) {
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
		boost::recursive_mutex::scoped_lock l(m_drops_mutex);
		for (unordered_map<int32_t, Drop *>::iterator iter = m_drops.begin(); iter != m_drops.end(); iter++) {
			if (iter->second != nullptr) {
				iter->second->showDrop(player);
			}
		}
	}

	// Mists
	for (unordered_map<int32_t, Mist *>::iterator iter = m_mists.begin(); iter != m_mists.end(); iter++) {
		if (iter->second != nullptr) {
			MapPacket::showMist(player, iter->second);
		}
	}

	// Mystic doors
	Door *door = nullptr;
	for (unordered_map<int32_t, Door *>::iterator iter = m_doors.begin(); iter != m_doors.end(); iter++) {
		door = iter->second;
		if (door->getOwner() == player || (player->getParty() != 0 && door->getOwner()->getParty() == player->getParty())) {
			MapPacket::showDoor(player, door, (this == door->getTown()));
		}
	}

	if (Party *p = player->getParty()) {
		p->showHpBar(player);
		p->receiveHpBar(player);
	}

	if (hasClock()) {
		time_t rawtime;
		time(&rawtime);
		struct tm *timeinfo = localtime(&rawtime);
		MapPacket::showClock(player, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	}

	if (m_weatherItemid != 0) {
		MapPacket::changeWeatherPlayer(player, m_weatherAdmin, m_weatherItemid, m_weatherMessage);
	}

	if (m_jukeboxItemid != 0) {
		InventoryPacket::playCashSongPlayer(player, m_jukeboxItemid, m_jukeboxPlayer);
	}
	
	for (unordered_map<int32_t, Kite>::iterator iter = m_kites.begin(); iter != m_kites.end(); iter++) {
		MapPacket::spawnKite(player, iter->second);
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

bool Map::createWeather(Player *player, bool adminWeather, int32_t time, int32_t itemid, const string &message) {
	Timer::Id timerId(Timer::Types::WeatherTimer, 0, 0); // Just to check if there's already a weather item running and adding a new one
	if (itemid != 0 && getTimers()->checkTimer(timerId) != 0) {
		// Hacking
		return false;
	}

	setWeather(adminWeather, itemid, message);
	new Timer::Timer(bind(&Map::setWeather, this, false, 0, ""), timerId, getTimers(), Timer::Time::fromNow(time));
	return true;
}

bool Map::createKite(Player *player, int32_t itemid, const string &message) {
	int8_t minX = 60, minY = 80;
	Pos &playerPos = player->getPos();
	for (unordered_map<int32_t, Kite>::iterator iter = m_kites.begin(); iter != m_kites.end(); iter++) {
		Kite &kite = iter->second;
		if (abs(kite.position.y - playerPos.y) < minY) {
			if (abs(kite.position.x - playerPos.x) < minX) {
				return false;
			}
		}
	}

	// Create new kite!
	Kite newKite;
	newKite.itemid = itemid;
	newKite.id = m_kiteids.next();
	newKite.ownerName = player->getName();
	newKite.ownerId = player->getId();
	newKite.message = message;
	newKite.lifeTime = 3600000; // BMS like...lol!
	newKite.map = this->getId();
	newKite.position = Pos(playerPos);
	m_kites[newKite.id] = newKite;

	Timer::Id timerId(Timer::Types::KiteTimer, newKite.id, 0);
	new Timer::Timer(bind(&Map::removeKite, this, newKite.id, 1), timerId, getTimers(), Timer::Time::fromNow(newKite.lifeTime));

	MapPacket::spawnKite(newKite);

	return true;
}

bool Map::playJukebox(Player *player, int32_t itemid, int32_t time) {
	Timer::Id timerId(Timer::Types::JukeboxTimer, 0, 0); // Just to check if there's already a jukebox running and adding a new one
	if (m_jukeboxItemid != 0 && getTimers()->checkTimer(timerId) != 0) {
		// Someone plays the jukebox already.
		return false;
	}

	setJukebox(itemid, player->getName());
	new Timer::Timer(bind(&Map::setJukebox, this, itemid, ""), timerId, getTimers(), Timer::Time::fromNow(time * 1000));
	return true;
}

void Map::setWeather(bool adminWeather, int32_t itemid, const string &message) {
	m_weatherItemid = itemid;
	m_weatherMessage = message;
	m_weatherAdmin = adminWeather;

	MapPacket::changeWeather(getId(), adminWeather, itemid, message);
}

void Map::setJukebox(int32_t itemid, const string &user) {
	m_jukeboxItemid = itemid;
	m_jukeboxPlayer = user;

	if (itemid != 0) {
		// There's actually no way to stop the damn song other than rejoining the map.
		InventoryPacket::playCashSong(getId(), itemid, user);
	}
}

void Map::removeKite(int32_t id, int8_t reason) {
	if (m_kites.find(id) != m_kites.end()) {
		Timer::Id timerId(Timer::Types::KiteTimer, id, 0);
		if (getTimers()->checkTimer(timerId)) {
			getTimers()->removeTimer(timerId);
		}

		Kite kite = m_kites[id];
		MapPacket::despawnKite(kite, reason);
		m_kites.erase(id);
	}
}

void Map::removeKitesPlayer(int32_t playerid) {
	unordered_map<int32_t, Kite> tmp(m_kites);
	
	for (unordered_map<int32_t, Kite>::iterator iter = tmp.begin(); iter != tmp.end(); iter++) {
		Kite &kite = iter->second;
		if (kite.ownerId == playerid) {
			Timer::Id timerId(Timer::Types::KiteTimer, kite.id, 0);
			if (getTimers()->checkTimer(timerId)) {
				getTimers()->removeTimer(timerId);
			}

			MapPacket::despawnKite(kite, 0);
			m_kites.erase(kite.id);
		}
	}
	tmp.clear();
}