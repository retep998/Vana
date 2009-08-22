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
	MapInfo() : left(0), top(0), bottom(0), right(0), musicname("") {}
	bool clock;
	bool town;
	int8_t fieldType;
	int8_t continent;
	int8_t starthour;
	int8_t endhour;
	int16_t left;
	int16_t top;
	int16_t bottom;
	int16_t right;
	int32_t id;
	int32_t rm;
	int32_t forcedReturn;
	int32_t shipInterval;
	int32_t fieldLimit;
	int32_t link;
	int32_t timemob;
	double spawnrate;
	string musicname;
	string message;
};
typedef shared_ptr<MapInfo> MapInfoPtr;

struct FootholdInfo {
	int16_t id;
	Pos pos1;
	Pos pos2;
};
typedef vector<FootholdInfo> FootholdsInfo;

struct PortalInfo {
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
	int32_t id;
	Pos pos;
	int16_t fh;
	int16_t rx0;
	int16_t rx1;
	int8_t facingside;
};
typedef vector<NPCSpawnInfo> NPCSpawnsInfo;

struct ReactorSpawnInfo {
	ReactorSpawnInfo() : spawnat(-1), spawned(false) { }
	int32_t id;
	int32_t time;
	int32_t link;
	clock_t spawnat;
	bool spawned;
	Pos pos;
};
typedef vector<ReactorSpawnInfo> ReactorSpawnsInfo;

struct SeatInfo {
	Pos pos;
	Player *occupant;
};

typedef std::map<int16_t, SeatInfo> SeatsInfo;

struct MobSpawnInfo {
	MobSpawnInfo() : spawned(false), spawnat(-1) { }
	int8_t facingside;
	int16_t fh;
	int32_t id;
	int32_t time;
	int32_t link;
	bool spawned;
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

	Map * getMap(int32_t mapid);
	int8_t getContinent(int32_t mapid);
private:
	MapDataProvider();
	static MapDataProvider *singleton;
	typedef boost::bimap<int8_t, boost::bimaps::unordered_multiset_of<int8_t> > continent_map;
	typedef continent_map::value_type continent_info;

	unordered_map<int32_t, Map *> maps;
	continent_map continents;
	boost::mutex loadmap_mutex;
	
	void loadMap(int32_t mapid, Map *&map);
};

#endif
