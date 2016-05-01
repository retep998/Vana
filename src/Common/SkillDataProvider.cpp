/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "SkillDataProvider.hpp"
#include "Algorithm.hpp"
#include "Database.hpp"
#include "InitializeCommon.hpp"
#include "SkillConstants.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>

namespace Vana {

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

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("skill_player_data"));

	for (const auto &row : rs) {
		skill_id_t skillId = row.get<skill_id_t>("skillid");

		m_skillLevels[skillId] = hash_map_t<skill_level_t, SkillLevelInfo>();
		m_skillMaxLevels[skillId] = 1;
	}
}

auto SkillDataProvider::loadPlayerSkillLevels() -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("skill_player_level_data"));

	for (const auto &row : rs) {
		SkillLevelInfo level;
		skill_id_t skillId = row.get<skill_id_t>("skillid");
		skill_level_t skillLevel = row.get<skill_level_t>("skill_level");

		level.level = skillLevel;
		level.mobCount = row.get<int8_t>("mob_count");
		level.hitCount = row.get<int8_t>("hit_count");
		level.range = row.get<int16_t>("range");
		level.buffTime = seconds_t{row.get<int32_t>("buff_time")};
		level.mp = row.get<health_t>("mp_cost");
		level.hp = row.get<health_t>("hp_cost");
		level.damage = row.get<int16_t>("damage");
		level.fixedDamage = row.get<damage_t>("fixed_damage");
		level.criticalDamage = row.get<uint8_t>("critical_damage");
		level.mastery = row.get<int8_t>("mastery");
		level.optionalItem = row.get<item_id_t>("optional_item_cost");
		level.item = row.get<item_id_t>("item_cost");
		level.itemCount = row.get<slot_qty_t>("item_count");
		level.bulletConsume = row.get<slot_qty_t>("bullet_cost");
		level.moneyConsume = row.get<int16_t>("money_cost");
		level.x = row.get<int16_t>("x_property");
		level.y = row.get<int16_t>("y_property");
		level.speed = row.get<stat_t>("speed");
		level.jump = row.get<stat_t>("jump");
		level.str = row.get<stat_t>("str");
		level.wAtk = row.get<stat_t>("weapon_atk");
		level.wDef = row.get<stat_t>("weapon_def");
		level.mAtk = row.get<stat_t>("magic_atk");
		level.mDef = row.get<stat_t>("magic_def");
		level.acc = row.get<stat_t>("accuracy");
		level.avo = row.get<stat_t>("avoid");
		level.hpProp = row.get<uint16_t>("hp");
		level.mpProp = row.get<uint16_t>("mp");
		level.prop = row.get<uint16_t>("prop");
		level.morph = row.get<morph_id_t>("morph");
		level.dimensions = Rect{
			Point{row.get<coord_t>("ltx"), row.get<coord_t>("lty")},
			Point{row.get<coord_t>("rbx"), row.get<coord_t>("rby")}
		};
		level.coolTime = seconds_t{row.get<int32_t>("cooldown_time")};

		m_skillLevels[skillId][skillLevel] = level;
		if (m_skillMaxLevels.find(skillId) == std::end(m_skillMaxLevels) || m_skillMaxLevels[skillId] < skillLevel) {
			m_skillMaxLevels[skillId] = skillLevel;
		}
	}
}

auto SkillDataProvider::loadMobSkills() -> void {
	m_mobSkills.clear();

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("skill_mob_data"));

	for (const auto &row : rs) {
		MobSkillLevelInfo mobLevel;
		mob_skill_id_t skillId = row.get<mob_skill_id_t>("skillid");
		mob_skill_level_t level = row.get<mob_skill_level_t>("skill_level");

		mobLevel.level = level;
		mobLevel.time = seconds_t{row.get<int16_t>("buff_time")};
		mobLevel.mp = row.get<uint8_t>("mp_cost");
		mobLevel.x = row.get<int32_t>("x_property");
		mobLevel.y = row.get<int32_t>("y_property");
		mobLevel.prop = row.get<int16_t>("chance");
		mobLevel.count = row.get<uint8_t>("target_count");
		mobLevel.cooldown = row.get<int16_t>("cooldown");
		mobLevel.dimensions = Rect{
			Point{row.get<coord_t>("ltx"), row.get<coord_t>("lty")},
			Point{row.get<coord_t>("rbx"), row.get<coord_t>("rby")}
		};
		mobLevel.hp = row.get<uint8_t>("hp_limit_percentage");
		mobLevel.limit = row.get<int16_t>("summon_limit");
		mobLevel.summonEffect = row.get<int8_t>("summon_effect");

		m_mobSkills[skillId][level] = mobLevel;
	}
}

auto SkillDataProvider::loadMobSummons() -> void {
	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("skill_mob_summons"));

	for (const auto &row : rs) {
		mob_skill_level_t level = row.get<mob_skill_level_t>("level");
		mob_id_t mobId = row.get<mob_id_t>("mobid");

		m_mobSkills[MobSkills::Summon][level].summons.push_back(mobId);
	}
}

auto SkillDataProvider::loadBanishData() -> void {
	m_banishInfo.clear();

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("skill_mob_banish_data"));

	for (const auto &row : rs) {
		BanishFieldInfo banish;
		mob_id_t mobId = row.get<mob_id_t>("mobid");

		banish.message = row.get<string_t>("message");
		banish.field = row.get<map_id_t>("destination");
		banish.portal = row.get<string_t>("portal");

		m_banishInfo[mobId] = banish;
	}
}

auto SkillDataProvider::loadMorphs() -> void {
	m_morphInfo.clear();

	auto &db = Database::getDataDb();
	auto &sql = db.getSession();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.makeTable("morph_data"));

	for (const auto &row : rs) {
		MorphInfo morph;
		morph_id_t morphId = row.get<morph_id_t>("morphid");

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

auto SkillDataProvider::isValidSkill(skill_id_t skillId) const -> bool {
	return ext::is_element(m_skillLevels, skillId);
}

auto SkillDataProvider::getMaxLevel(skill_id_t skillId) const -> skill_level_t {
	return m_skillMaxLevels.find(skillId)->second;
}

auto SkillDataProvider::getSkill(skill_id_t skill, skill_level_t level) const -> const SkillLevelInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_skillLevels, skill), level);
}

auto SkillDataProvider::getMobSkill(mob_skill_id_t skill, mob_skill_level_t level) const -> const MobSkillLevelInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_mobSkills, skill), level);
}

auto SkillDataProvider::getBanishData(mob_id_t mobId) const -> const BanishFieldInfo * const {
	return ext::find_value_ptr(m_banishInfo, mobId);
}

auto SkillDataProvider::getMorphData(morph_id_t morph) const -> const MorphInfo * const {
	return ext::find_value_ptr(m_morphInfo, morph);
}

}