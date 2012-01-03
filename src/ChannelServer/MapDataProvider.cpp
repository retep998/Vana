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
#include "MapDataProvider.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "Map.h"
#include "MapleTvs.h"
#include "MapObjects.h"
#include "StringUtilities.h"

using StringUtilities::runFlags;

MapDataProvider * MapDataProvider::singleton = nullptr;

FieldLimit::FieldLimit() :
	jump(false),
	movementSkills(false),
	summoningBag(false),
	mysticDoor(false),
	channelSwitching(false),
	regularExpLoss(false),
	vipRock(false),
	minigames(false),
	mount(false),
	potionUse(false),
	dropDown(false),
	chalkboard(false)
{
}

MapDataProvider::MapDataProvider()
{
	loadData();
}

Map * MapDataProvider::getMap(int32_t mapId) {
	if (m_maps.find(mapId) != m_maps.end()) {
		return m_maps[mapId];
	}
	else {
		Map *map = nullptr;
		loadMap(mapId, map);
		return map;
	}
}

void MapDataProvider::loadData() {
	m_continents.clear();
	int8_t mapCluster;
	int8_t continent;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM map_continent_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		mapCluster = row.get<int8_t>("map_cluster");
		continent = row.get<int8_t>("continent");

		m_continents.insert(continent_info(mapCluster, continent));
	}
}

void MapDataProvider::loadMap(int32_t mapId, Map *&map) {
	boost::mutex::scoped_lock l(m_loadMutex);

	int32_t checkMap = loadMapData(mapId, map);
	if (checkMap != -1) {
		loadSeats(map, checkMap);
		loadPortals(map, checkMap);
		loadMapLife(map, checkMap);
		loadFootholds(map, checkMap);
		loadMapTimeMob(map);
	}
}

