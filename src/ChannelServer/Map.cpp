/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Map.hpp"
#include "Algorithm.hpp"
#include "ChannelServer.hpp"
#include "Drop.hpp"
#include "EffectPacket.hpp"
#include "GameLogicUtilities.hpp"
#include "GmPacket.hpp"
#include "Instance.hpp"
#include "Inventory.hpp"
#include "MapleTvPacket.hpp"
#include "MapleTvs.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "MiscUtilities.hpp"
#include "Mist.hpp"
#include "Mob.hpp"
#include "MobConstants.hpp"
#include "MobHandler.hpp"
#include "MobsPacket.hpp"
#include "NpcDataProvider.hpp"
#include "NpcPacket.hpp"
#include "PacketWrapper.hpp"
#include "Party.hpp"
#include "Player.hpp"
#include "PlayerPacket.hpp"
#include "Randomizer.hpp"
#include "ReactorPacket.hpp"
#include "Reactor.hpp"
#include "Session.hpp"
#include "SplitPacketBuilder.hpp"
#include "SummonHandler.hpp"
#include "TimeUtilities.hpp"
#include "Timer.hpp"
#include <ctime>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

Map::Map(ref_ptr_t<MapInfo> info, int32_t id) :
	m_info(info),
	m_id(id),
	m_objectIds(1000),
	m_music(info->defaultMusic)
{
	// Dynamic loading, start the map timer once the object is created
	Timer::Timer::create([this](const time_point_t &now) { this->mapTick(now); },
		Timer::Id(Timer::Types::MapTimer, id, 0),
		getTimers(), seconds_t(0), seconds_t(1));

	Pos rightBottom = info->dimensions.rightBottom();
	double mapHeight = std::max<double>(rightBottom.y, 800);
	double mapWidth = std::max<double>(rightBottom.x - 450, 600);
	m_minSpawnCount = ext::constrain_range(static_cast<int32_t>((mapHeight * mapWidth * info->spawnRate) / 128000.), 1, 40);
	m_maxSpawnCount = m_minSpawnCount * 2;
}

// Map info
auto Map::setMusic(const string_t &musicName) -> void {
	m_music = (musicName == "default" ? m_info->defaultMusic : musicName);
	send(EffectPacket::playMusic(m_music));
}

auto Map::setMobSpawning(int32_t spawn) -> void {
	m_spawnMobs = spawn;
}

auto Map::makeNpcId(uint32_t receivedId) -> uint32_t {
	return receivedId - NpcStart;
}

auto Map::makeNpcId() -> uint32_t {
	return m_npcSpawns.size() - 1 + NpcStart;
}

auto Map::makeReactorId(uint32_t receivedId) -> uint32_t {
	return receivedId - ReactorStart;
}

auto Map::makeReactorId() -> uint32_t {
	return m_reactorSpawns.size() - 1 + ReactorStart;
}

// Data initialization
auto Map::addFoothold(const FootholdInfo &foothold) -> void {
	m_footholds.push_back(foothold);
}

auto Map::addSeat(const SeatInfo &seat) -> void {
	m_seats[seat.id] = seat;
}

auto Map::addReactorSpawn(const ReactorSpawnInfo &spawn) -> void {
	m_reactorSpawns.push_back(spawn);
	m_reactorSpawns[m_reactorSpawns.size() - 1].spawned = true;
	Reactor *reactor = new Reactor(getId(), spawn.id, spawn.pos, spawn.facesLeft);
	send(ReactorPacket::spawnReactor(reactor));
}

auto Map::addMobSpawn(const MobSpawnInfo &spawn) -> void {
	m_mobSpawns.push_back(spawn);
	m_mobSpawns[m_mobSpawns.size() - 1].spawned = true;
	spawnMob(m_mobSpawns.size() - 1, spawn);
}

