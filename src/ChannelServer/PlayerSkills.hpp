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

#include "Types.hpp"
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
	auto connectData(PacketCreator &packet) const -> void;

	auto addSkillLevel(int32_t skillId, uint8_t amount, bool sendPacket = true) -> bool;
	auto getSkillLevel(int32_t skillId) const -> uint8_t;
	auto getMaxSkillLevel(int32_t skillId) const -> uint8_t;
	auto setMaxSkillLevel(int32_t skillId, uint8_t maxLevel, bool sendPacket = true) -> void;
	auto getSkillInfo(int32_t skillId) const -> const SkillLevelInfo * const;

	auto hasElementalAmp() const -> bool;
	auto hasEnergyCharge() const -> bool;
	auto hasHpIncrease() const -> bool;
	auto hasMpIncrease() const -> bool;
	auto hasVenomousWeapon() const -> bool;
	auto hasAchilles() const -> bool;
	auto hasNoDamageSkill() const -> bool;
	auto getElementalAmp() const -> int32_t;
	auto getEnergyCharge() const -> int32_t;
	auto getComboAttack() const -> int32_t;
	auto getAdvancedCombo() const -> int32_t;
	auto getAlchemist() const -> int32_t;
	auto getHpIncrease() const -> int32_t;
	auto getMpIncrease() const -> int32_t;
	auto getMastery() const -> int32_t;
	auto getMpEater() const -> int32_t;
	auto getVenomousWeapon() const -> int32_t;
	auto getAchilles() const -> int32_t;
	auto getNoDamageSkill() const -> int32_t;
	auto getRechargeableBonus() const -> int16_t;

	auto addCooldown(int32_t skillId, int16_t time) -> void;
	auto removeCooldown(int32_t skillId) -> void;
	auto removeAllCooldowns() -> void;
private:
	auto hasSkill(int32_t skillId) const -> bool;

	Player *m_player = nullptr;
	hash_map_t<int32_t, PlayerSkillInfo> m_skills;
	hash_map_t<int32_t, int16_t> m_cooldowns;
};