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

#include "MapConstants.hpp"
#include "Pos.hpp"
#include "Rect.hpp"
#include "Types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct SkillLevelInfo {
	int8_t mobCount = 0;
	int8_t hitCount = 0;
	int8_t mastery = 0;
	uint8_t criticalDamage = 0;
	int16_t mp = 0;
	int16_t hp = 0;
	int16_t itemCount = 0;
	int16_t bulletConsume = 0;
	int16_t moneyConsume = 0;
	int16_t x = 0;
	int16_t y = 0;
	int16_t speed = 0;
	int16_t jump = 0;
	int16_t str = 0;
	int16_t wAtk = 0;
	int16_t wDef = 0;
	int16_t mAtk = 0;
	int16_t mDef = 0;
	int16_t acc = 0;
	int16_t avo = 0;
	int16_t coolTime = 0;
	int16_t morph = 0;
	int16_t damage = 0;
	int16_t range = 0;
	uint16_t hpProp = 0;
	uint16_t mpProp = 0;
	uint16_t prop = 0;
	int32_t fixedDamage = 0;
	int32_t item = 0;
	int32_t time = 0;
	int32_t optionalItem = 0;
	Rect dimensions;
};

struct ChargeOrStationarySkillInfo {
	uint8_t level = 0;
	uint8_t weaponSpeed = 0;
	uint8_t direction = 0;
	int32_t skillId = 0;
};

struct ReturnDamageInfo {
	bool isPhysical = true;
	uint8_t reduction = 0;
	int32_t damage = 0;
	int32_t mapMobId = 0;
	Pos pos;
};

struct MpEaterInfo {
	bool used = false;
	uint8_t level = 0;
	int16_t x = 0;
	uint16_t prop = 0;
	int32_t skillId = 0;
};

struct MobSkillLevelInfo {
	int8_t summonEffect = 0;
	uint8_t mp = 0;
	uint8_t hp = 0;
	uint8_t count = 0;
	int16_t cooldown = 0;
	int16_t prop = 0;
	int16_t limit = 0;
	int16_t time = 0;
	int32_t x = 0;
	int32_t y = 0;
	Rect dimensions;
	vector_t<int32_t> summons;
};

struct BanishField {
	int32_t field = Maps::NoMap;
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
	EnergyCharge
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
	uint8_t skillLevel = 0;
	uint8_t portals = 0;
	int16_t starPos = -1;
	int16_t cashStarPos = -1;
	int32_t skillId = 0;
	int32_t summonId = 0;
	int32_t charge = 0;
	int32_t starId = 0;
	uint32_t ticks = 0;
	int64_t totalDamage = 0;
	Pos projectilePos;
	Pos playerPos;
	hash_map_t<int32_t, vector_t<int32_t>> damages;
};