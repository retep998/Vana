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

#include "MapConstants.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	struct SkillLevelInfo {
		int8_t mobCount = 0;
		int8_t hitCount = 0;
		int8_t mastery = 0;
		uint8_t criticalDamage = 0;
		skill_level_t level = 0;
		health_t mp = 0;
		health_t hp = 0;
		slot_qty_t itemCount = 0;
		slot_qty_t bulletConsume = 0;
		int16_t moneyConsume = 0;
		int16_t x = 0;
		int16_t y = 0;
		stat_t speed = 0;
		stat_t jump = 0;
		stat_t str = 0;
		stat_t wAtk = 0;
		stat_t wDef = 0;
		stat_t mAtk = 0;
		stat_t mDef = 0;
		stat_t acc = 0;
		stat_t avo = 0;
		int16_t morph = 0;
		int16_t damage = 0;
		int16_t range = 0;
		uint16_t hpProp = 0;
		uint16_t mpProp = 0;
		uint16_t prop = 0;
		damage_t fixedDamage = 0;
		item_id_t item = 0;
		item_id_t optionalItem = 0;
		seconds_t buffTime = seconds_t{0};
		seconds_t coolTime = seconds_t{0};
		Rect dimensions;
	};

	struct ChargeOrStationarySkillInfo {
		skill_level_t level = 0;
		uint8_t weaponSpeed = 0;
		uint8_t direction = 0;
		skill_id_t skillId = 0;
	};

	struct ReturnDamageInfo {
		bool isPhysical = true;
		uint8_t reduction = 0;
		damage_t damage = 0;
		map_object_t mapMobId = 0;
		Point pos;
	};

	struct MpEaterInfo {
		bool used = false;
		skill_level_t level = 0;
		int16_t x = 0;
		uint16_t prop = 0;
		skill_id_t skillId = 0;
	};

	struct MobSkillLevelInfo {
		int8_t summonEffect = 0;
		uint8_t mp = 0;
		uint8_t hp = 0;
		uint8_t count = 0;
		mob_skill_level_t level = 0;
		int16_t cooldown = 0;
		int16_t prop = 0;
		int16_t limit = 0;
		int32_t x = 0;
		int32_t y = 0;
		seconds_t time = seconds_t{0};
		Rect dimensions;
		vector_t<mob_id_t> summons;
	};

	struct BanishField {
		map_id_t field = Maps::NoMap;
		string_t message;
		string_t portal;
	};

	struct MorphData {
		bool superman = false;
		uint8_t speed = 0;
		uint8_t jump = 0;
		double traction = 0.;
		double swim = 0.;
	};

	enum class SkillType {
		Melee,
		Ranged,
		Magic,
		Summon,
		EnergyCharge,
	};

	struct Attack {
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