auto Map::addPortal(const PortalInfo &portal) -> void {
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

auto Map::addTimeMob(ref_ptr_t<TimeMob> info) -> void {
	Timer::Timer::create([this](const time_point_t &now) { this->timeMob(false); },
		Timer::Id(Timer::Types::MapTimer, getId(), 1),
		getTimers(), TimeUtilities::getDistanceToNextOccurringSecondOfHour(0), hours_t(1));

	Timer::Timer::create([this](const time_point_t &now) { this->timeMob(true); },
		Timer::Id(Timer::Types::MapTimer, getId(), 2),
		getTimers(), seconds_t(3)); // First check

	m_timeMobInfo = info;
}

// Players
auto Map::addPlayer(Player *player) -> void {
	m_players.push_back(player);
	if (m_info->forceMapEquip) {
		player->send(MapPacket::forceMapEquip());
	}
	if (!player->isUsingGmHide()) {
		send(MapPacket::playerPacket(player), player);
	}
	else {
		player->send(GmPacket::beginHide());
	}
	if (m_timer.count() > 0) {
		player->send(MapPacket::showTimer(m_timer - duration_cast<seconds_t>(TimeUtilities::getNow() - m_timerStart)));
	}
	else if (Instance *instance = getInstance()) {
		if (instance->showTimer() && instance->checkInstanceTimer().count() > 0) {
			player->send(MapPacket::showTimer(instance->checkInstanceTimer()));
		}
	}
	if (m_ship) {
		// Boat packet, need to change this section slightly...
	}

	checkPlayerEquip(player);
}

auto Map::checkPlayerEquip(Player *player) -> void {
	if (!player->hasGmBenefits()) {
		int32_t dps = m_info->damagePerSecond;
		if (dps > 0) {
			int32_t protectItem = m_info->protectItem;
			int32_t playerId = player->getId();
			if (protectItem > 0) {
				if (!player->getInventory()->isEquippedItem(protectItem)) {
					m_playersWithoutProtectItem[playerId] = player;
				}
				else {
					auto kvp = m_playersWithoutProtectItem.find(playerId);
					if (kvp != std::end(m_playersWithoutProtectItem)) {
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

auto Map::boatDock(bool isDocked) -> void {
	m_ship = isDocked;
	// TODO FIXME packet
}

auto Map::getNumPlayers() const -> size_t {
	return m_players.size();
}

auto Map::getPlayer(uint32_t index) const -> Player * {
	return m_players[index];
}

auto Map::getPlayerNames() -> string_t {
	out_stream_t names;
	for (size_t i = 0; i < m_players.size(); i++) {
		if (Player *player = m_players[i]) {
			names << player->getName() << " ";
		}
	}
	return names.str();
}

auto Map::removePlayer(Player *player) -> void {
	int32_t playerId = player->getId();
	for (size_t i = 0; i < m_players.size(); i++) {
		if (m_players[i] == player) {
			m_players.erase(std::begin(m_players) + i);
			break;
		}
	}
	SummonHandler::removeSummon(player, true, false, SummonMessages::None);
	SummonHandler::removeSummon(player, false, true, SummonMessages::None);
	send(MapPacket::removePlayer(player->getId()), player);
	updateMobControl(player);

	auto kvp = m_playersWithoutProtectItem.find(playerId);
	if (kvp != std::end(m_playersWithoutProtectItem)) {
		m_playersWithoutProtectItem.erase(kvp);
	}
}

auto Map::runFunctionPlayers(const Rect &dimensions, int16_t prop, function_t<void(Player *)> successFunc) -> void {
	runFunctionPlayers(dimensions, prop, 0, successFunc);
}

auto Map::runFunctionPlayers(const Rect &dimensions, int16_t prop, int16_t count, function_t<void(Player *)> successFunc) -> void {
	int16_t done = 0;
	// Prevent iterator invalidation
	auto copy = m_players;
	for (const auto &player : copy) {
		if (dimensions.contains(player->getPos()) && Randomizer::rand<int16_t>(99) < prop) {
			successFunc(player);
			done++;
		}
		if (count > 0 && done == count) {
			break;
		}
	}
}

auto Map::runFunctionPlayers(function_t<void(Player *)> successFunc) -> void {
	// Prevent iterator invalidation
	auto copy = m_players;
	for (const auto &player : copy) {
		successFunc(player);
	}
}

auto Map::buffPlayers(int32_t buffId) -> void {
	for (const auto &player : m_players) {
		if (player->getStats()->getHp() > 0) {
			Inventory::useItem(player, buffId);
			player->sendMap(EffectPacket::sendMobItemBuffEffect(player->getId(), buffId));
		}
	}
}

auto Map::gmHideChange(Player *player) -> void {
	if (player->isUsingGmHide()) {
		updateMobControl(player);
		send(MapPacket::removePlayer(player->getId()), player);
	}
	else {
		send(MapPacket::playerPacket(player), player);
		for (const auto &kvp : m_mobs) {
			if (auto mob = kvp.second) {
				if (mob->getController() == nullptr && mob->getControlStatus() != MobControlStatus::None) {
					updateMobControl(mob);
				}
			}
		}
	}
}

auto Map::getAllPlayerIds() const -> vector_t<int32_t> {
	auto copy = m_players;
	vector_t<int32_t> ret;
	for (const auto &player : copy) {
		ret.push_back(player->getId());
	}
	return ret;
}

// Reactors
auto Map::addReactor(Reactor *reactor) -> void {
	m_reactors.push_back(reactor);
	reactor->setId(m_reactors.size() - 1 + Map::ReactorStart);
}

auto Map::getReactor(uint32_t id) const -> Reactor * {
	return id < m_reactors.size() ? m_reactors[id] : nullptr;
}

auto Map::getNumReactors() const -> size_t {
	return m_reactors.size();
}

auto Map::removeReactor(uint32_t id) -> void {
	ReactorSpawnInfo &info = m_reactorSpawns[id];
	if (info.time >= 0) {
		// We don't want to respawn -1s, leave that to some script
		time_point_t reactorRespawn = TimeUtilities::getNowWithTimeAdded(seconds_t(info.time));
		m_reactorRespawns.emplace_back(id, reactorRespawn);
	}
}

auto Map::killReactors(bool showPacket) -> void {
	for (const auto &reactor : m_reactors) {
		if (reactor->isAlive()) {
			reactor->kill();
			if (showPacket) {
				send(ReactorPacket::destroyReactor(reactor));
			}
		}
	}
}

// Footholds
auto Map::findFloor(const Pos &pos, Pos &floorPos, int16_t yMod) -> SearchResult {
	// Determines where a drop falls using the footholds data
	// to check the platforms and find the correct one.
	int16_t x = pos.x;
	int16_t y = pos.y + yMod;
	int16_t closestValue = std::numeric_limits<int16_t>::max();
	bool anyFound = false;

	for (const auto &foothold : m_footholds) {
		const Line &line = foothold.line;

		if (line.withinRangeX(x)) {
			int16_t yInterpolation = line.interpolateForY(x);
			if (yInterpolation <= closestValue && yInterpolation >= y) {
				closestValue = yInterpolation;
				anyFound = true;
			}
		}
	}

	if (anyFound) {
		floorPos.x = x;
		floorPos.y = closestValue;
	}

	return anyFound ? SearchResult::Found : SearchResult::NotFound;
}

auto Map::findRandomFloorPos() -> Pos {
	Pos rightBottom = m_info->dimensions.rightBottom();
	Pos leftTop = m_info->dimensions.leftTop();
	if (leftTop.x == 0) {
		leftTop.x = std::numeric_limits<int16_t>::min();
	}
	if (leftTop.y == 0) {
		leftTop.y = std::numeric_limits<int16_t>::max();
	}
	if (rightBottom.x == 0) {
		rightBottom.x = std::numeric_limits<int16_t>::max();
	}
	if (rightBottom.y == 0) {
		rightBottom.y = std::numeric_limits<int16_t>::min();
	}
	return findRandomFloorPos(Rect(leftTop, rightBottom));
}

auto Map::findRandomFloorPos(const Rect &area) -> Pos {
	vector_t<const FootholdInfo *> validFootholds;
	for (const auto &foothold : m_footholds) {
		if (area.containsAnyPartOfLine(foothold.line)) {
			validFootholds.push_back(&foothold);
		}
	}

	Pos leftTop = area.leftTop();
	Pos rightBottom = area.rightBottom();
	auto xGenerate = [&rightBottom, &leftTop]() -> int16_t { return Randomizer::rand<int16_t>(rightBottom.x, leftTop.x); };
	auto yGenerate = [&rightBottom, &leftTop]() -> int16_t { return Randomizer::rand<int16_t>(rightBottom.y, leftTop.y); };

	Pos ret;
	if (validFootholds.size() == 0) {
		// There's no saving this, just use a random point in the area
		ret.x = xGenerate();
		ret.y = yGenerate();
		return ret;
	}

	do {
		int16_t x = xGenerate();
		int16_t y = yGenerate();
		bool anyFound = false;
		int16_t closestValue = std::numeric_limits<int16_t>::max();

		for (const auto &foothold : validFootholds) {
			if (foothold->line.withinRangeX(x)) {
				int16_t yInterpolation = foothold->line.interpolateForY(x);
				if (yInterpolation <= closestValue && yInterpolation >= y) {
					closestValue = yInterpolation;
					anyFound = true;
				}
			}
		}

		if (anyFound) {
			ret.x = x;
			ret.y = closestValue;
			break;
		}
	} while (true);

	return ret;
}

auto Map::getFhAtPosition(const Pos &pos) -> int16_t {
	// TODO FIXME
	// Consider refactoring
	int16_t foothold = 0;
	for (const auto &cur : m_footholds) {
		if (cur.line.contains(pos)) {
			foothold = cur.id;
			break;
		}
	}
	return foothold;
}

// Portals
auto Map::getPortal(const string_t &name) -> PortalInfo * {
	auto portal = m_portals.find(name);
	return portal != std::end(m_portals) ? &portal->second : nullptr;
}

auto Map::getSpawnPoint(int8_t portalId) -> PortalInfo * {
	int8_t id = portalId != -1 ? portalId : Randomizer::rand<int8_t>(m_spawnPoints.size() - 1);
	return &m_spawnPoints[id];
}

auto Map::getNearestSpawnPoint(const Pos &pos) -> PortalInfo * {
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

auto Map::getPortalNames() const -> vector_t<string_t> {
	vector_t<string_t> ret;
	for (const auto &kvp : m_portals) {
		ret.push_back(kvp.first);
	}
	return ret;
}

// NPCs
auto Map::removeNpc(uint32_t index) -> void {
	if (isValidNpcIndex(index)) {
		NpcSpawnInfo npc = m_npcSpawns[index];
		uint32_t id = makeNpcId();
		send(NpcPacket::showNpc(npc, id, false));
		send(NpcPacket::controlNpc(npc, id, false));
		m_npcSpawns.erase(std::begin(m_npcSpawns) + index);
	}
}

auto Map::addNpc(const NpcSpawnInfo &npc) -> int32_t {
	m_npcSpawns.push_back(npc);
	uint32_t id = makeNpcId();
	send(NpcPacket::showNpc(npc, id));
	send(NpcPacket::controlNpc(npc, id));

	if (NpcDataProvider::getInstance().isMapleTv(npc.id)) {
		MapleTvs::getInstance().addMap(this);
	}

	return m_npcSpawns.size() - 1;
}

auto Map::isValidNpcIndex(uint32_t id) const -> bool {
	return id < m_npcSpawns.size();
}

auto Map::getNpc(uint32_t id) const -> NpcSpawnInfo {
	return m_npcSpawns[id];
}

// Mobs
auto Map::spawnMob(int32_t mobId, const Pos &pos, int16_t foothold, ref_ptr_t<Mob> owner, int8_t summonEffect) -> ref_ptr_t<Mob> {
	int32_t id = getObjectId();

	auto mob = make_ref_ptr<Mob>(id, getId(), mobId, summonEffect != 0 ? owner : nullptr, pos, -1, false, foothold, MobControlStatus::Normal);
	if (summonEffect != 0) {
		owner->addSpawn(id, mob);
	}

	m_mobs[id] = mob;
	send(MobsPacket::spawnMob(mob, summonEffect, owner, (owner == nullptr)));
	updateMobControl(mob, true);

	if (Instance *instance = getInstance()) {
		instance->sendMessage(InstanceMessage::MobSpawn, mobId, id, getId());
	}

	return mob;
}

auto Map::spawnMob(int32_t spawnId, const MobSpawnInfo &info) -> ref_ptr_t<Mob> {
	int32_t id = getObjectId();

	ref_ptr_t<Mob> noOwner = nullptr;
	auto mob = make_ref_ptr<Mob>(id, getId(), info.id, noOwner, info.pos, spawnId, info.facesLeft, info.foothold, MobControlStatus::Normal);
	m_mobs[id] = mob;
	send(MobsPacket::spawnMob(mob, 0, nullptr, true));
	updateMobControl(mob, true);

	if (Instance *instance = getInstance()) {
		instance->sendMessage(InstanceMessage::MobSpawn, info.id, id, getId());
	}

	return mob;
}

auto Map::spawnShell(int32_t mobId, const Pos &pos, int16_t foothold) -> ref_ptr_t<Mob> {
	int32_t id = getObjectId();

	ref_ptr_t<Mob> noOwner = nullptr;
	auto mob = make_ref_ptr<Mob>(id, getId(), mobId, noOwner, pos, -1, false, foothold, MobControlStatus::None);
	m_mobs[id] = mob;
	updateMobControl(mob, true);

	if (Instance *instance = getInstance()) {
		instance->sendMessage(InstanceMessage::MobSpawn, mobId, id, getId());
	}

	return mob;
}

auto Map::getMob(int32_t mapMobId) -> ref_ptr_t<Mob> {
	auto kvp = m_mobs.find(mapMobId);
	return kvp != std::end(m_mobs) ? kvp->second : nullptr;
}

auto Map::updateMobControl(Player *player) -> void {
	for (const auto &kvp : m_mobs) {
		if (auto mob = kvp.second) {
			if (mob->getController() == player) {
				updateMobControl(mob);
			}
		}
	}
}

auto Map::updateMobControl(ref_ptr_t<Mob> mob, bool spawn, Player *display) -> void {
	Player *newController = nullptr;
	Player *oldController = mob->getController();
	if (mob->getControlStatus() != MobControlStatus::None) {
		newController = findController(mob);
	}
	if (newController != oldController) {
		mob->endControl();
	}
	mob->setController(newController, spawn, display);
}

auto Map::switchController(ref_ptr_t<Mob> mob, Player *newController) -> void {
	Player *oldController = mob->getController();
	mob->endControl();
	mob->setController(newController, false, nullptr);
}

auto Map::findController(ref_ptr_t<Mob> mob) -> Player * {
	int32_t maxPos = 200000;
	Player *controller = nullptr;
	for (const auto &player : m_players) {
		if (!player->isUsingGmHide()) {
			int32_t curPos = mob->getPos() - player->getPos();
			if (curPos < maxPos) {
				maxPos = curPos;
				controller = player;
			}
		}
	}
	return controller;
}

auto Map::mobDeath(ref_ptr_t<Mob> mob, bool fromExplosion) -> void {
	auto kvp = m_mobs.find(mob->getMapMobId());
	if (kvp != std::end(m_mobs)) {
		int32_t mapMobId = kvp->first;
		int32_t mobId = mob->getMobId();
		if (Instance *instance = getInstance()) {
			instance->sendMessage(InstanceMessage::MobDeath, mobId, mapMobId, m_id);
		}

		if (mob->hasStatus(StatusEffects::Mob::ShadowWeb)) {
			removeWebbedMob(mapMobId);
		}
	
		if (mob->isSponge()) {
			for (const auto &kvp : mob->m_spawns) {
				if (auto spawn = kvp.second.lock()) {
					spawn->kill();
				}
			}
		}
		else {
			switch (mobId) {
				case Mobs::ZakumArm1:
				case Mobs::ZakumArm2:
				case Mobs::ZakumArm3:
				case Mobs::ZakumArm4:
				case Mobs::ZakumArm5:
				case Mobs::ZakumArm6:
				case Mobs::ZakumArm7:
				case Mobs::ZakumArm8:
					if (auto owner = mob->m_owner.lock()) {
						if (owner->m_spawns.size() == 1) {
							// Last linked arm is dying
							owner->m_controlStatus = MobControlStatus::Normal;
							convertShellToNormal(owner);
						}
					}
					break;
			}
		}

		// Spawn death mobs
		if (Mob::spawnsSponge(mobId)) {
			// Special logic to keep units linked
			ref_ptr_t<Mob> sponge = nullptr;
			vector_t<ref_ptr_t<Mob>> parts;
			for (const auto &summonId : mob->m_info->summon) {
				if (Mob::isSponge(summonId)) {
					sponge = spawnMob(summonId, mob->getPos(), mob->getFoothold(), mob);
				}
				else {
					parts.push_back(spawnMob(summonId, mob->getPos(), mob->getFoothold(), mob));
				}
			}
	
			for (const auto &part : parts) {
				part->m_sponge = sponge;
				sponge->addSpawn(part->getMapMobId(), part);
			}
		}
		else if (auto sponge = mob->getSponge().lock()) {
			// More special logic to keep units linked
			sponge->m_spawns.erase(mapMobId);
			for (const auto &summonId : mob->m_info->summon) {
				auto spawn = spawnMob(summonId, mob->getPos(), mob->getFoothold(), mob);
				sponge->addSpawn(mapMobId, spawn);
			}
		}
		else {
			for (const auto &summonId : mob->m_info->summon) {
				spawnMob(summonId, mob->getPos(), mob->getFoothold(), mob);
			}
		}

		if (mob->m_spawns.size() > 0) {
			for (const auto &kvp : mob->m_spawns) {
				if (auto spawn = kvp.second.lock()) {
					spawn->m_owner.reset();
				}
			}
		}

		if (auto owner = mob->m_owner.lock()) {
			owner->m_spawns.erase(mapMobId);
		}


		send(MobsPacket::dieMob(mapMobId, fromExplosion ? 4 : 1));
		if (mob->m_info->buff != 0) {
			buffPlayers(mob->m_info->buff);
		}

		int32_t spawnId = mob->getSpawnId();
		if (spawnId >= 0) {
			MobSpawnInfo &spawn = m_mobSpawns[spawnId];
			if (spawn.time != -1) {
				// Add spawn point to respawns if mob was spawned by a spawn point
				// Randomly spawn between 1x and 2x the spawn time
				seconds_t timeModifier = seconds_t(spawn.time * (Randomizer::rand<int32_t>(200, 100)) / 100);
				time_point_t spawnTime = TimeUtilities::getNowWithTimeAdded<seconds_t>(timeModifier);
				m_mobRespawns.emplace_back(spawnId, spawnTime);
				spawn.spawned = false;
			}
		}
		m_mobs.erase(kvp);

		if (m_timeMob == mapMobId) {
			m_timeMob = 0;
		}
	}
}

auto Map::mobSummonSkillUsed(ref_ptr_t<Mob> mob, const MobSkillLevelInfo * const skill) -> void {
	if (m_mobs.size() > 50) {
		return;
	}

	const Pos &mobPos = mob->getPos();
	Rect area = skill->dimensions.resize(1).move(mobPos);

	for (const auto &spawnId : skill->summons) {
		Pos floor = findRandomFloorPos(area);
		spawnMob(spawnId, floor, 0, mob, skill->summonEffect);
	}
}

auto Map::killMobs(Player *player, int32_t mobId) -> int32_t {
	// Iterator invalidation
	auto mobMap = m_mobs;
	int32_t mobsKilled = 0;
	for (const auto &kvp : mobMap) {
		if (auto mob = kvp.second) {
			if (mobId == 0 || mob->getMobId() == mobId) {
				if (!mob->isSponge()) {
					// Sponges will be taken care of by their parts
					mob->kill();
				}
				mobsKilled++;
			}
		}
	}
	return mobsKilled;
}

auto Map::countMobs(int32_t mobId) -> int32_t {
	// Iterator invalidation
	auto mobMap = m_mobs;
	int32_t mobCount = 0;
	for (const auto &kvp : mobMap) {
		if (auto mob = kvp.second) {
			if ((mobId > 0 && mob->getMobId() == mobId) || mobId == 0) {
				mobCount++;
			}
		}
	}
	return mobCount;
}

auto Map::healMobs(int32_t hp, int32_t mp, const Rect &dimensions) -> void {
	// Iterator invalidation
	auto mobMap = m_mobs;
	for (const auto &kvp : mobMap) {
		if (auto mob = kvp.second) {
			if (dimensions.contains(mob->getPos())) {
				mob->skillHeal(hp, mp);
			}
		}
	}
}

auto Map::statusMobs(vector_t<StatusInfo> &statuses, const Rect &dimensions) -> void {
	// Iterator invalidation
	auto mobMap = m_mobs;
	for (const auto &kvp : mobMap) {
		if (auto mob = kvp.second) {
			if (dimensions.contains(mob->getPos())) {
				mob->addStatus(0, statuses);
			}
		}
	}
}

auto Map::spawnZakum(const Pos &pos, int16_t foothold) -> void {
	auto body = spawnShell(Mobs::ZakumBody1, pos, foothold);

	init_list_t<int32_t> parts = {
		Mobs::ZakumArm1, Mobs::ZakumArm2, Mobs::ZakumArm3,
		Mobs::ZakumArm4, Mobs::ZakumArm5, Mobs::ZakumArm6,
		Mobs::ZakumArm7, Mobs::ZakumArm8
	};

	for (const auto &part : parts) {
		auto spawnedPart = spawnMob(part, pos, foothold);
		spawnedPart->setOwner(body);
		body->addSpawn(spawnedPart->getMapMobId(), spawnedPart);
	}
}

auto Map::convertShellToNormal(ref_ptr_t<Mob> mob) -> void {
	send(MobsPacket::endControlMob(mob->getMapMobId()));
	send(MobsPacket::spawnMob(mob, 0, nullptr));
	updateMobControl(mob);
}

auto Map::addWebbedMob(int32_t mapMobId) -> void {
	m_webbed[mapMobId] = view_ptr_t<Mob>(m_mobs[mapMobId]);
}

auto Map::removeWebbedMob(int32_t mapMobId) -> void {
	m_webbed.erase(mapMobId);
}

auto Map::runFunctionMobs(function_t<void(ref_ptr_t<const Mob>)> func) -> void {
	for (const auto &kvp : m_mobs) {
		func(kvp.second);
	}
}

// Drops
auto Map::addDrop(Drop *drop) -> void {
	owned_lock_t<recursive_mutex_t> l(m_dropsMutex);
	int32_t id = getObjectId();
	drop->setId(id);
	Pos foundPosition = drop->getPos();
	findFloor(foundPosition, foundPosition, -100);
	drop->setPos(foundPosition);
	m_drops[id] = drop;
}

auto Map::removeDrop(int32_t id) -> void {
	owned_lock_t<recursive_mutex_t> l(m_dropsMutex);
	auto drop = m_drops.find(id);
	if (drop != std::end(m_drops)) {
		m_drops.erase(drop);
	}
}

auto Map::getDrop(int32_t id) -> Drop * {
	owned_lock_t<recursive_mutex_t> l(m_dropsMutex);
	auto drop = m_drops.find(id);
	return drop != std::end(m_drops) ? drop->second : nullptr;
}

auto Map::clearDrops(bool showPacket) -> void {
	owned_lock_t<recursive_mutex_t> l(m_dropsMutex);
	auto copy = m_drops;
	for (const auto &drop : copy) {
		drop.second->removeDrop(showPacket);
	}
}

// Seats
auto Map::seatOccupied(int16_t id) -> bool {
	auto seat = m_seats.find(id);
	if (seat == std::end(m_seats)) {
		// Hacking
		return true;
	}
	return seat->second.occupant != nullptr;
}

auto Map::playerSeated(int16_t id, Player *player) -> void {
	auto seat = m_seats.find(id);
	if (seat == std::end(m_seats)) {
		// Hacking
		return;
	}
	seat->second.occupant = player;
}

// Mists
auto Map::addMist(Mist *mist) -> void {
	mist->setId(getMistId());

	if (mist->isPoison() && !mist->isMobMist()) {
		m_poisonMists[mist->getId()] = mist;
	}
	else {
		m_mists[mist->getId()] = mist;
	}

	Timer::Timer::create([this, mist](const time_point_t &now) { this->removeMist(mist); },
		Timer::Id(Timer::Types::MistTimer, mist->getId(), 0),
		getTimers(), seconds_t(mist->getTime()));

	send(MapPacket::spawnMist(mist, false));
}

auto Map::getMist(int32_t id) -> Mist * {
	auto mist = m_mists.find(id);
	if (mist != std::end(m_mists)) {
		return mist->second;
	}
	mist = m_poisonMists.find(id);
	return mist != std::end(m_poisonMists) ? mist->second : nullptr;
}

auto Map::removeMist(Mist *mist) -> void {
	int32_t id = mist->getId();
	if (mist->isPoison() && !mist->isMobMist()) {
		m_poisonMists.erase(id);
	}
	else {
		m_mists.erase(id);
	}
	delete mist;
	send(MapPacket::removeMist(id));
}

auto Map::clearMists(bool showPacket) -> void {
	hash_map_t<int32_t, Mist *> mistlist = m_mists;
	for (const auto &mist : mistlist) {
		removeMist(mist.second);
	}
	mistlist = m_poisonMists;
	for (const auto &mist : mistlist) {
		removeMist(mist.second);
	}
}

// Timer stuff
auto Map::respawn(int8_t types) -> void {
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

auto Map::checkSpawn(time_point_t time) -> void {
	if (duration_cast<seconds_t>(time - m_lastSpawn) < seconds_t(8)) return;

	Respawnable *respawn;

	for (size_t i = 0; i < m_mobRespawns.size(); ++i) {
		respawn = &m_mobRespawns[i];
		if (time > respawn->spawnAt) {
			m_mobSpawns[respawn->spawnId].spawned = true;
			spawnMob(respawn->spawnId, m_mobSpawns[respawn->spawnId]);

			m_mobRespawns.erase(std::begin(m_mobRespawns) + i);
			i--;
		}
	}

	for (size_t i = 0; i < m_reactorRespawns.size(); ++i) {
		respawn = &m_reactorRespawns[i];
		if (time > respawn->spawnAt) {
			m_reactorSpawns[respawn->spawnId].spawned = true;
			getReactor(respawn->spawnId)->restore();

			m_reactorRespawns.erase(std::begin(m_reactorRespawns) + i);
			i--;
		}
	}

	m_lastSpawn = time;
}

auto Map::checkShadowWeb() -> void {
	if (m_webbed.size() > 0) {
		for (const auto &mob : m_webbed) {
			if (auto ptr = mob.second.lock()) {
				ptr->applyWebDamage();
			}
		}
	}
}

auto Map::checkMists() -> void {
	if (m_poisonMists.size() == 0) {
		return;
	}

	Mist *mist = nullptr;

	for (const auto &kvp : m_mobs) {
		auto mob = kvp.second;

		if (mob == nullptr || mob->hasStatus(StatusEffects::Mob::Poison) || mob->getHp() == 1) {
			continue;
		}

		for (const auto &kvp : m_poisonMists) {
			mist = kvp.second;
			if (mist->getArea().contains(mob->getPos())) {
				bool poisoned = MobHandler::handleMobStatus(mist->getOwnerId(), mob, mist->getSkillId(), mist->getSkillLevel(), 0, 0) > 0;
				if (poisoned) {
					// Mob is poisoned, don't need to check any more mists
					break;
				}
			}
		}
	}
}

auto Map::clearDrops(time_point_t time) -> void {
	// Clear drops based on how long they have been in the map
	owned_lock_t<recursive_mutex_t> l(m_dropsMutex);

	time -= minutes_t(3); // Drops disappear after 3 minutes

	hash_map_t<int32_t, Drop *> drops = m_drops;
	for (const auto &kvp : drops) {
		if (Drop *drop = kvp.second) {
			if (drop->getDroppedAtTime() < time) {
				drop->removeDrop();
			}
		}
	}
}

auto Map::mapTick(const time_point_t &now) -> void {
	auto &config = ChannelServer::getInstance().getConfig();
	if (config.mapUnloadTime > 0) {
		// TODO FIXME need more robust handling of instances active when the map goes to unload
		if (m_players.size() > 0 || getInstance() != nullptr) {
			m_emptyMapTicks = 0;
		}
		else {
			m_emptyMapTicks++;
			if (m_emptyMapTicks > config.mapUnloadTime) {
				Maps::unloadMap(getId());
				return;
			}
		}
	}

	checkSpawn(now);
	clearDrops(now);
	checkMists();

	if (TimeUtilities::getSecond() % 3 == 0) {
		checkShadowWeb();
	}
	int32_t dps = m_info->damagePerSecond;
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

auto Map::timeMob(bool firstLoad) -> void {
	int32_t cHour = TimeUtilities::getHour(false);
	TimeMob *tm = getTimeMob();
	if (firstLoad) {
		if (cHour >= tm->startHour && cHour < tm->endHour) {
			Pos p = findRandomFloorPos();
			m_timeMob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0)->getMapMobId();
			send(PlayerPacket::showMessage(tm->message, PlayerPacket::NoticeTypes::Blue));
		}
	}
	else {
		if (cHour == tm->startHour) {
			Pos p = findRandomFloorPos();
			m_timeMob = spawnMob(tm->id, p, getFhAtPosition(p), nullptr, 0)->getMapMobId();
			send(PlayerPacket::showMessage(tm->message, PlayerPacket::NoticeTypes::Blue));
		}
		else if (cHour == tm->endHour && m_timeMob != 0) {
			auto m = getMob(m_timeMob);
			m->kill();
		}
	}
}

auto Map::setMapTimer(const seconds_t &timer) -> void {
	if (timer.count() > 0 && m_timer.count() != 0) {
		throw std::runtime_error("Timer already executing on map " + StringUtilities::lexical_cast<string_t>(getId()));
	}

	m_timer = timer;
	m_timerStart = TimeUtilities::getNow();

	send(MapPacket::showTimer(timer));
	if (timer.count() > 0) {
		Timer::Timer::create([this](const time_point_t &now) { this->setMapTimer(seconds_t(0)); },
			Timer::Id(Timer::Types::MapTimer, getId(), 25),
			getTimers(), timer);
	}
}

auto Map::showObjects(Player *player) -> void {
	// Music
	if (m_music != m_info->defaultMusic) {
		player->send(EffectPacket::playMusic(m_music));
	}

	// MapleTV messengers
	// TODO FIXME api
	if (MapleTvs::getInstance().isMapleTvMap(getId()) && MapleTvs::getInstance().hasMessage()) {
		player->send(MapleTvPacket::showMessage(MapleTvs::getInstance().getCurrentMessage(), MapleTvs::getInstance().getMessageTime()));
	}

	// Players
	for (const auto &mapPlayer : m_players) {
		if (player != mapPlayer && !mapPlayer->isUsingGmHide()) {
			player->send(MapPacket::playerPacket(mapPlayer));
			SummonHandler::showSummons(mapPlayer, player);
			// Bug in global; would be fixed here:
			// Hurricane/Pierce do not display properly if using when someone enters the map
			// Berserk does not display properly either - players[i]->getActiveBuffs()->getBerserk()
		}
	}

	// NPCs
	int32_t i = 0;
	for (const auto &npc : m_npcSpawns) {
		int32_t id = static_cast<int32_t>(i + Map::NpcStart);
		player->send(NpcPacket::showNpc(npc, id));
		player->send(NpcPacket::controlNpc(npc, id));
		i++;
	}

	// Reactors
	for (const auto &reactor : m_reactors) {
		if (reactor->isAlive()) {
			player->send(ReactorPacket::spawnReactor(reactor));
		}
	}

	// Mobs
	for (const auto &kvp : m_mobs) {
		if (auto mob = kvp.second) {
			if (mob->getControlStatus() == MobControlStatus::None) {
				updateMobControl(mob, true, player);
			}
			else {
				player->send(MobsPacket::spawnMob(mob, 0, nullptr, false));
				updateMobControl(mob);
			}
		}
	}

	// Drops
	{
		owned_lock_t<recursive_mutex_t> l(m_dropsMutex);
		for (const auto &kvp : m_drops) {
			if (Drop *drop = kvp.second) {
				drop->showDrop(player);
			}
		}
	}

	// Mists
	for (const auto &kvp : m_mists) {
		if (Mist *mist = kvp.second) {
			player->send(MapPacket::spawnMist(mist, true));
		}
	}

	if (Party *party = player->getParty()) {
		party->showHpBar(player);
		party->receiveHpBar(player);
	}

	if (m_info->clock) {
		time_t rawTime = time(nullptr);
		struct tm *timeInfo = localtime(&rawTime);
		player->send(MapPacket::showClock(timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec));
	}
}

auto Map::send(const PacketBuilder &builder, Player *sender) -> void {
	for (const auto &mapPlayer : m_players) {
		if (mapPlayer != sender) {
			mapPlayer->send(builder);
		}
	}
}

auto Map::send(const SplitPacketBuilder &builder, Player *sender) -> void {
	if (builder.player.getSize() > 0) {
		sender->send(builder.player);
	}

	if (builder.map.getSize() > 0) {
		for (const auto &mapPlayer : m_players) {
			if (mapPlayer != sender) {
				if (!sender->isUsingGmHide()) {
					mapPlayer->send(builder.map);
				}
			}
		}
	}
}

auto Map::createWeather(Player *player, bool adminWeather, int32_t time, int32_t itemId, const string_t &message) -> bool {
	Timer::Id timerId(Timer::Types::WeatherTimer, 0, 0); // Just to check if there's already a weather item running and adding a new one
	if (getTimers()->isTimerRunning(timerId)) {
		// Hacking
		return false;
	}

	send(MapPacket::changeWeather(adminWeather, itemId, message));
	Timer::Timer::create([this, adminWeather](const time_point_t &now) { this->send(MapPacket::changeWeather(adminWeather, 0, "")); },
		timerId, getTimers(), seconds_t(time));
	return true;
}

// Instance
auto Map::endInstance(bool reset) -> void {
	setInstance(nullptr);
	setMusic("default");
	m_mobs.clear();
	clearDrops(false);
	killReactors(false);
	if (reset) {
		respawn();
	}
}
