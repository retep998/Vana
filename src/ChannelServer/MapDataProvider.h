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
#pragma once

#include "noncopyable.hpp"
#include "Pos.h"
#include "Rect.h"
#include "Types.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class Map;
class Player;

struct TimeMob {
	TimeMob() : startHour(0), endHour(0), id(0) { }
	int8_t startHour;
	int8_t endHour;
	int32_t id;
	string message;
};
typedef shared_ptr<TimeMob> TimeMobPtr;

struct FieldLimit {
	FieldLimit();
	bool jump;
	bool movementSkills;
	bool summoningBag;
	bool mysticDoor;
	bool channelSwitching;
	bool regularExpLoss;
	bool vipRock;
	bool minigames;
	bool mount;
	bool potionUse;
	bool dropDown;
	bool chalkboard;
};

struct MapInfo {
	MapInfo() :
		clock(false),
		town(false),
		swim(false),
		fly(false),
		everlast(false),
		noLeaderPass(false),
		shop(false),
		scrollDisable(false),
		shuffleReactors(false),
		forceMapEquip(false),
		damagePerSecond(0)
		{ }
	bool clock;
	bool town;
	bool swim;
	bool fly;
	bool everlast;
	bool noLeaderPass;
	bool shop;
	bool scrollDisable;
	bool shuffleReactors;
	bool forceMapEquip;
	int8_t continent;
	int8_t regenRate;
	int8_t shipKind;
	uint8_t minLevel;
	uint8_t decHp;
	uint16_t dps;
	int32_t rm;
	int32_t forcedReturn;
	int32_t link;
	int32_t timeLimit;
	int32_t protectItem;
	int32_t damagePerSecond;
	double spawnRate;
	double traction;
	string defaultMusic;
	string shuffleName;
	string message;
	Rect dimensions;
	FieldLimit limitations;
};
typedef shared_ptr<MapInfo> MapInfoPtr;

class MapDataProvider : boost::noncopyable {
public:
	static MapDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new MapDataProvider();
		return singleton;
	}

	void loadData();
	Map * getMap(int32_t mapId);
	void unloadMap(int32_t mapId);
	int8_t getContinent(int32_t mapId);
private:
	MapDataProvider();
	static MapDataProvider *singleton;

	int32_t loadMapData(int32_t mapId, Map *&map);
	void loadMapTimeMob(Map *map);
	void loadFootholds(Map *map, int32_t link);
	void loadMapLife(Map *map, int32_t link);
	void loadPortals(Map *map, int32_t link);
	void loadSeats(Map *map, int32_t link);
	void loadMap(int32_t mapId, Map *&map);

	unordered_map<int32_t, Map *> m_maps;
	unordered_map<int8_t, int8_t> m_continents;
	std::mutex m_loadMutex;
};