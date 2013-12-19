/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "ChannelServer.h"
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
#include <initializer_list>
#include <iostream>
#include <stdexcept>

using std::bind;

Map::Map(MapInfoPtr info, int32_t id) :
	m_info(info),
	m_id(id),
	m_objectIds(1000),
	m_instance(nullptr),
	m_timer(0),
	m_timeMob(0),
	m_emptyMapTicks(0),
	m_spawnMobs(-1),
	m_minSpawnCount(0),
	m_maxSpawnCount(0),
	m_lastSpawn(seconds_t(0)),
	m_music(info->defaultMusic)
{
	m_timers = std::make_unique<Timer::Container>();

	// Dynamic loading, start the map timer once the object is created
	Timer::create([this]() { this->mapTick(); },
		Timer::Id(Timer::Types::MapTimer, id, 0),
		getTimers(), seconds_t(0), seconds_t(1));

	double mapHeight = std::max(info->dimensions.rightBottom.y, static_cast<int16_t>(800));
	double mapWidth = std::max(info->dimensions.rightBottom.x - 450, 600);
	m_minSpawnCount = std::min(40, std::max(1, static_cast<int32_t>(mapHeight * mapWidth * info->spawnRate * 0.0000078125)));
	m_maxSpawnCount = m_minSpawnCount * 2;
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

void Map::addSeat(const SeatInfo &seat) {
	m_seats[seat.id] = seat;
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
	Timer::create([this]() { this->timeMob(false); },
		Timer::Id(Timer::Types::MapTimer, getId(), 1),
		getTimers(), TimeUtilities::getDistanceToNextOccurringSecondOfHour(0), hours_t(1));

	Timer::create([this]() { this->timeMob(true); },
		Timer::Id(Timer::Types::MapTimer, getId(), 2),
		getTimers(), seconds_t(3)); // First check

	m_timeMobInfo = info;
}

// Players
void Map::addPlayer(Player *player) {
	m_players.push_back(player);
	if (forceMapEquip()) {
		MapPacket::forceMapEquip(player);
	}
	if (!player->isUsingGmHide()) {
		MapPacket::showPlayer(player);
	}
	else {
		GmPacket::beginHide(player);
	}
	if (m_timer.count() > 0) {
		MapPacket::showTimer(player, m_timer - std::chrono::duration_cast<seconds_t>(TimeUtilities::getNow() - m_timerStart));
	}
	else if (Instance *i = getInstance()) {
		if (i->showTimer() && i->checkInstanceTimer().count() > 0) {
			MapPacket::showTimer(player, i->checkInstanceTimer());
		}
	}
	if (m_ship) {
		// Boat packet, need to change this section slightly...
	}

	checkPlayerEquip(player);
}

void Map::checkPlayerEquip(Player *player) {
	if (!player->isGm()) {
		int32_t dps = getInfo()->damagePerSecond;
		if (dps > 0) {
			int32_t protectItem = getInfo()->protectItem;
			int32_t playerId = player->getId();
			if (protectItem > 0) {
				if (!player->getInventory()->isEquippedItem(protectItem)) {
					m_playersWithoutProtectItem[playerId] = player;
				}
				else {
					auto kvp = m_playersWithoutProtectItem.find(playerId);
					if (kvp != m_playersWithoutProtectItem.end()) {
						m_playersWithoutProtectItem.erase(kvp);
					}
				}
			}
			else {
				m_playersWithoutProtectItem[playerId] = player;
			}
		}
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
	int32_t playerId = player->getId();
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

	auto kvp = m_playersWithoutProtectItem.find(playerId);
	if (kvp != m_playersWithoutProtectItem.end()) {
		m_playersWithoutProtectItem.erase(kvp);
	}
}

void Map::runFunctionPlayers(const Pos &origin, const Rect &dimensions, int16_t prop, function<void(Player *)> successFunc) {
	runFunctionPlayers(origin, dimensions, prop, 0, successFunc);
}

void Map::runFunctionPlayers(const Pos &origin, const Rect &dimensions, int16_t prop, int16_t count, function<void(Player *)> successFunc) {
	int16_t done = 0;
	for (const auto &player : m_players) {
		if (GameLogicUtilities::isInBox(origin, dimensions, player->getPos()) && Randomizer::rand<int16_t>(99) < prop) {
			successFunc(player);
			done++;
		}
		if (count > 0 && done == count) {
			break;
		}
	}
}

void Map::runFunctionPlayers(function<void (Player *)> successFunc) {
	for (const auto &player : m_players) {
		successFunc(player);
	}
}

void Map::buffPlayers(int32_t buffId) {
	for (const auto &player : m_players) {
		if (player->getStats()->getHp() > 0) {
			Inventory::useItem(player, buffId);
			EffectPacket::sendMobItemBuffEffect(player, buffId);
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
	ReactorSpawnInfo &info = m_reactorSpawns[id];
	if (info.time >= 0) {
		// We don't want to respawn -1s, leave that to some script
		time_point_t reactorRespawn = TimeUtilities::getNowWithTimeAdded(seconds_t(info.time));
		m_reactorRespawns.push_back(Respawnable(id, reactorRespawn));
	}
}

void Map::killReactors(bool showPacket) {
	for (const auto &reactor : m_reactors) {
		if (reactor->isAlive()) {
			reactor->kill();
			if (showPacket) {
				ReactorPacket::destroyReactor(reactor);
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
	for (const auto &foothold : m_footholds) {
		const Pos &pos1 = foothold.pos1;
		const Pos &pos2 = foothold.pos2;
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
	const Rect &dimensions = m_info->dimensions;
	int16_t xMin = (dimensions.leftTop.x != 0 ? dimensions.leftTop.x : 0x8000);
	int16_t xMax = (dimensions.rightBottom.x != 0 ? dimensions.rightBottom.x : 0x7FFF);
	int16_t yMin = (dimensions.leftTop.y != 0 ? dimensions.leftTop.y : 0x8000);
	int16_t yMax = (dimensions.rightBottom.y != 0 ? dimensions.rightBottom.y : 0x7FFF);
	int16_t xPos = 0;
	int16_t yPos = 0;
	int16_t xTemp = 0;
	int16_t yTemp = 0;
	Pos pos(0, 0);
	Pos tPos;
	while (pos.x == 0 && pos.y == 0) {
		xTemp = Randomizer::rand<int16_t>(xMax, xMin);
		yTemp = Randomizer::rand<int16_t>(yMax, yMin);
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
	for (const auto &cur : m_footholds) {
		if (((pos.x > cur.pos1.x && pos.x <= cur.pos2.x) || (pos.x > cur.pos2.x && pos.x <= cur.pos1.x)) && ((pos.y > cur.pos1.y && pos.y <= cur.pos2.y) || (pos.y > cur.pos2.y && pos.y <= cur.pos1.y))) {
			foothold = cur.id;
			break;
		}
	}
	return foothold;
}

// Portals
PortalInfo * Map::getPortal(const string &name) {
	auto portal = m_portals.find(name);
	return portal != m_portals.end() ? &portal->second : nullptr;
}

PortalInfo * Map::getSpawnPoint(int8_t portalId) {
	int8_t id = (portalId != -1 ? portalId : Randomizer::rand<int8_t>(m_spawnPoints.size() - 1));
	return &m_spawnPoints[id];
}

PortalInfo * Map::getNearestSpawnPoint(const Pos &pos) {
	int8_t id = -1;
	int32_t distance = 200000;
	for (const auto &kvp : m_spawnPoints) {
		const PortalInfo &info = kvp.second;
		int32_t cmp = info.pos - pos;
		if (cmp < distance) {
			id = kvp.first;
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

int32_t Map::addNpc(const NpcSpawnInfo &npc) {
	m_npcSpawns.push_back(npc);
	NpcPacket::showNpc(getId(), npc, makeNpcId());

	if (NpcDataProvider::Instance()->isMapleTv(npc.id)) {
		MapleTvs::Instance()->addMap(this);
	}

	return m_npcSpawns.size() - 1;
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
		auto kvp = m_mobs.find(id);
		return (kvp != m_mobs.end() ? kvp->second : nullptr);
	}
	for (const auto &kvp : m_mobs) {
		if (Mob *mob = kvp.second) {
			if (mob->getMobId() == id) {
				return mob;
			}
		}
	}
	return nullptr;
}

void Map::updateMobControl(Player *player) {
	for (const auto &kvp : m_mobs) {
		if (Mob *mob = kvp.second) {
			if (mob->getControl() == player) {
				updateMobControl(mob);
			}
		}
	}
}

void Map::updateMobControl(Mob *mob, bool spawn, Player *display) {
	int32_t maxPos = 200000;
	Player *newController = nullptr;
	if (mob->getControlStatus() != Mobs::ControlStatus::ControlNone) {
		for (const auto &player : m_players) {
			if (!player->isUsingGmHide()) {
				int32_t curPos = mob->getPos() - player->getPos();
				if (curPos < maxPos) {
					maxPos = curPos;
					newController = player;
					break;
				}
			}
		}
	}
	mob->setControl(newController, spawn, display);
}

void Map::removeMob(int32_t id, int32_t spawnId) {
	auto mob = m_mobs.find(id);
	if (mob != m_mobs.end()) {
		if (spawnId >= 0) {
			MobSpawnInfo &spawn = m_mobSpawns[spawnId];
			if (spawn.time != -1) {
				// Add spawn point to respawns if mob was spawned by a spawn point
				// Randomly spawn between 1x and 2x the spawn time
				seconds_t timeModifier = seconds_t(spawn.time * (Randomizer::rand<int32_t>(200, 100)) / 100);
				time_point_t spawnTime = TimeUtilities::getNowWithTimeAdded<seconds_t>(timeModifier);
				m_mobRespawns.push_back(Respawnable(spawnId, spawnTime));
				spawn.spawned = false;
			}
		}
		m_mobs.erase(mob);

		if (m_timeMob == id) {
			m_timeMob = 0;
		}
	}
}

int32_t Map::killMobs(Player *player, int32_t mobId, bool playerInitiated, bool showPacket) {
	unordered_map<int32_t, Mob *> mobs = m_mobs;
	int32_t mobsKilled = 0;
	for (const auto &kvp : mobs) {
		if (Mob *mob = kvp.second) {
			if ((mobId > 0 && mob->getMobId() == mobId) || mobId == 0) {
				if (playerInitiated && player != nullptr) {
					if (!mob->isSponge()) {
						// Sponges will be taken care of by their parts
						mob->applyDamage(player->getId(), mob->getHp());
					}
				}
				else {
					mob->die(showPacket);
				}
				mobsKilled++;
			}
		}
	}
	return mobsKilled;
}

int32_t Map::countMobs(int32_t mobId) {
	unordered_map<int32_t, Mob *> mobs = m_mobs;
	int32_t mobCount = 0;
	for (const auto &kvp : mobs) {
		if (Mob *mob = kvp.second) {
			if ((mobId > 0 && mob->getMobId() == mobId) || mobId == 0) {
				mobCount++;
			}
		}
	}
	return mobCount;
}

void Map::healMobs(int32_t hp, int32_t mp, const Pos &origin, const Rect &dimensions) {
	unordered_map<int32_t, Mob *> mobMap = m_mobs;
	for (const auto &kvp : mobMap) {
		if (Mob *mob = kvp.second) {
			if (GameLogicUtilities::isInBox(origin, dimensions, mob->getPos())) {
				mob->skillHeal(hp, mp);
			}
		}
	}
}

void Map::statusMobs(vector<StatusInfo> &statuses, const Pos &origin, const Rect &dimensions) {
	unordered_map<int32_t, Mob *> mobMap = m_mobs;
	for (const auto &kvp : mobMap) {
		if (Mob *mob = kvp.second) {
			if (GameLogicUtilities::isInBox(origin, dimensions, mob->getPos())) {
				mob->addStatus(0, statuses);
			}
		}
	}
}

void Map::spawnZakum(const Pos &pos, int16_t fh) {
	int32_t partId = 0;
	Mob *p = nullptr;
	Mob *body = getMob(spawnShell(Mobs::ZakumBody1, pos, fh));

	std::initializer_list<int32_t> parts = {
		Mobs::ZakumArm1, Mobs::ZakumArm2, Mobs::ZakumArm3,
		Mobs::ZakumArm4, Mobs::ZakumArm5, Mobs::ZakumArm6,
		Mobs::ZakumArm7, Mobs::ZakumArm8
	};

	for (const auto &part : parts) {
		partId = spawnMob(part, pos, fh);
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
	std::unique_lock<std::recursive_mutex> l(m_dropsMutex);
	int32_t id = getObjectId();
	drop->setId(id);
	drop->setPos(findFloor(drop->getPos()));
	m_drops[id] = drop;
}

void Map::removeDrop(int32_t id) {
	std::unique_lock<std::recursive_mutex> l(m_dropsMutex);
	auto drop = m_drops.find(id);
	if (drop != m_drops.end()) {
		m_drops.erase(drop);
	}
}

Drop * Map::getDrop(int32_t id) {
	std::unique_lock<std::recursive_mutex> l(m_dropsMutex);
	auto drop = m_drops.find(id);
	return (drop != m_drops.end() ? drop->second : nullptr);
}

void Map::clearDrops(bool showPacket) {
	std::unique_lock<std::recursive_mutex> l(m_dropsMutex);
	unordered_map<int32_t, Drop *> drops = m_drops;
	for (const auto &drop : drops) {
		drop.second->removeDrop(showPacket);
	}
}

// Seats
bool Map::seatOccupied(int16_t id) {
	bool occupied = true;
	auto seat = m_seats.find(id);
	if (seat != m_seats.end()) {
		occupied = (seat->second.occupant != nullptr);
	}
	return occupied;
}

void Map::playerSeated(int16_t id, Player *player) {
	auto seat = m_seats.find(id);
	if (m_seats.find(id) != m_seats.end()) {
		seat->second.occupant = player;
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

	Timer::create([this, mist]() { this->removeMist(mist); },
		Timer::Id(Timer::Types::MistTimer, mist->getId(), 0),
		getTimers(), seconds_t(mist->getTime()));

	MapPacket::spawnMist(getId(), mist);
}

Mist * Map::getMist(int32_t id) {
	auto mist = m_mists.find(id);
	if (mist != m_mists.end()) {
		return mist->second;
	}
	mist = m_poisonMists.find(id);
	return (mist != m_poisonMists.end() ? mist->second : nullptr);
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
	unordered_map<int32_t, Mist *> mistlist = m_mists;
	for (const auto &mist : mistlist) {
		removeMist(mist.second);
	}
	mistlist = m_poisonMists;
	for (const auto &mist : mistlist) {
		removeMist(mist.second);
	}
}

// Timer stuff
void Map::respawn(int8_t types) {
	if (types & SpawnTypes::Mob) {
		m_mobRespawns.clear();
		for (size_t spawnId = 0; spawnId < m_mobSpawns.size(); spawnId++) {
			MobSpawnInfo &info = m_mobSpawns[spawnId];
			if (!info.spawned) {
				info.spawned = true;
				spawnMob(spawnId, info);
			}
		}
	}
	if (types & SpawnTypes::Reactor) {
		m_reactorRespawns.clear();
		for (size_t spawnId = 0; spawnId < m_reactors.size(); ++spawnId) {
			Reactor *reactor = m_reactors[spawnId];
			if (!reactor->isAlive()) {
				m_reactorSpawns[spawnId].spawned = true;
				reactor->restore();
			}
		}
	}
}

void Map::checkSpawn(time_point_t time) {
	if (std::chrono::duration_cast<seconds_t>(time - m_lastSpawn) < seconds_t(8)) return;

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

	m_lastSpawn = time;
}

void Map::checkShadowWeb() {
	if (m_webbed.size() > 0) {
		for (const auto &mob : m_webbed) {
			mob.second->applyWebDamage();
		}
	}
}

void Map::checkMists() {
	if (m_poisonMists.size() == 0) {
		return;
	}

	Mob *mob = nullptr;
	Mist *mist = nullptr;

	for (const auto &kvp : m_mobs) {
		mob = kvp.second;

		if (mob == nullptr || mob->hasStatus(StatusEffects::Mob::Poison) || mob->getHp() == 1) {
			continue;
		}

		for (const auto &kvp : m_poisonMists) {
			mist = kvp.second;
			if (GameLogicUtilities::isInBox(mist->getOrigin(), mist->getSkillDimensions(), mob->getPos())) {
				bool poisoned = (MobHandler::handleMobStatus(mist->getOwnerId(), mob, mist->getSkillId(), mist->getSkillLevel(), 0, 0) > 0);
				if (poisoned) {
					// Mob is poisoned, don't need to check any more mists
					break;
				}
			}
		}
	}
}

void Map::clearDrops(time_point_t time) {
	// Clear drops based on how long they have been in the map
	std::unique_lock<std::recursive_mutex> l(m_dropsMutex);

	time -= minutes_t(3); // Drops disappear after 3 minutes

	unordered_map<int32_t, Drop *> drops = m_drops;
	for (const auto &kvp : drops) {
		if (Drop *drop = kvp.second) {
			if (drop->getDroppedAtTime() < time) {
				drop->removeDrop();
			}
		}
	}
}

void Map::mapTick() {
	int32_t mapUnloadTime = ChannelServer::Instance()->getMapUnloadTime();
	if (mapUnloadTime > 0) {
		if (m_players.size() > 0) {
			m_emptyMapTicks = 0;
		}
		else {
			m_emptyMapTicks++;
			if (m_emptyMapTicks > mapUnloadTime) {
				Maps::unloadMap(getId());
				return;
			}
		}
	}

	time_point_t time = TimeUtilities::getNow();

	checkSpawn(time);
	clearDrops(time);
	checkMists();

	if (TimeUtilities::getSecond() % 3 == 0) {
		checkShadowWeb();
	}
	int32_t dps = getInfo()->damagePerSecond;
	if (dps > 0 && m_playersWithoutProtectItem.size() > 0) {
		for (const auto &kvp : m_playersWithoutProtectItem) {
			if (Player *player = kvp.second) {
				if (!player->getStats()->isDead() && !player->hasGmBenefits()) {
					player->getStats()->damageHp(dps);
				}
			}
		}
	}
}

void Map::timeMob(bool firstLoad) {
	int32_t cHour = TimeUtilities::getHour(false);
	TimeMob *tm = getTimeMob();
	if (firstLoad) {
		if (cHour >= tm->startHour && cHour < tm->endHour) {
			const Pos &p = findRandomPos();
			m_timeMob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0);
			showMessage(tm->message, PlayerPacket::NoticeTypes::Blue);
		}
	}
	else {
		if (cHour == tm->startHour) {
			const Pos &p = findRandomPos();
			m_timeMob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0);
			showMessage(tm->message, PlayerPacket::NoticeTypes::Blue);
		}
		else if (cHour == tm->endHour && m_timeMob != 0) {
			Mob *m = getMob(m_timeMob);
			m->applyDamage(0, m->getHp());
		}
	}
}

void Map::setMapTimer(const seconds_t &timer) {
	if (timer.count() > 0 && m_timer.count() != 0) {
		throw std::runtime_error("Timer already executing on map " + StringUtilities::lexical_cast<string>(getId()));
	}

	m_timer = timer;
	m_timerStart = TimeUtilities::getNow();

	MapPacket::showTimer(getId(), timer);
	if (timer.count() > 0) {
		Timer::create([this]() { this->setMapTimer(seconds_t(0)); },
			Timer::Id(Timer::Types::MapTimer, getId(), 25),
			getTimers(), timer);
	}
}

void Map::showObjects(Player *player) {
	// Music
	if (getMusic() != getDefaultMusic()) {
		EffectPacket::playMusic(player, getMusic());
	}

	// MapleTV messengers
	if (MapleTvs::Instance()->isMapleTvMap(getId()) && MapleTvs::Instance()->hasMessage()) {
		PacketCreator packet;
		MapleTvs::Instance()->getMapleTvEntryPacket(packet);
		player->getSession()->send(packet);
	}

	// Players
	for (const auto &mapPlayer : m_players) {
		if (player != mapPlayer && !mapPlayer->isUsingGmHide()) {
			PacketCreator packet = MapPacket::playerPacket(mapPlayer);
			player->getSession()->send(packet);
			SummonHandler::showSummons(mapPlayer, player);
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
	for (const auto &reactor : m_reactors) {
		if (reactor->isAlive()) {
			ReactorPacket::showReactor(player, reactor);
		}
	}

	// Mobs
	for (const auto &kvp : m_mobs) {
		if (Mob *mob = kvp.second) {
			if (mob->getControlStatus() == Mobs::ControlStatus::ControlNone) {
				updateMobControl(mob, true, player);
			}
			else {
				MobsPacket::spawnMob(player, mob, 0, nullptr, false, true);
				updateMobControl(mob);
			}
		}
	}

	// Drops
	{
		std::unique_lock<std::recursive_mutex> l(m_dropsMutex);
		for (const auto &kvp : m_drops) {
			if (Drop *drop = kvp.second) {
				drop->showDrop(player);
			}
		}
	}

	// Mists
	for (const auto &kvp : m_mists) {
		if (Mist *mist = kvp.second) {
			MapPacket::showMist(player, mist);
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
	for (const auto &mapPlayer : m_players) {
		if (mapPlayer != player) {
			mapPlayer->getSession()->send(packet);
		}
	}
}

void Map::showMessage(const string &message, int8_t type) {
	for (const auto &mapPlayer : m_players) {
		PlayerPacket::showMessage(mapPlayer, message, type);
	}
}

bool Map::createWeather(Player *player, bool adminWeather, int32_t time, int32_t itemId, const string &message) {
	Timer::Id timerId(Timer::Types::WeatherTimer, 0, 0); // Just to check if there's already a weather item running and adding a new one
	if (getTimers()->isTimerRunning(timerId)) {
		// Hacking
		return false;
	}

	MapPacket::changeWeather(getId(), adminWeather, itemId, message);
	Timer::create([this, adminWeather]() { MapPacket::changeWeather(this->getId(), adminWeather, 0, ""); },
		timerId, getTimers(), seconds_t(time));
	return true;
}