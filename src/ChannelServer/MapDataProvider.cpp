/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "MapDataProvider.hpp"
#include "Database.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "Map.hpp"
#include "MapleTvs.hpp"
#include "MapObjects.hpp"
#include "StringUtilities.hpp"
#include <utility>

auto MapDataProvider::getMap(map_id_t mapId) -> Map * {
	if (m_maps.find(mapId) != std::end(m_maps)) {
		return m_maps[mapId];
	}
	else {
		Map *map = nullptr;
		loadMap(mapId, map);
		return map;
	}
}

auto MapDataProvider::unloadMap(map_id_t mapId) -> void {
	auto iter = m_maps.find(mapId);
	if (iter != std::end(m_maps)) {
		delete iter->second;
		m_maps.erase(iter);
	}
}

auto MapDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Continents... ";

	m_continents.clear();
	int8_t mapCluster;
	int8_t continent;

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_continent_data"));

	for (const auto &row : rs) {
		mapCluster = row.get<int8_t>("map_cluster");
		continent = row.get<int8_t>("continent");

		m_continents.emplace(mapCluster, continent);
	}

	std::cout << "DONE" << std::endl;
}

auto MapDataProvider::loadMap(map_id_t mapId, Map *&map) -> void {
	owned_lock_t<mutex_t> l{m_loadMutex};

	map_id_t checkMap = loadMapData(mapId, map);
	if (checkMap != -1) {
		loadSeats(map, checkMap);
		loadPortals(map, checkMap);
		loadMapLife(map, checkMap);
		loadFootholds(map, checkMap);
		loadMapTimeMob(map);
	}
}

auto MapDataProvider::loadMapData(map_id_t mapId, Map *&map) -> map_id_t {
	map_id_t link = 0;

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_data") << " WHERE mapid = :map", soci::use(mapId, "map"));

	for (const auto &row : rs) {
		ref_ptr_t<MapInfo> mapInfo = make_ref_ptr<MapInfo>();
		link = row.get<map_id_t>("link");
		mapInfo->link = link;

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&mapInfo](const string_t &cmp) {
			if (cmp == "town") mapInfo->town = true;
			else if (cmp == "clock") mapInfo->clock = true;
			else if (cmp == "swim") mapInfo->swim = true;
			else if (cmp == "fly") mapInfo->fly = true;
			else if (cmp == "everlast") mapInfo->everlast = true;
			else if (cmp == "no_party_leader_pass") mapInfo->noLeaderPass = true;
			else if (cmp == "shop") mapInfo->shop = true;
			else if (cmp == "scroll_disable") mapInfo->scrollDisable = true;
			else if (cmp == "shuffle_reactors") mapInfo->shuffleReactors = true;
		});

		StringUtilities::runFlags(row.get<opt_string_t>("field_type"), [&mapInfo](const string_t &cmp) {
			if (cmp == "force_map_equip") mapInfo->forceMapEquip = true;
		});

		StringUtilities::runFlags(row.get<opt_string_t>("field_limitations"), [&mapInfo](const string_t &cmp) {
			FieldLimit *limitations = &mapInfo->limitations;
			if (cmp == "jump") limitations->jump = true;
			else if (cmp == "movement_skills") limitations->movementSkills = true;
			else if (cmp == "summoning_bag") limitations->summoningBag = true;
			else if (cmp == "mystic_door") limitations->mysticDoor = true;
			else if (cmp == "channel_switching") limitations->channelSwitching = true;
			else if (cmp == "regular_exp_loss") limitations->regularExpLoss = true;
			else if (cmp == "vip_rock") limitations->vipRock = true;
			else if (cmp == "minigames") limitations->minigames = true;
			else if (cmp == "mount") limitations->mount = true;
			else if (cmp == "potion_use") limitations->potionUse = true;
			else if (cmp == "drop_down") limitations->dropDown = true;
			else if (cmp == "chalkboard") limitations->chalkboard = true;
		});

		mapInfo->continent = getContinent(mapId).get(0);
		mapInfo->returnMap = row.get<map_id_t>("return_map");
		mapInfo->forcedReturn = row.get<map_id_t>("forced_return_map");
		mapInfo->spawnRate = row.get<double>("mob_rate");
		mapInfo->defaultMusic = row.get<string_t>("default_bgm");
		mapInfo->dimensions = Rect{
			Point{row.get<coord_t>("map_ltx"), row.get<coord_t>("map_lty")},
			Point{row.get<coord_t>("map_rbx"), row.get<coord_t>("map_rby")}
		};
		mapInfo->shuffleName = row.get<string_t>("shuffle_name");
		mapInfo->regularHpDecrease = row.get<uint8_t>("decrease_hp");
		mapInfo->traction = row.get<double>("default_traction");
		mapInfo->regenRate = row.get<int8_t>("regen_rate");
		mapInfo->minLevel = row.get<player_level_t>("min_level_limit");
		mapInfo->timeLimit = row.get<int32_t>("time_limit");
		mapInfo->protectItem = row.get<item_id_t>("protect_item");
		mapInfo->damagePerSecond = row.get<damage_t>("damage_per_second");
		mapInfo->shipKind = row.get<int8_t>("ship_kind");

		map = new Map(mapInfo, mapId);
	}

	m_maps[mapId] = map;
	if (map == nullptr) {
		return -1;
	}
	return (link == 0 ? mapId : link);
}

