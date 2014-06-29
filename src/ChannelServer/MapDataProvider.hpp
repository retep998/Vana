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
#pragma once

#include "Point.hpp"
#include "Rect.hpp"
#include "Types.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class Map;
class Player;

struct TimeMob {
	int8_t startHour = 0;
	int8_t endHour = 0;
	mob_id_t id = 0;
	string_t message;
};

struct FieldLimit {
	bool jump = false;
	bool movementSkills = false;
	bool summoningBag = false;
	bool mysticDoor = false;
	bool channelSwitching = false;
	bool regularExpLoss = false;
	bool vipRock = false;
	bool minigames = false;
	bool mount = false;
	bool potionUse = false;
	bool dropDown = false;
	bool chalkboard = false;
};

struct MapInfo {
	bool clock = false;
	bool town = false;
	bool swim = false;
	bool fly = false;
	bool everlast = false;
	bool noLeaderPass = false;
	bool shop = false;
	bool scrollDisable = false;
	bool shuffleReactors = false;
	bool forceMapEquip = false;
	int8_t continent = -1;
	int8_t regenRate = 0;
	int8_t shipKind = -1;
	player_level_t minLevel = 0;
	uint8_t decHp = 0;
	uint16_t dps = 0;
	map_id_t rm = 0;
	map_id_t forcedReturn = 0;
	map_id_t link = 0;
	int32_t timeLimit = 0;
	item_id_t protectItem = 0;
	damage_t damagePerSecond = 0;
	double spawnRate = 0.;
	double traction = 0.;
	string_t defaultMusic;
	string_t shuffleName;
	string_t message;
	Rect dimensions;
	FieldLimit limitations;
};

class MapDataProvider {
public:
	auto loadData() -> void;
	auto getMap(map_id_t mapId) -> Map *;
	auto unloadMap(map_id_t mapId) -> void;
	auto getContinent(map_id_t mapId) const -> int8_t;
private:
	auto loadMapData(map_id_t mapId, Map *&map) -> map_id_t;
	auto loadMapTimeMob(Map *map) -> void;
	auto loadFootholds(Map *map, map_id_t link) -> void;
	auto loadMapLife(Map *map, map_id_t link) -> void;
	auto loadPortals(Map *map, map_id_t link) -> void;
	auto loadSeats(Map *map, map_id_t link) -> void;
	auto loadMap(map_id_t mapId, Map *&map) -> void;

	mutex_t m_loadMutex;
	hash_map_t<map_id_t, Map *> m_maps;
	hash_map_t<int8_t, int8_t> m_continents;
};