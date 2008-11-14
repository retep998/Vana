/*
Copyright (C) 2008 Vana Development Team

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
#include "Database.h"
#include "MiscUtilities.h"
#include <string>

using MiscUtilities::atob;
using std::string;

MapDataProvider *MapDataProvider::singleton = 0;

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
	boost::recursive_mutex::scoped_lock l(loadmap_mutex);

	mysqlpp::Query query = Database::getDataDB().query();
	query << "SELECT returnmap, forcedreturn, fieldtype, fieldlimit, mobrate, clock, ship FROM mapdata WHERE mapid = " << mapid;
	mysqlpp::UseQueryResult res = query.use();

	MYSQL_ROW dataRow;
	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : Return Map
		//    1 : Forced Return Map
		//    2 : Field Type
		//    3 : Field Limit
		//    4 : Mob Spawn Rate
		//    5 : Clock
		//    6 : Ship Interval
		MapInfoPtr mapinfo(new MapInfo);
		mapinfo->id = mapid;
		mapinfo->rm = atoi(dataRow[0]);
		mapinfo->forcedReturn = atoi(dataRow[1]);
		mapinfo->fieldType = atoi(dataRow[2]);
		mapinfo->fieldLimit = atoi(dataRow[3]);
		mapinfo->spawnrate = atof(dataRow[4]);
		mapinfo->clock = atob(dataRow[5]);
		mapinfo->shipInterval = atoi(dataRow[6]);
		map = new Map(mapinfo);
	}
	maps[mapid] = map;
	if (map == 0)
		return;

	// Portals
	query << "SELECT id, name, x, y, toid, toname, script, onlyonce FROM mapportaldata WHERE mapid = " << mapid;
	res = query.use();

	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : Portal ID
		//    1 : Name
		//    2 : x
		//    3 : y
		//    4 : To ID
		//    5 : To Name
		//    6 : Script
		//    7 : Only once
		PortalInfo portal;
		portal.id = atoi(dataRow[0]);
		portal.name = dataRow[1];
		portal.pos = Pos(atoi(dataRow[2]), atoi(dataRow[3]));
		portal.toid = atoi(dataRow[4]);
		portal.toname = dataRow[5];
		portal.script = dataRow[6];
		portal.onlyOnce = atob(dataRow[7]);
		map->addPortal(portal);
	}

	// Life [NPCs and Mobs]
	query << "SELECT isnpc, lifeid, x, cy, fh, rx0, rx1, mobtime FROM maplifedata WHERE mapid = " << mapid;
	res = query.use();

	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : Is NPC?
		//    1 : Life ID
		//    2 : x
		//    3 : cy
		//    4 : fh
		//    5 : rx0
		//    6 : rx1
		//    7 : Mob Time
		if (atob(dataRow[0])) {
			NPCSpawnInfo npc;
			npc.id = atoi(dataRow[1]);
			npc.x = atoi(dataRow[2]);
			npc.cy = atoi(dataRow[3]);
			npc.fh = atoi(dataRow[4]);
			npc.rx0 = atoi(dataRow[5]);
			npc.rx1 = atoi(dataRow[6]);
			map->addNPC(npc);
		}
		else {
			MobSpawnInfo spawn;
			spawn.id = atoi(dataRow[1]);
			spawn.pos = Pos(atoi(dataRow[2]), atoi(dataRow[3]));
			spawn.fh = atoi(dataRow[4]);
			spawn.time = atoi(dataRow[7]);
			map->addMobSpawn(spawn);
		}
	}

	// Reactors
	query << "SELECT reactorid, x, y, reactortime FROM mapreactordata WHERE mapid = " << mapid;
	res = query.use();

	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : Reactor ID
		//    1 : x
		//    2 : y
		//    3 : Reactor Time
		ReactorSpawnInfo reactor;
		reactor.id = atoi(dataRow[0]);
		reactor.pos = Pos(atoi(dataRow[1]), atoi(dataRow[2]));
		reactor.time = atoi(dataRow[3]);
		map->addReactorSpawn(reactor);
	}

	// Footholds
	query << "SELECT x1, y1, x2, y2 FROM mapfootholddata WHERE mapid = " << mapid;
	res = query.use();

	while ((dataRow = res.fetch_raw_row())) {
		// Col0 : x1
		//    1 : y1
		//    2 : x2
		//    3 : y2
		//    4 : Previous
		//    5 : Next
		FootholdInfo foot;
		foot.pos1 = Pos(atoi(dataRow[0]), atoi(dataRow[1]));
		foot.pos2 = Pos(atoi(dataRow[2]), atoi(dataRow[3]));
		map->addFoothold(foot);
	}
}