auto MapDataProvider::loadSeats(Map *map, map_id_t link) -> void {

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_seats") << " WHERE mapid = :map", soci::use(link, "map"));

	for (const auto &row : rs) {
		SeatInfo chair;
		seat_id_t id = row.get<seat_id_t>("seatid");
		chair.pos = Point{row.get<coord_t>("x_pos"), row.get<coord_t>("y_pos")};
		chair.id = id;

		map->addSeat(chair);
	}
}

auto MapDataProvider::loadPortals(Map *map, map_id_t link) -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_portals") << " WHERE mapid = :map", soci::use(link, "map"));

	for (const auto &row : rs) {
		PortalInfo portal;
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&portal](const string_t &cmp) {
			if (cmp == "only_once") portal.onlyOnce = true;
		});

		portal.id = row.get<portal_id_t>("id");
		portal.name = row.get<string_t>("label");
		portal.pos = Point{row.get<coord_t>("x_pos"), row.get<coord_t>("y_pos")};
		portal.toMap = row.get<map_id_t>("destination");
		portal.toName = row.get<string_t>("destination_label");
		portal.script = row.get<string_t>("script");

		map->addPortal(portal);
	}
}

auto MapDataProvider::loadMapLife(Map *map, map_id_t link) -> void {
	NpcSpawnInfo npc;
	MobSpawnInfo spawn;
	ReactorSpawnInfo reactor;
	SpawnInfo life;
	string_t type;

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_life") << " WHERE mapid = :map", soci::use(link, "map"));

	for (const auto &row : rs) {
		life = SpawnInfo{};
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&life](const string_t &cmp) {
			if (cmp == "faces_left") life.facesLeft = true;
		});

		type = row.get<string_t>("life_type");
		life.id = row.get<int32_t>("lifeid");
		life.pos = Point{row.get<coord_t>("x_pos"), row.get<coord_t>("y_pos")};
		life.foothold = row.get<foothold_id_t>("foothold");
		life.time = row.get<int32_t>("respawn_time");

		if (type == "npc") {
			npc = NpcSpawnInfo{};
			npc.setSpawnInfo(life);
			npc.rx0 = row.get<coord_t>("min_click_pos");
			npc.rx1 = row.get<coord_t>("max_click_pos");
			map->addNpc(npc);
		}
		else if (type == "mob") {
			spawn = MobSpawnInfo{};
			spawn.setSpawnInfo(life);
			map->addMobSpawn(spawn);
		}
		else if (type == "reactor") {
			reactor = ReactorSpawnInfo{};
			reactor.setSpawnInfo(life);
			reactor.name = row.get<string_t>("life_name");
			map->addReactorSpawn(reactor);
		}
	}
}

auto MapDataProvider::loadFootholds(Map *map, map_id_t link) -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_footholds") << " WHERE mapid = :map", soci::use(link, "map"));

	for (const auto &row : rs) {
		FootholdInfo foot;
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&foot](const string_t &cmp) {
			if (cmp == "forbid_downward_jump") foot.forbidJumpDown = true;
		});

		foot.id = row.get<foothold_id_t>("id");
		foot.line = Line{
			Point{row.get<coord_t>("x1"), row.get<coord_t>("y1")},
			Point{row.get<coord_t>("x2"), row.get<coord_t>("y2")}
		};
		foot.dragForce = row.get<int16_t>("drag_force");
		foot.leftEdge = row.get<foothold_id_t>("previousid") == 0;
		foot.rightEdge = row.get<foothold_id_t>("nextid") == 0;
		map->addFoothold(foot);
	}
}

auto MapDataProvider::loadMapTimeMob(Map *map) -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("map_time_mob") << " WHERE mapid = :map", soci::use(map->getId(), "map"));

	for (const auto &row : rs) {
		ref_ptr_t<TimeMob> info = make_ref_ptr<TimeMob>();

		info->id = row.get<mob_id_t>("mobid");
		info->startHour = row.get<int8_t>("start_hour");
		info->endHour = row.get<int8_t>("end_hour");
		info->message = row.get<string_t>("message");
	}
}

auto MapDataProvider::getContinent(map_id_t mapId) const -> opt_int8_t {
	int8_t cluster = GameLogicUtilities::getMapCluster(mapId);
	auto kvp = m_continents.find(cluster);
	if (kvp == std::end(m_continents)) {
		return {};
	}
	return kvp->second;
}