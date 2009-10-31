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
#include "MapDataProvider.h"
#include "Map.h"
#include "MapleTvs.h"
#include "MapObjects.h"
#include "Database.h"
#include "StringUtilities.h"

using StringUtilities::runFlags;

MapDataProvider * MapDataProvider::singleton = 0;

MapDataProvider::MapDataProvider() {
	loadData();
}

Map * MapDataProvider::getMap(int32_t mapid) {
	if (maps.find(mapid) != maps.end()) {
		return maps[mapid];
	}
	else {
		Map *map = 0;
		loadMap(mapid, map);
		return map;
	}
}

void MapDataProvider::loadData() {
	continents.clear();
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_continent_data";
	mysqlpp::UseQueryResult res = query.use();
	int8_t mapcluster;
	int8_t continent;

	enum ContinentData {
		MapCluster = 0,
		ContinentId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		mapcluster = atoi(row[MapCluster]);
		continent = atoi(row[ContinentId]);

		continents.insert(continent_info(mapcluster, continent));
	}
}

void MapDataProvider::loadMap(int32_t mapid, Map *&map) {
	boost::mutex::scoped_lock l(loadmap_mutex);

	int32_t checkmap = loadMapData(mapid, map);
	if (checkmap != -1) {
		loadSeats(map, checkmap);
		loadPortals(map, checkmap);
		loadMapLife(map, checkmap);
		loadFootholds(map, checkmap);
		loadMapTimeMob(map);
	}
}

namespace Functors {
	struct MapFlags {
		void operator()(const string &cmp) {
			if (cmp == "town") map->town = true;
			else if (cmp == "clock") map->clock = true;
			else if (cmp == "swim") map->swim = true;
			else if (cmp == "fly") map->fly = true;
			else if (cmp == "everlast") map->everlast = true;
			else if (cmp == "no_party_leader_pass") map->noLeaderPass = true;
			else if (cmp == "shop") map->shop = true;
			else if (cmp == "scroll_disable") map->scrollDisable = true;
			else if (cmp == "shuffle_reactors") map->shuffleReactors = true;
		}
		MapInfoPtr map;
	};
	struct FieldTypeFlags {
		void operator()(const string &cmp) {
			if (cmp == "force_map_equip") map->forceMapEquip = true;
		}
		MapInfoPtr map;
	};
}

int32_t MapDataProvider::loadMapData(int32_t mapid, Map *&map) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT *, (field_limitations + 0) FROM map_data WHERE mapid = " << mapid;
	mysqlpp::UseQueryResult res = query.use();
	int32_t link = 0;

	using namespace Functors;

	enum MapColumns {
		MapId = 0,
		Flags, ShuffleName, Bgm, MinLevel, TimeLimit,
		RegenRate, Traction, LTX, LTY, RBX,
		RBY, ReturnMap, ForcedReturn, FieldType, Unused,
		DecHp, Dps, ProtectItem, MobRate, Link,
		FieldLimit
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		MapInfoPtr mapinfo(new MapInfo);
		link = atoi(row[Link]);
		mapinfo->link = link;

		FieldTypeFlags f = {mapinfo};
		MapFlags whoo = {mapinfo};
		runFlags(row[FieldType], f);
		runFlags(row[Flags], whoo);

		mapinfo->continent = getContinent(mapid);
		mapinfo->rm = atoi(row[ReturnMap]);
		mapinfo->forcedReturn = atoi(row[ForcedReturn]);
		mapinfo->fieldLimit = atoi(row[FieldLimit]);
		mapinfo->spawnRate = atof(row[MobRate]);
		mapinfo->defaultMusic = row[Bgm];
		mapinfo->lt = Pos(atoi(row[LTX]), atoi(row[LTY]));
		mapinfo->rb = Pos(atoi(row[RBX]), atoi(row[RBY]));
		mapinfo->shuffleName = row[ShuffleName];
		mapinfo->decHp = atoi(row[DecHp]);
		mapinfo->dps = atoi(row[Dps]);
		mapinfo->traction = atof(row[Traction]);
		mapinfo->regenRate = atoi(row[RegenRate]);
		mapinfo->minLevel = atoi(row[MinLevel]);
		mapinfo->timeLimit = atoi(row[TimeLimit]);
		mapinfo->protectItem = atoi(row[ProtectItem]);

		map = new Map(mapinfo, mapid);
	}

	maps[mapid] = map;
	if (map == 0) // Map does not exist, so no need to run the rest of the code
		return -1;
	return (link == 0 ? mapid : link);
}

