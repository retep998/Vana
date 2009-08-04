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
#include <string>

using MiscUtilities::atob;
using std::string;

MapDataProvider * MapDataProvider::singleton = 0;

MapDataProvider::MapDataProvider() {
	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM continentdata";
	mysqlpp::UseQueryResult res = query.use();
	MYSQL_ROW dataRow;
	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Map cluster ID
		//    1 : Continent ID
		int8_t mapcluster = atoi(dataRow[0]);
		int8_t continent = atoi(dataRow[1]);
		continents.insert(continent_info(mapcluster, continent));
	}
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

void MapDataProvider::loadMap(int32_t mapid, Map *&map) {
	boost::mutex::scoped_lock l(loadmap_mutex);

	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT * FROM mapdata WHERE mapid = " << mapid;
	mysqlpp::UseQueryResult res = query.use();

	int32_t link = 0;
	MYSQL_ROW dataRow;
	enum MapColumns {
		MapId = 0,
		Top, Left, Right, Bottom, ReturnMap,
		ForcedReturn, FieldType, FieldLimit, DecHp, ProtectItem,
		TimeMob, StartHour, EndHour, Message, MobRate,
		Town, Clock, Ship, Link
	};
	while (dataRow = res.fetch_raw_row()) {
		link = atoi(dataRow[Link]);

		MapInfoPtr mapinfo(new MapInfo);

		mapinfo->link = link;
		mapinfo->id = mapid;

		mapinfo->continent = getContinent(mapid);
		mapinfo->rm = atoi(dataRow[ReturnMap]);
		mapinfo->forcedReturn = atoi(dataRow[ForcedReturn]);
		mapinfo->fieldType = atoi(dataRow[FieldType]);
		mapinfo->fieldLimit = atoi(dataRow[FieldLimit]);
		mapinfo->spawnrate = atof(dataRow[MobRate]);
		mapinfo->clock = atob(dataRow[Clock]);
		mapinfo->shipInterval = atoi(dataRow[Ship]);
		mapinfo->town = atob(dataRow[Town]);
		mapinfo->top = atoi(dataRow[Top]);
		mapinfo->right = atoi(dataRow[Right]);
		mapinfo->left = atoi(dataRow[Left]);
		mapinfo->bottom = atoi(dataRow[Bottom]);
		mapinfo->timemob = atoi(dataRow[TimeMob]);
		mapinfo->starthour = atoi(dataRow[StartHour]);
		mapinfo->endhour = atoi(dataRow[EndHour]);
		mapinfo->message = dataRow[Message];

		map = new Map(mapinfo);
	}

	maps[mapid] = map;
	if (map == 0) // Map does not exist, so no need to run the rest of the code
		return;

	int32_t checkmap = (link == 0 ? mapid : link);

	// Seats
	query << "SELECT seatid, x, y from mapseatdata WHERE mapid = " << checkmap;
	res = query.use();
	SeatInfo chair;

	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Seat ID
		//    1 : x
		//    2 : y

		int16_t id = atoi(dataRow[0]);
		chair.pos = Pos(atoi(dataRow[1]), atoi(dataRow[2]));
		chair.occupant = 0;
		map->addSeat(id, chair);
	}

	// Portals
	query << "SELECT id, name, x, y, tomap, toname, script, onlyonce FROM mapportaldata WHERE mapid = " << checkmap;
	res = query.use();
	PortalInfo portal;

	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Portal ID
		//    1 : Name
		//    2 : x
		//    3 : y
		//    4 : To Map
		//    5 : To Name
		//    6 : Script
		//    7 : Only once

		portal.id = atoi(dataRow[0]);
		portal.name = dataRow[1];
		portal.pos = Pos(atoi(dataRow[2]), atoi(dataRow[3]));
		portal.tomap = atoi(dataRow[4]);
		portal.toname = dataRow[5];
		portal.script = dataRow[6];
		portal.onlyOnce = atob(dataRow[7]);
		map->addPortal(portal);
	}

	// Life [NPCs and Mobs]
	query << "SELECT isnpc, lifeid, x, cy, fh, rx0, rx1, mobtime, facesright FROM maplifedata WHERE mapid = " << checkmap;
	res = query.use();
	NPCSpawnInfo npc;
	MobSpawnInfo spawn;

	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Is NPC?
		//    1 : Life ID
		//    2 : x
		//    3 : cy
		//    4 : fh
		//    5 : rx0
		//    6 : rx1
		//    7 : Mob Time
		//    8 : Faces Right

		if (atob(dataRow[0])) {
			npc.id = atoi(dataRow[1]);
			npc.pos = Pos(atoi(dataRow[2]), atoi(dataRow[3]));
			npc.fh = atoi(dataRow[4]);
			npc.rx0 = atoi(dataRow[5]);
			npc.rx1 = atoi(dataRow[6]);
			npc.facingside = (atoi(dataRow[8]) == 1 ? 0 : 1);
			map->addNPC(npc);
			if (MapleTVs::Instance()->isMapleTVNPC(npc.id))
				MapleTVs::Instance()->addMap(map);
		}
		else {
			spawn.id = atoi(dataRow[1]);
			spawn.pos = Pos(atoi(dataRow[2]), atoi(dataRow[3]));
			spawn.fh = atoi(dataRow[4]);
			spawn.time = atoi(dataRow[7]);
			spawn.facingside = (atoi(dataRow[8]) == 1 ? 0 : 1);
			map->addMobSpawn(spawn);
		}
	}

	// Reactors
	query << "SELECT reactorid, x, y, reactortime, link FROM mapreactordata WHERE mapid = " << checkmap;
	res = query.use();
	ReactorSpawnInfo reactor;

	while (dataRow = res.fetch_raw_row()) {
		// Col0 : Reactor ID
		//    1 : x
		//    2 : y
		//    3 : Reactor Time
		//    4 : Link

		reactor.id = atoi(dataRow[0]);
		reactor.pos = Pos(atoi(dataRow[1]), atoi(dataRow[2]));
		reactor.time = atoi(dataRow[3]);
		reactor.link = atoi(dataRow[4]);
		map->addReactorSpawn(reactor);
	}

	// Footholds
	query << "SELECT id, x1, y1, x2, y2 FROM mapfootholddata WHERE mapid = " << checkmap;
	res = query.use();
	FootholdInfo foot;

	while (dataRow = res.fetch_raw_row()) {
		// Col0 : id
		//    1 : x1
		//    2 : y1
		//    3 : x2
		//    4 : y2

		foot.id = atoi(dataRow[0]) - 1;
		foot.pos1 = Pos(atoi(dataRow[1]), atoi(dataRow[2]));
		foot.pos2 = Pos(atoi(dataRow[3]), atoi(dataRow[4]));
		map->addFoothold(foot);
	}
}

int8_t MapDataProvider::getContinent(int32_t mapid) {
	int8_t cluster = static_cast<int8_t>(mapid / 10000000); // Leave first two digits, that's our "map cluster"
	try {
		return continents.left.at(cluster);
	}
	catch (std::out_of_range) {

	}
	return 0;
}