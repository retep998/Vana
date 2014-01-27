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

#include "GameConstants.hpp"
#include "Types.hpp"
#include <memory>
#include <vector>

enum class MobAttackType {
	Normal,
	SingleTarget,
	Projectile,
	AreaEffect,
	AreaEffectPlus,
};

struct MobAttackInfo {
	int8_t id = 0;
	uint8_t level = 0;
	uint8_t disease = 0;
	uint8_t mpConsume = 0;
	uint16_t mpBurn = 0;
	bool deadlyAttack = false;
	MobAttackType attackType = MobAttackType::Normal;
};

struct MobSkillInfo {
	uint8_t skillId = 0;
	uint8_t level = 0;
	int16_t effectAfter = 0;
};

struct MobInfo {
	bool boss = false;
	bool canFreeze = false;
	bool canPoison = false;
	bool undead = false;
	bool flying = false;
	bool friendly = false;
	bool publicReward = false;
	bool explosiveReward = false;
	bool invincible = false;
	bool damageable = true;
	bool canDoBumpDamage = true;
	bool autoAggro = false;
	bool keepCorpse = false;
	bool onlyNormalAttacks = false;
	int8_t carnivalPoints = 0;
	int8_t hpColor = 0;
	int8_t hpBackgroundColor = 0;
	uint8_t skillCount = 0;
	int16_t wAtk = 0;
	int16_t wDef = 0;
	int16_t mAtk = 0;
	int16_t mDef = 0;
	int16_t acc = 0;
	int16_t avo = 0;
	int16_t speed = 0;
	int16_t chaseSpeed = 0;
	int16_t summonType = 0;
	uint16_t level = 0;
	int32_t selfDestruction = 0;
	int32_t buff = 0;
	int32_t link = 0;
	int32_t removeAfter = 0;
	int32_t knockback = 0;
	int32_t fixedDamage = 0;
	int32_t damagedBySkill = 0;
	int32_t damagedByMob = 0;
	uint32_t hp = 0;
	uint32_t mp = 0;
	uint32_t exp = 0;
	uint32_t hpRecovery = 0;
	uint32_t mpRecovery = 0;
	double traction = 0.;
	MobElementalAttribute iceAttr = MobElementalAttribute::Normal;
	MobElementalAttribute fireAttr = MobElementalAttribute::Normal;
	MobElementalAttribute poisonAttr = MobElementalAttribute::Normal;
	MobElementalAttribute lightningAttr = MobElementalAttribute::Normal;
	MobElementalAttribute holyAttr = MobElementalAttribute::Normal;
	MobElementalAttribute nonElemAttr = MobElementalAttribute::Normal;
	vector_t<int32_t> summon;

	auto hasHpBar() const -> bool { return hpColor > 0; }
};