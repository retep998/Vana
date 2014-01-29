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

#include "SkillConstants.hpp"
#include "SkillDataObjects.hpp"
#include "Types.hpp"
#include <unordered_map>

class SkillDataProvider {
	SINGLETON(SkillDataProvider);
public:
	auto loadData() -> void;

	auto isValidSkill(int32_t skillId) const -> bool;
	auto getMaxLevel(int32_t skillId) const -> uint8_t;
	auto getSkill(int32_t skill, uint8_t level) const -> const SkillLevelInfo * const;
	auto getMobSkill(uint8_t skill, uint8_t level) const -> const MobSkillLevelInfo * const;
	auto getBanishData(int32_t mobId) const -> const BanishField * const;
	auto getMorphData(int16_t morph) const -> const MorphData * const;
private:
	auto loadPlayerSkills() -> void;
	auto loadPlayerSkillLevels() -> void;
	auto loadMobSkills() -> void;
	auto loadMobSummons() -> void;
	auto loadBanishData() -> void;
	auto loadMorphs() -> void;

	hash_map_t<uint8_t, hash_map_t<uint8_t, MobSkillLevelInfo>> m_mobSkills;
	hash_map_t<int32_t, hash_map_t<uint8_t, SkillLevelInfo>> m_skillLevels;
	hash_map_t<int32_t, uint8_t> m_skillMaxLevels;
	hash_map_t<int32_t, BanishField> m_banishInfo;
	hash_map_t<int16_t, MorphData> m_morphInfo;
};