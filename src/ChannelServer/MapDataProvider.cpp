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
#include "MapleTVs.h"
#include "Database.h"
#include "MiscUtilities.h"
#include "StringUtilities.h"
#include <string>

using MiscUtilities::atob;
using StringUtilities::runFlags;
using std::string;

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
	if (checkmap != 0) {
		loadSeats(map, checkmap);
		loadPortals(map, checkmap);
		loadMapLife(map, checkmap);
		loadFootholds(map, checkmap);
	}
}

int32_t MapDataProvider::loadMapData(int32_t mapid, Map *&map) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT *, (field_limitations + 0) FROM map_data WHERE mapid = " << mapid;
	mysqlpp::UseQueryResult res = query.use();
	int32_t link = 0;

	struct Functor {
		void operator()(const string &cmp) {
			if (cmp == "town") map->town = true;
			else if (cmp == "clock") map->clock = true;
			else if (cmp == "swim") map->swim = true;
			else if (cmp == "fly") map->fly = true;
			else if (cmp == "everlast") map->everlast = true;
			else if (cmp == "no_party_leader_pass") map->nopartyleaderpass = true;
			else if (cmp == "shop") map->shop = true;
			else if (cmp == "scroll_disable") map->scrolldisable = true;
			else if (cmp == "shuffle_reactors") map->shufflereactors = true;
		}
		MapInfoPtr map;
	};
	struct FieldTypeFunctor {
		void operator()(const string &cmp) {
			if (cmp == "force_map_equip") map->forcemapequip = true;
		}
		MapInfoPtr map;
	};

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
		mapinfo->id = mapid;

		FieldTypeFunctor f = {mapinfo};
		Functor whoo = {mapinfo};
		runFlags(row[FieldType], f);
		runFlags(row[Flags], whoo);

		mapinfo->continent = getContinent(mapid);
		mapinfo->rm = atoi(row[ReturnMap]);
		mapinfo->forcedReturn = atoi(row[ForcedReturn]);
		mapinfo->fieldLimit = atoi(row[FieldLimit]);
		mapinfo->spawnrate = atof(row[MobRate]);
		mapinfo->defaultmusic = row[Bgm];
		mapinfo->musicname = row[Bgm];
		mapinfo->lt = Pos(atoi(row[LTX]), atoi(row[LTY]));
		mapinfo->rb = Pos(atoi(row[RBX]), atoi(row[RBY]));
		mapinfo->shufflename = row[ShuffleName];
		mapinfo->dechp = atoi(row[DecHp]);
		mapinfo->dps = atoi(row[Dps]);
		mapinfo->traction = atof(row[Traction]);
		mapinfo->regenrate = atoi(row[RegenRate]);
		mapinfo->minlevel = atoi(row[MinLevel]);
		mapinfo->timelimit = atoi(row[TimeLimit]);
		mapinfo->protectitem = atoi(row[ProtectItem]);

		map = new Map(mapinfo);
	}

	maps[mapid] = map;
	if (map == 0) // Map does not exist, so no need to run the rest of the code
		return 0;
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

void MapDataProvider::loadPortals(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_portals WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	PortalInfo portal;

	struct FlagFunctor {
		void operator()(const string &cmp) {
			if (cmp == "only_once") p->onlyOnce = true;
		}
		PortalInfo *p;
	};

	enum Portals {
		MapId = 0,
		Id, Name, X, Y, ToMap,
		ToName, Script, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		portal = PortalInfo();
		
		FlagFunctor whoo = {&portal};
		runFlags(row[Flags], whoo);

		portal.id = atoi(row[Id]);
		portal.name = row[Name];
		portal.pos = Pos(atoi(row[X]), atoi(row[Y]));
		portal.tomap = atoi(row[ToMap]);
		portal.toname = row[ToName];
		portal.script = row[Script];

		map->addPortal(portal);
	}
}

void MapDataProvider::loadMapLife(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_life WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	NPCSpawnInfo npc;
	MobSpawnInfo spawn;
	ReactorSpawnInfo reactor;
	string type;
	Pos pos;

	struct NpcFlags {
		void operator()(const string &cmp) {
			if (cmp == "faces_left") npc->facesleft = true;
		}
		NPCSpawnInfo *npc;
	};
	struct MobFlags {
		void operator()(const string &cmp) {
			if (cmp == "faces_left") mob->facesleft = true;
		}
		MobSpawnInfo *mob;
	};
	struct ReactorFlags {
		void operator()(const string &cmp) {
			if (cmp == "faces_left") reactor->facesleft = true;
		}
		ReactorSpawnInfo *reactor;
	};

	enum MapLife {
		Id = 0,
		MapId, LifeType, LifeId, Name, X,
		Y, Foothold, MinClickPos, MaxClickPos,
		RespawnTime, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		type = row[LifeType];
		pos = Pos(atoi(row[X]), atoi(row[Y]));

		if (type == "npc") {
			npc = NPCSpawnInfo();

			NpcFlags flags = {&npc};
			runFlags(row[Flags], flags);

			npc.id = atoi(row[LifeId]);
			npc.pos = pos;
			npc.fh = atoi(row[Foothold]);
			npc.rx0 = atoi(row[MinClickPos]);
			npc.rx1 = atoi(row[MaxClickPos]);
			map->addNPC(npc);
		}
		else if (type == "mob") {
			spawn = MobSpawnInfo();

			MobFlags flags = {&spawn};
			runFlags(row[Flags], flags);

			spawn.id = atoi(row[LifeId]);
			spawn.pos = pos;
			spawn.fh = atoi(row[Foothold]);
			spawn.time = atoi(row[RespawnTime]);
			map->addMobSpawn(spawn);
		}
		else if (type == "reactor") {
			reactor = ReactorSpawnInfo();

			ReactorFlags flags = {&reactor};
			runFlags(row[Flags], flags);

			reactor.id = atoi(row[LifeId]);
			reactor.pos = pos;
			reactor.time = atoi(row[RespawnTime]);
			map->addReactorSpawn(reactor);
		}
	}
}

void MapDataProvider::loadFootholds(Map *map, int32_t link) {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_footholds WHERE mapid = " << link;
	mysqlpp::UseQueryResult res = query.use();
	FootholdInfo foot;

	struct Functor {
		void operator()(const string &cmp) {
			if (cmp == "forbid_downward_jump") foot->forbidjumpdown = true;
		}
		FootholdInfo *foot;
	};
	enum Footholds {
		MapId = 0,
		Id, X1, Y1, X2, Y2,
		DragForce, Prev, Next, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		foot = FootholdInfo();
		Functor whoo = {&foot};
		runFlags(row[Flags], whoo);

		foot.id = atoi(row[Id]);
		foot.pos1 = Pos(atoi(row[X1]), atoi(row[Y1]));
		foot.pos2 = Pos(atoi(row[X2]), atoi(row[Y2]));
		foot.dragforce = atoi(row[DragForce]);
		foot.next = atoi(row[Next]);
		foot.prev = atoi(row[Prev]);
		map->addFoothold(foot);
	}
}

void MapDataProvider::loadMapTimeMob(MapInfoPtr info) {
	// TODO: Fix this, MCDB 4.0 broke it
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM map_time_mob WHERE mapid = " << info->id;
	mysqlpp::UseQueryResult res = query.use();

	enum Footholds {
		MapId = 0,
		MobId, StartHour, EndHour, Message
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		info->timemob = atoi(row[MobId]);
		info->starthour = atoi(row[StartHour]);
		info->endhour = atoi(row[EndHour]);
		info->message = row[Message];
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