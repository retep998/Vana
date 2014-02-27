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

class PacketBuilder;
class Player;
struct SkillLevelInfo;

struct PlayerSkillInfo {
	skill_level_t level = 0;
	skill_level_t maxSkillLevel = 0;
	skill_level_t playerMaxSkillLevel = 0;
};

class PlayerSkills {
	NONCOPYABLE(PlayerSkills);
	NO_DEFAULT_CONSTRUCTOR(PlayerSkills);
public:
	PlayerSkills(Player *m_player) : m_player(m_player) { load(); }

	auto load() -> void;
	auto save(bool saveCooldowns = false) -> void;
	auto connectData(PacketBuilder &packet) const -> void;

	auto addSkillLevel(skill_id_t skillId, skill_level_t amount, bool sendPacket = true) -> bool;
	auto getSkillLevel(skill_id_t skillId) const -> skill_level_t;
	auto getMaxSkillLevel(skill_id_t skillId) const -> skill_level_t;
	auto setMaxSkillLevel(skill_id_t skillId, skill_level_t maxLevel, bool sendPacket = true) -> void;
	auto getSkillInfo(skill_id_t skillId) const -> const SkillLevelInfo * const;

	auto hasElementalAmp() const -> bool;
	auto hasEnergyCharge() const -> bool;
	auto hasHpIncrease() const -> bool;
	auto hasMpIncrease() const -> bool;
	auto hasVenomousWeapon() const -> bool;
	auto hasAchilles() const -> bool;
	auto hasNoDamageSkill() const -> bool;
	auto getElementalAmp() const -> skill_id_t;
	auto getEnergyCharge() const -> skill_id_t;
	auto getComboAttack() const -> skill_id_t;
	auto getAdvancedCombo() const -> skill_id_t;
	auto getAlchemist() const -> skill_id_t;
	auto getHpIncrease() const -> skill_id_t;
	auto getMpIncrease() const -> skill_id_t;
	auto getMastery() const -> skill_id_t;
	auto getMpEater() const -> skill_id_t;
	auto getVenomousWeapon() const -> skill_id_t;
	auto getAchilles() const -> skill_id_t;
	auto getNoDamageSkill() const -> skill_id_t;
	auto getRechargeableBonus() const -> slot_qty_t;

	auto addCooldown(skill_id_t skillId, int16_t time) -> void;
	auto removeCooldown(skill_id_t skillId) -> void;
	auto removeAllCooldowns() -> void;
private:
	auto hasSkill(skill_id_t skillId) const -> bool;

	Player *m_player = nullptr;
	hash_map_t<skill_id_t, PlayerSkillInfo> m_skills;
	hash_map_t<skill_id_t, int16_t> m_cooldowns;
};