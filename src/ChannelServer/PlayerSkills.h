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

#include "Types.h"
#include <unordered_map>

class PacketCreator;
class Player;
struct SkillLevelInfo;

struct PlayerSkillInfo {
	uint8_t level = 0;
	uint8_t maxSkillLevel = 0;
	uint8_t playerMaxSkillLevel = 0;
};

class PlayerSkills {
	NONCOPYABLE(PlayerSkills);
	NO_DEFAULT_CONSTRUCTOR(PlayerSkills);
public:
	PlayerSkills(Player *m_player) : m_player(m_player) { load(); }

	auto load() -> void;
	auto save(bool saveCooldowns = false) -> void;
	auto connectData(PacketCreator &packet) -> void;

	auto addSkillLevel(int32_t skillId, uint8_t amount, bool sendPacket = true) -> bool;
	auto getSkillLevel(int32_t skillId) -> uint8_t;
	auto getMaxSkillLevel(int32_t skillId) -> uint8_t;
	auto setMaxSkillLevel(int32_t skillId, uint8_t maxLevel, bool sendPacket = true) -> void;
	auto getSkillInfo(int32_t skillId) -> SkillLevelInfo *;

	auto hasElementalAmp() -> bool;
	auto hasEnergyCharge() -> bool;
	auto hasHpIncrease() -> bool;
	auto hasMpIncrease() -> bool;
	auto hasVenomousWeapon() -> bool;
	auto hasAchilles() -> bool;
	auto hasNoDamageSkill() -> bool;
	auto getElementalAmp() -> int32_t;
	auto getEnergyCharge() -> int32_t;
	auto getComboAttack() -> int32_t;
	auto getAdvancedCombo() -> int32_t;
	auto getAlchemist() -> int32_t;
	auto getHpIncrease() -> int32_t;
	auto getMpIncrease() -> int32_t;
	auto getMastery() -> int32_t;
	auto getMpEater() -> int32_t;
	auto getVenomousWeapon() -> int32_t;
	auto getAchilles() -> int32_t;
	auto getNoDamageSkill() -> int32_t;
	auto getRechargeableBonus() -> int16_t;

	auto addCooldown(int32_t skillId, int16_t time) -> void;
	auto removeCooldown(int32_t skillId) -> void;
	auto removeAllCooldowns() -> void;
private:
	auto hasSkill(int32_t skillId) -> bool;

	Player *m_player = nullptr;
	hash_map_t<int32_t, PlayerSkillInfo> m_skills;
	hash_map_t<int32_t, int16_t> m_cooldowns;
};