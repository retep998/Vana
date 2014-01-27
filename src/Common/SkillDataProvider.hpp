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

	auto isValidSkill(int32_t skillId) -> bool { return m_skillLevels.find(skillId) != std::end(m_skillLevels); }
	auto getMaxLevel(int32_t skillId) -> uint8_t { return m_skillMaxLevels.find(skillId) != std::end(m_skillMaxLevels) ? m_skillMaxLevels[skillId] : 0; }
	auto getSkill(int32_t skill, uint8_t level) -> SkillLevelInfo *;
	auto getMobSkill(uint8_t skill, uint8_t level) -> MobSkillLevelInfo *;

	auto hasBanishData(int32_t mobId) -> bool { return m_banishInfo.find(mobId) != std::end(m_banishInfo); }
	auto getBanishData(int32_t mobId) -> BanishField * { return hasBanishData(mobId) ? &m_banishInfo[mobId] : nullptr; }

	auto hasMorphData(int16_t morph) -> bool { return m_morphInfo.find(morph) != std::end(m_morphInfo); }
	auto getMorphData(int16_t morph) -> MorphData * { return hasMorphData(morph) ? &m_morphInfo[morph] : nullptr; }
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