int32_t MapDataProvider::loadMapData(int32_t mapId, Map *&map) {
	int32_t link = 0;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT *, (field_limitations + 0) FROM map_data WHERE mapid = :map", soci::use(mapId, "map"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		MapInfoPtr mapInfo(new MapInfo);
		link = row.get<int32_t>("link");
		mapInfo->link = link;

		runFlags(row.get<opt_string>("flags"), [&mapInfo](const string &cmp) {
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

		runFlags(row.get<opt_string>("field_type"), [&mapInfo](const string &cmp) {
			if (cmp == "force_map_equip") mapInfo->forceMapEquip = true;
		});

		runFlags(row.get<opt_string>("field_limitations"), [&mapInfo](const string &cmp) {
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

		mapInfo->continent = getContinent(mapId);
		mapInfo->rm = row.get<int32_t>("return_map");
		mapInfo->forcedReturn = row.get<int32_t>("forced_return_map");
		mapInfo->spawnRate = row.get<double>("mob_rate");
		mapInfo->defaultMusic = row.get<string>("default_bgm");
		mapInfo->lt = Pos(row.get<int16_t>("map_ltx"), row.get<int16_t>("map_lty"));
		mapInfo->rb = Pos(row.get<int16_t>("map_rbx"), row.get<int16_t>("map_rby"));
		mapInfo->shuffleName = row.get<string>("shuffle_name");
		mapInfo->decHp = row.get<uint8_t>("decrease_hp");
		mapInfo->dps = row.get<uint16_t>("damage_per_second");
		mapInfo->traction = row.get<double>("default_traction");
		mapInfo->regenRate = row.get<int8_t>("regen_rate");
		mapInfo->minLevel = row.get<uint8_t>("min_level_limit");
		mapInfo->timeLimit = row.get<int32_t>("time_limit");
		mapInfo->protectItem = row.get<int32_t>("protect_item");
		mapInfo->shipKind = row.get<int8_t>("ship_kind");

		map = new Map(mapInfo, mapId);
	}

	m_maps[mapId] = map;
	if (map == nullptr) {
		return -1;
	}
	return (link == 0 ? mapId : link);
}

void MapDataProvider::loadSeats(Map *map, int32_t link) {
	SeatInfo chair;
	int16_t id;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM map_seats WHERE mapid = :map", soci::use(link, "map"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		id = row.get<int16_t>("seatid");
		chair = SeatInfo();
		chair.pos = Pos(row.get<int16_t>("x_pos"), row.get<int16_t>("y_pos"));

		map->addSeat(id, chair);
	}
}

void MapDataProvider::loadPortals(Map *map, int32_t link) {
	PortalInfo portal;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM map_portals WHERE mapid = :map", soci::use(link, "map"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		portal = PortalInfo();
		runFlags(row.get<opt_string>("flags"), [&portal](const string &cmp) {
			if (cmp == "only_once") portal.onlyOnce = true;
		});

		portal.id = row.get<int32_t>("id");
		portal.name = row.get<string>("label");
		portal.pos = Pos(row.get<int16_t>("x_pos"), row.get<int16_t>("y_pos"));
		portal.toMap = row.get<int32_t>("destination");
		portal.toName = row.get<string>("destination_label");
		portal.script = row.get<string>("script");

		map->addPortal(portal);
	}
}

void MapDataProvider::loadMapLife(Map *map, int32_t link) {
	NpcSpawnInfo npc;
	MobSpawnInfo spawn;
	ReactorSpawnInfo reactor;
	SpawnInfo life;
	string type;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM map_life WHERE mapid = :map", soci::use(link, "map"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		life = SpawnInfo();
		runFlags(row.get<opt_string>("flags"), [&life](const string &cmp) {
			if (cmp == "faces_left") life.facesRight = false;
		});

		type = row.get<string>("life_type");
		life.id = row.get<int32_t>("lifeid");
		life.pos = Pos(row.get<int16_t>("x_pos"), row.get<int16_t>("y_pos"));
		life.foothold = row.get<int16_t>("foothold");
		life.time = row.get<int32_t>("respawn_time");

		if (type == "npc") {
			npc = NpcSpawnInfo();
			npc.setSpawnInfo(life);
			npc.rx0 = row.get<int16_t>("min_click_pos");
			npc.rx1 = row.get<int16_t>("max_click_pos");
			map->addNpc(npc);
		}
		else if (type == "mob") {
			spawn = MobSpawnInfo();
			spawn.setSpawnInfo(life);
			map->addMobSpawn(spawn);
		}
		else if (type == "reactor") {
			reactor = ReactorSpawnInfo();
			reactor.setSpawnInfo(life);
			reactor.name = row.get<string>("life_name");
			map->addReactorSpawn(reactor);
		}
	}
}

void MapDataProvider::loadFootholds(Map *map, int32_t link) {
	FootholdInfo foot;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM map_footholds WHERE mapid = :map", soci::use(link, "map"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		foot = FootholdInfo();
		runFlags(row.get<opt_string>("flags"), [&foot](const string &cmp) {
			if (cmp == "forbid_downward_jump") foot.forbidJumpDown = true;
		});

		foot.id = row.get<int32_t>("id");
		foot.pos1 = Pos(row.get<int16_t>("x1"), row.get<int16_t>("y1"));
		foot.pos2 = Pos(row.get<int16_t>("x2"), row.get<int16_t>("y2"));
		foot.dragForce = row.get<int16_t>("drag_force");
		foot.leftEdge = row.get<int16_t>("previousid") == 0;
		foot.rightEdge = row.get<int16_t>("nextid") == 0;
		map->addFoothold(foot);
	}
}

void MapDataProvider::loadMapTimeMob(Map *map) {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM map_time_mob WHERE mapid = :map", soci::use(map->getId(), "map"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		TimeMobPtr info(new TimeMob);

		info->id = row.get<int32_t>("mobid");
		info->startHour = row.get<int8_t>("start_hour");
		info->endHour = row.get<int8_t>("end_hour");
		info->message = row.get<string>("message");
	}
}

int8_t MapDataProvider::getContinent(int32_t mapId) {
	int8_t cluster = GameLogicUtilities::getMapCluster(mapId);
	try {
		return m_continents.left.at(cluster);
	}
	catch (std::out_of_range) {
		std::cerr << "Attempted to get a continent ID that does not exist for mapId " << mapId << std::endl;
	}
	return 0;
}