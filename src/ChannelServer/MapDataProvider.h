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
#ifndef MAPDATAPROVIDER_H
#define MAPDATAPROVIDER_H

#include "Pos.h"
#include "Types.h"
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

using std::string;
using std::tr1::shared_ptr;
using std::tr1::unordered_map;
using std::vector;

class Map;
class Player;

struct MapInfo {
	MapInfo() :
		lt(Pos(0,0)),
		rb(Pos(0,0)),
		timemob(0),
		clock(false),
		town(false),
		swim(false),
		fly(false),
		everlast(false),
		nopartyleaderpass(false),
		shop(false),
		scrolldisable(false),
		shufflereactors(false),
		forcemapequip(false)
		{ }
	bool clock;
	bool town;
	bool swim;
	bool fly;
	bool everlast;
	bool nopartyleaderpass;
	bool shop;
	bool scrolldisable;
	bool shufflereactors;
	bool forcemapequip;
	int8_t continent;
	int8_t starthour;
	int8_t endhour;
	int8_t regenrate;
	uint8_t minlevel;
	uint8_t dechp;
	uint16_t dps;
	int32_t id;
	int32_t rm;
	int32_t forcedReturn;
	int32_t fieldLimit;
	int32_t link;
	int32_t timemob;
	int32_t timelimit;
	int32_t protectitem;
	double spawnrate;
	double traction;
	string musicname;
	string defaultmusic;
	string shufflename;
	string message;
	Pos lt;
	Pos rb;
};
typedef shared_ptr<MapInfo> MapInfoPtr;

struct FootholdInfo {
	FootholdInfo() : forbidjumpdown(false) { }

	bool forbidjumpdown;
	int16_t id;
	int16_t dragforce;
	int16_t next;
	int16_t prev;
	Pos pos1;
	Pos pos2;
};
typedef vector<FootholdInfo> FootholdsInfo;

struct PortalInfo {
	PortalInfo() : onlyOnce(false) { }

	int8_t id;
	string name;
	Pos pos;
	int32_t tomap;
	string toname;
	string script;
	bool onlyOnce; // Run it only once per map entry
};
typedef unordered_map<string, PortalInfo> PortalsInfo;
typedef unordered_map<int8_t, PortalInfo> SpawnPoints;

struct NPCSpawnInfo {
	NPCSpawnInfo() : facesright(true) { }
	int32_t id;
	Pos pos;
	int16_t fh;
	int16_t rx0;
	int16_t rx1;
	bool facesright;
};
typedef vector<NPCSpawnInfo> NPCSpawnsInfo;

struct ReactorSpawnInfo {
	ReactorSpawnInfo() : spawnat(-1), spawned(false), facesright(true) { }
	int32_t id;
	int32_t time;
	clock_t spawnat;
	bool spawned;
	bool facesright;
	Pos pos;
};
typedef vector<ReactorSpawnInfo> ReactorSpawnsInfo;

struct SeatInfo {
	SeatInfo() : occupant(0) { }
	Pos pos;
	Player *occupant;
};

typedef std::map<int16_t, SeatInfo> SeatsInfo;

struct MobSpawnInfo {
	MobSpawnInfo() : spawned(false), facesright(true), spawnat(-1) { }
	bool facesright;
	bool spawned;
	int16_t fh;
	int32_t id;
	int32_t time;
	int32_t link;
	clock_t spawnat;
	Pos pos;
};
typedef vector<MobSpawnInfo> MobSpawnsInfo;

class MapDataProvider : boost::noncopyable {
public:
	static MapDataProvider * Instance() {
		if (singleton == 0)
			singleton = new MapDataProvider();
		return singleton;
	}

	void loadData();
	Map * getMap(int32_t mapid);
	int8_t getContinent(int32_t mapid);
private:
	MapDataProvider();
	static MapDataProvider *singleton;
	typedef boost::bimap<int8_t, boost::bimaps::unordered_multiset_of<int8_t> > continent_map;
	typedef continent_map::value_type continent_info;

	int32_t loadMapData(int32_t mapid, Map *&map);
	void loadMapTimeMob(MapInfoPtr info);
	void loadFootholds(Map *map, int32_t link);
	void loadMapLife(Map *map, int32_t link);
	void loadPortals(Map *map, int32_t link);
	void loadSeats(Map *map, int32_t link);

	unordered_map<int32_t, Map *> maps;
	continent_map continents;
	boost::mutex loadmap_mutex;
	
	void loadMap(int32_t mapid, Map *&map);
};

#endif
