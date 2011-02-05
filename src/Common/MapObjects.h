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
#pragma once

#include "Types.h"
#include "Pos.h"
#include <string>

using std::string;

class Player;

struct FootholdInfo {
	FootholdInfo() : forbidJumpDown(false), leftEdge(false), rightEdge(false) { }

	bool forbidJumpDown;
	bool leftEdge;
	bool rightEdge;
	int16_t id;
	int16_t dragForce;
	Pos pos1;
	Pos pos2;
};

struct PortalInfo {
	PortalInfo() : onlyOnce(false) { }

	bool onlyOnce; // Run it only once per map entry
	int8_t id;
	int32_t toMap;
	string toName;
	string script;
	string name;
	Pos pos;
};

struct SpawnInfo {
	SpawnInfo() : facesRight(true), spawned(false) { }

	void setSpawnInfo(const SpawnInfo &rhs) {
		id = rhs.id;
		time = rhs.time;
		foothold = rhs.foothold;
		pos = rhs.pos;
		facesRight = rhs.facesRight;
		spawned = rhs.spawned;
	}
	int32_t id;
	int32_t time;
	int16_t foothold;
	Pos pos;
	bool facesRight;
	bool spawned;
};

struct NpcSpawnInfo : public SpawnInfo {
	NpcSpawnInfo() : SpawnInfo() { }

	int16_t rx0;
	int16_t rx1;
};

struct MobSpawnInfo : public SpawnInfo {
	MobSpawnInfo() : SpawnInfo(), link(0) { }

	int32_t link;
};

struct ReactorSpawnInfo : public SpawnInfo {
	ReactorSpawnInfo() : SpawnInfo() { }

	string name;
};

struct Respawnable {
	Respawnable() : spawnAt(-1), spawnId(0) { }
	Respawnable(size_t spawnId, clock_t spawnAt) : spawnAt(spawnAt), spawnId(spawnId) { }

	size_t spawnId;
	clock_t spawnAt;
};

struct SeatInfo {
	SeatInfo() : occupant(nullptr) { }

	Pos pos;
	Player *occupant;
};
