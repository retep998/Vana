/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "Types.hpp"
#include <unordered_map>
#include <vector>

namespace Vana {
	struct AttackData {
		bool isMesoExplosion = false;
		bool isShadowMeso = false;
		bool isChargeSkill = false;
		bool isPiercingArrow = false;
		bool isHeal = false;
		int8_t targets = 0;
		int8_t hits = 0;
		uint8_t display = 0;
		uint8_t weaponSpeed = 0;
		uint8_t animation = 0;
		uint8_t weaponClass = 0;
		skill_level_t skillLevel = 0;
		uint8_t portals = 0;
		inventory_slot_t starPos = -1;
		inventory_slot_t cashStarPos = -1;
		skill_id_t skillId = 0;
		summon_id_t summonId = 0;
		charge_time_t charge = 0;
		item_id_t starId = 0;
		tick_count_t ticks = 0;
		int64_t totalDamage = 0;
		Point projectilePos;
		Point playerPos;
		hash_map_t<map_object_t, vector_t<damage_t>> damages;
	};
}