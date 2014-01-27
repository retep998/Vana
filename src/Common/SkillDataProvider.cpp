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
#include "SkillDataProvider.h"
#include "Database.h"
#include "InitializeCommon.h"
#include "SkillConstants.h"
#include "StringUtilities.h"
#include <iomanip>
#include <iostream>

auto SkillDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Skills... ";

	loadPlayerSkills();
	loadPlayerSkillLevels();
	loadMobSkills();
	loadMobSummons();
	loadBanishData();
	loadMorphs();

	std::cout << "DONE" << std::endl;
}

auto SkillDataProvider::loadPlayerSkills() -> void {
	m_skillLevels.clear();
	m_skillMaxLevels.clear();
	int32_t skillId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM skill_player_data");

	for (const auto &row : rs) {
		skillId = row.get<int32_t>("skillid");

		m_skillLevels[skillId] = hash_map_t<uint8_t, SkillLevelInfo>();
		m_skillMaxLevels[skillId] = 1;
	}
}

auto SkillDataProvider::loadPlayerSkillLevels() -> void {
	SkillLevelInfo level;
	int32_t skillId;
	uint8_t skillLevel;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM skill_player_level_data");

	for (const auto &row : rs) {
		skillId = row.get<int32_t>("skillid");
		skillLevel = row.get<uint8_t>("skill_level");

		level.mobCount = row.get<int8_t>("mob_count");
		level.hitCount = row.get<int8_t>("hit_count");
		level.range = row.get<int16_t>("range");
		level.time = row.get<int32_t>("buff_time");
		level.mp = row.get<int16_t>("mp_cost");
		level.hp = row.get<int16_t>("hp_cost");
		level.damage = row.get<int16_t>("damage");
		level.fixedDamage = row.get<int32_t>("fixed_damage");
		level.criticalDamage = row.get<uint8_t>("critical_damage");
		level.mastery = row.get<int8_t>("mastery");
		level.optionalItem = row.get<int32_t>("optional_item_cost");
		level.item = row.get<int32_t>("item_cost");
		level.itemCount = row.get<int16_t>("item_count");
		level.bulletConsume = row.get<int16_t>("bullet_cost");
		level.moneyConsume = row.get<int16_t>("money_cost");
		level.x = row.get<int16_t>("x_property");
		level.y = row.get<int16_t>("y_property");
		level.speed = row.get<int16_t>("speed");
		level.jump = row.get<int16_t>("jump");
		level.str = row.get<int16_t>("str");
		level.wAtk = row.get<int16_t>("weapon_atk");
		level.wDef = row.get<int16_t>("weapon_def");
		level.mAtk = row.get<int16_t>("magic_atk");
		level.mDef = row.get<int16_t>("magic_def");
		level.acc = row.get<int16_t>("accuracy");
		level.avo = row.get<int16_t>("avoid");
		level.hpProp = row.get<uint16_t>("hp");
		level.mpProp = row.get<uint16_t>("mp");
		level.prop = row.get<uint16_t>("prop");
		level.morph = row.get<int16_t>("morph");
		level.dimensions = Rect(Pos(row.get<int16_t>("ltx"), row.get<int16_t>("lty")),
								Pos(row.get<int16_t>("rbx"), row.get<int16_t>("rby")));
		level.coolTime = row.get<int16_t>("cooldown_time");

		m_skillLevels[skillId][skillLevel] = level;
		if (m_skillMaxLevels.find(skillId) == std::end(m_skillMaxLevels) || m_skillMaxLevels[skillId] < skillLevel) {
			m_skillMaxLevels[skillId] = skillLevel;
		}
	}
}

auto SkillDataProvider::loadMobSkills() -> void {
	m_mobSkills.clear();
	uint8_t skillId;
	uint8_t level;
	MobSkillLevelInfo mobLevel;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM skill_mob_data");

	for (const auto &row : rs) {
		skillId = row.get<uint8_t>("skillid");
		level = row.get<uint8_t>("skill_level");
		mobLevel.time = row.get<int16_t>("buff_time");
		mobLevel.mp = row.get<uint8_t>("mp_cost");
		mobLevel.x = row.get<int32_t>("x_property");
		mobLevel.y = row.get<int32_t>("y_property");
		mobLevel.prop = row.get<int16_t>("chance");
		mobLevel.count = row.get<uint8_t>("target_count");
		mobLevel.cooldown = row.get<int16_t>("cooldown");
		mobLevel.dimensions = Rect(Pos(row.get<int16_t>("ltx"), row.get<int16_t>("lty")),
									Pos(row.get<int16_t>("rbx"), row.get<int16_t>("rby")));
		mobLevel.hp = row.get<uint8_t>("hp_limit_percentage");
		mobLevel.limit = row.get<int16_t>("summon_limit");
		mobLevel.summonEffect = row.get<int8_t>("summon_effect");

		m_mobSkills[skillId][level] = mobLevel;
	}
}

auto SkillDataProvider::loadMobSummons() -> void {
	uint8_t level;
	int32_t mobId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM skill_mob_summons");

	for (const auto &row : rs) {
		level = row.get<uint8_t>("level");
		mobId = row.get<int32_t>("mobid");

		m_mobSkills[MobSkills::Summon][level].summons.push_back(mobId);
	}
}

auto SkillDataProvider::loadBanishData() -> void {
	m_banishInfo.clear();
	BanishField banish;
	int32_t mobId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM skill_mob_banish_data");

	for (const auto &row : rs) {
		mobId = row.get<int32_t>("mobid");

		banish.message = row.get<string_t>("message");
		banish.field = row.get<int32_t>("destination");
		banish.portal = row.get<string_t>("portal");

		m_banishInfo[mobId] = banish;
	}
}

auto SkillDataProvider::loadMorphs() -> void {
	m_morphInfo.clear();
	MorphData morph;
	int16_t morphId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM morph_data");

	for (const auto &row : rs) {
		morph = MorphData();
		morphId = row.get<int16_t>("morphid");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&morph](const string_t &cmp) {
			if (cmp == "superman") morph.superman = true;
		});

		morph.speed = row.get<uint8_t>("speed");
		morph.jump = row.get<uint8_t>("jump");
		morph.traction = row.get<double>("traction");
		morph.swim = row.get<double>("swim");

		m_morphInfo[morphId] = morph;
	}
}

auto SkillDataProvider::getSkill(int32_t skill, uint8_t level) -> SkillLevelInfo * {
	if (m_skillLevels.find(skill) != std::end(m_skillLevels)) {
		if (m_skillLevels[skill].find(level) != std::end(m_skillLevels[skill])) {
			return &m_skillLevels[skill][level];
		}
	}
	return nullptr;
}

auto SkillDataProvider::getMobSkill(uint8_t skill, uint8_t level) -> MobSkillLevelInfo * {
	if (m_mobSkills.find(skill) != std::end(m_mobSkills)) {
		if (m_mobSkills[skill].find(level) != std::end(m_mobSkills[skill])) {
			return &m_mobSkills[skill][level];
		}
	}
	return nullptr;
}