void MapDataProvider::loadSeats(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * from map_seats WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	SeatInfo chair;
	int16_t id;

	enum Seats {
		MapId = 0,
		Id, X, Y
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[Id]);
		chair.pos = Pos(atoi(row[X]), atoi(row[Y]));

		map->addSeat(id, chair);
	}
}

namespace Functors {
	struct PortalFlags {
		void operator()(const string &cmp) {
			if (cmp == "only_once") p->onlyOnce = true;
		}
		PortalInfo *p;
	};
}

void MapDataProvider::loadPortals(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_portals WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	PortalInfo portal;

	using namespace Functors;

	enum Portals {
		MapId = 0,
		Id, Name, X, Y, ToMap,
		ToName, Script, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		portal = PortalInfo();
		
		PortalFlags whoo = {&portal};
		runFlags(row[Flags], whoo);

		portal.id = atoi(row[Id]);
		portal.name = row[Name];
		portal.pos = Pos(atoi(row[X]), atoi(row[Y]));
		portal.toMap = atoi(row[ToMap]);
		portal.toName = row[ToName];
		portal.script = row[Script];

		map->addPortal(portal);
	}
}

namespace Functors {
	struct LifeFlags {
		void operator()(const string &cmp) {
			if (cmp == "faces_left") life->facesRight = false;
		}
		SpawnInfo *life;
	};
}

void MapDataProvider::loadMapLife(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_life WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	NpcSpawnInfo npc;
	MobSpawnInfo spawn;
	ReactorSpawnInfo reactor;
	SpawnInfo life;
	string type;
	Pos pos;

	using namespace Functors;

	enum MapLife {
		Id = 0,
		MapId, LifeType, LifeId, Name, X,
		Y, Foothold, MinClickPos, MaxClickPos,
		RespawnTime, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		life = SpawnInfo();
		LifeFlags flags = {&life};
		runFlags(row[Flags], flags);

		type = row[LifeType];
		life.id = atoi(row[LifeId]);
		life.pos = Pos(atoi(row[X]), atoi(row[Y]));
		life.foothold = atoi(row[Foothold]);
		life.time = atoi(row[RespawnTime]);

		if (type == "npc") {
			npc.setSpawnInfo(life);
			npc.rx0 = atoi(row[MinClickPos]);
			npc.rx1 = atoi(row[MaxClickPos]);

			map->addNpc(npc);
		}
		else if (type == "mob") {
			spawn.setSpawnInfo(life);

			map->addMobSpawn(spawn);
		}
		else if (type == "reactor") {
			reactor.setSpawnInfo(life);
			reactor.name = (row[Name] != 0 ? row[Name] : "");

			map->addReactorSpawn(reactor);
		}
	}
}

namespace Functors {
	struct FootholdFlags {
		void operator()(const string &cmp) {
			if (cmp == "forbid_downward_jump") foot->forbidJumpDown = true;
		}
		FootholdInfo *foot;
	};
}


void MapDataProvider::loadFootholds(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_footholds WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	FootholdInfo foot;

	using namespace Functors;

	enum Footholds {
		MapId = 0,
		Id, X1, Y1, X2, Y2,
		DragForce, Prev, Next, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		foot = FootholdInfo();
		FootholdFlags whoo = {&foot};
		runFlags(row[Flags], whoo);

		foot.id = atoi(row[Id]);
		foot.pos1 = Pos(atoi(row[X1]), atoi(row[Y1]));
		foot.pos2 = Pos(atoi(row[X2]), atoi(row[Y2]));
		foot.dragForce = atoi(row[DragForce]);
		foot.next = atoi(row[Next]);
		foot.prev = atoi(row[Prev]);
		map->addFoothold(foot);
	}
}

void MapDataProvider::loadMapTimeMob(Map *map) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_time_mob WHERE mapid = " << map->getId();
	mysqlpp::UseQueryResult res = query.use();

	enum TimeMobFields {
		MapId = 0,
		MobId, StartHour, EndHour, Message
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		TimeMobPtr info(new TimeMob);

		info->id = atoi(row[MobId]);
		info->startHour = atoi(row[StartHour]);
		info->endHour = atoi(row[EndHour]);
		info->message = row[Message];

		map->addTimeMob(info);
	}
}

int8_t MapDataProvider::getContinent(int32_t mapid) {
	int8_t cluster = static_cast<int8_t>(mapid / 10000000); // Leave first two digits, that's our "map cluster"
	try {
		return continents.left.at(cluster);
	}
	catch (std::out_of_range) {
		std::cout << "Attempted to get a continent ID that does not exist for mapid " << mapid << std::endl;
	}
	return 0;
}