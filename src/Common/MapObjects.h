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

#include "Pos.h"
#include "Line.h"
#include "Types.h"
#include <string>

class Player;

struct FootholdInfo {
	bool forbidJumpDown = false;
	bool leftEdge = false;
	bool rightEdge = false;
	int16_t id = 0;
	int16_t dragForce = 0;
	Line line;
};

struct PortalInfo {
	bool onlyOnce = false;
	int8_t id = 0;
	int32_t toMap = 0;
	string_t toName;
	string_t script;
	string_t name;
	Pos pos;
};

struct SpawnInfo {
	auto setSpawnInfo(const SpawnInfo &rhs) -> void {
		id = rhs.id;
		time = rhs.time;
		foothold = rhs.foothold;
		pos = rhs.pos;
		facesLeft = rhs.facesLeft;
		spawned = rhs.spawned;
	}

	bool facesLeft = false;
	bool spawned = false;
	int32_t id = 0;
	int32_t time = 0;
	int16_t foothold = 0;
	Pos pos;
};

struct NpcSpawnInfo : public SpawnInfo {
	int16_t rx0 = 0;
	int16_t rx1 = 0;
};

struct MobSpawnInfo : public SpawnInfo {
	int32_t link = 0;
};

struct ReactorSpawnInfo : public SpawnInfo {
	string_t name;
};

struct Respawnable {
	Respawnable() = default;
	Respawnable(size_t spawnId, time_point_t spawnAt) : spawnAt(spawnAt), spawnId(spawnId), spawn(true) { }

	bool spawn = false;
	size_t spawnId = 0;
	time_point_t spawnAt = seconds_t{0};
};

struct SeatInfo {
	int16_t id = 0;
	Player *occupant = nullptr;
	Pos pos;
};