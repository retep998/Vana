/*
Copyright (C) 2008-2013 Vana Development Team

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

#include "noncopyable.hpp"
#include "SkillConstants.h"
#include "SkillDataObjects.h"
#include "Types.h"
#include <unordered_map>

using std::unordered_map;

class SkillDataProvider : boost::noncopyable {
public:
	static SkillDataProvider * Instance() {
		if (singleton == nullptr)
			singleton = new SkillDataProvider();
		return singleton;
	}
	void loadData();

	bool isSkill(int32_t skillId) { return (m_skills.find(skillId) != m_skills.end()); }
	uint8_t getMaxLevel(int32_t skillId) { return (m_skillMaxLevels.find(skillId) != m_skillMaxLevels.end() ? m_skillMaxLevels[skillId] : 0); }
	SkillLevelInfo * getSkill(int32_t skill, uint8_t level);
	MobSkillLevelInfo * getMobSkill(uint8_t skill, uint8_t level);

	bool hasBanishData(int32_t mobId) { return (m_banishInfo.find(mobId) != m_banishInfo.end()); }
	BanishField * getBanishData(int32_t mobId) { return (hasBanishData(mobId) ? &m_banishInfo[mobId] : nullptr); }

	bool hasMorphData(int16_t morph) { return (m_morphInfo.find(morph) != m_morphInfo.end()); }
	MorphData * getMorphData(int16_t morph) { return (hasMorphData(morph) ? &m_morphInfo[morph] : nullptr); }
private:
	SkillDataProvider() {}
	static SkillDataProvider *singleton;

	void loadPlayerSkills();
	void loadMobSkills();
	void loadMobSummons();
	void loadBanishData();
	void loadMorphs();

	unordered_map<uint8_t, MobSkillsLevelInfo> m_mobSkills;
	unordered_map<int32_t, SkillsLevelInfo> m_skills;
	unordered_map<int32_t, uint8_t> m_skillMaxLevels;
	unordered_map<int32_t, BanishField> m_banishInfo;
	unordered_map<int16_t, MorphData> m_morphInfo;
};