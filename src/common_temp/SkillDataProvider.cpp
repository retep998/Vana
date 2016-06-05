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

namespace vana {

auto skill_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Skills... ";

	load_player_skills();
	load_player_skill_levels();
	load_mob_skills();
	load_mob_summons();
	load_banish_data();
	load_morphs();

	std::cout << "DONE" << std::endl;
}

auto skill_data_provider::load_player_skills() -> void {
	m_skill_levels.clear();
	m_skill_max_levels.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("skill_player_data"));

	for (const auto &row : rs) {
		game_skill_id skill_id = row.get<game_skill_id>("skillid");

		m_skill_levels[skill_id] = hash_map<game_skill_level, skill_level_info>();
		m_skill_max_levels[skill_id] = 1;
	}
}

auto skill_data_provider::load_player_skill_levels() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("skill_player_level_data"));

	for (const auto &row : rs) {
		skill_level_info level;
		game_skill_id skill_id = row.get<game_skill_id>("skillid");
		game_skill_level skill_level = row.get<game_skill_level>("skill_level");

		level.level = skill_level;
		level.mob_count = row.get<int8_t>("mob_count");
		level.hit_count = row.get<int8_t>("hit_count");
		level.range = row.get<int16_t>("range");
		level.buff_time = seconds{row.get<int32_t>("buff_time")};
		level.mp = row.get<game_health>("mp_cost");
		level.hp = row.get<game_health>("hp_cost");
		level.damage = row.get<int16_t>("damage");
		level.fixed_damage = row.get<game_damage>("fixed_damage");
		level.critical_damage = row.get<uint8_t>("critical_damage");
		level.mastery = row.get<int8_t>("mastery");
		level.optional_item = row.get<game_item_id>("optional_item_cost");
		level.item = row.get<game_item_id>("item_cost");
		level.item_count = row.get<game_slot_qty>("item_count");
		level.bullet_consume = row.get<game_slot_qty>("bullet_cost");
		level.money_consume = row.get<int16_t>("money_cost");
		level.x = row.get<int16_t>("x_property");
		level.y = row.get<int16_t>("y_property");
		level.speed = row.get<game_stat>("speed");
		level.jump = row.get<game_stat>("jump");
		level.str = row.get<game_stat>("str");
		level.w_atk = row.get<game_stat>("weapon_atk");
		level.w_def = row.get<game_stat>("weapon_def");
		level.m_atk = row.get<game_stat>("magic_atk");
		level.m_def = row.get<game_stat>("magic_def");
		level.acc = row.get<game_stat>("accuracy");
		level.avo = row.get<game_stat>("avoid");
		level.hp_prop = row.get<uint16_t>("hp");
		level.mp_prop = row.get<uint16_t>("mp");
		level.prop = row.get<uint16_t>("prop");
		level.morph = row.get<game_morph_id>("morph");
		level.dimensions = rect{
			point{row.get<game_coord>("ltx"), row.get<game_coord>("lty")},
			point{row.get<game_coord>("rbx"), row.get<game_coord>("rby")}
		};
		level.cool_time = seconds{row.get<int32_t>("cooldown_time")};

		m_skill_levels[skill_id][skill_level] = level;
		if (m_skill_max_levels.find(skill_id) == std::end(m_skill_max_levels) || m_skill_max_levels[skill_id] < skill_level) {
			m_skill_max_levels[skill_id] = skill_level;
		}
	}
}

auto skill_data_provider::load_mob_skills() -> void {
	m_mob_skills.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("skill_mob_data"));

	for (const auto &row : rs) {
		mob_skill_level_info mob_level;
		game_mob_skill_id skill_id = row.get<game_mob_skill_id>("skillid");
		game_mob_skill_level level = row.get<game_mob_skill_level>("skill_level");

		mob_level.level = level;
		mob_level.time = seconds{row.get<int16_t>("buff_time")};
		mob_level.mp = row.get<uint8_t>("mp_cost");
		mob_level.x = row.get<int32_t>("x_property");
		mob_level.y = row.get<int32_t>("y_property");
		mob_level.prop = row.get<int16_t>("chance");
		mob_level.count = row.get<uint8_t>("target_count");
		mob_level.cooldown = row.get<int16_t>("cooldown");
		mob_level.dimensions = rect{
			point{row.get<game_coord>("ltx"), row.get<game_coord>("lty")},
			point{row.get<game_coord>("rbx"), row.get<game_coord>("rby")}
		};
		mob_level.hp = row.get<uint8_t>("hp_limit_percentage");
		mob_level.limit = row.get<int16_t>("summon_limit");
		mob_level.summon_effect = row.get<int8_t>("summon_effect");

		m_mob_skills[skill_id][level] = mob_level;
	}
}

auto skill_data_provider::load_mob_summons() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("skill_mob_summons"));

	for (const auto &row : rs) {
		game_mob_skill_level level = row.get<game_mob_skill_level>("level");
		game_mob_id mob_id = row.get<game_mob_id>("mobid");

		m_mob_skills[mob_skills::summon][level].summons.push_back(mob_id);
	}
}

auto skill_data_provider::load_banish_data() -> void {
	m_banish_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("skill_mob_banish_data"));

	for (const auto &row : rs) {
		banish_field_info banish;
		game_mob_id mob_id = row.get<game_mob_id>("mobid");

		banish.message = row.get<string>("message");
		banish.field = row.get<game_map_id>("destination");
		banish.portal = row.get<string>("portal");

		m_banish_info[mob_id] = banish;
	}
}

auto skill_data_provider::load_morphs() -> void {
	m_morph_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("morph_data"));

	for (const auto &row : rs) {
		morph_info morph;
		game_morph_id morph_id = row.get<game_morph_id>("morphid");

		utilities::str::run_flags(row.get<opt_string>("flags"), [&morph](const string &cmp) {
			if (cmp == "superman") morph.superman = true;
		});

		morph.speed = row.get<uint8_t>("speed");
		morph.jump = row.get<uint8_t>("jump");
		morph.traction = row.get<double>("traction");
		morph.swim = row.get<double>("swim");

		m_morph_info[morph_id] = morph;
	}
}

auto skill_data_provider::is_valid_skill(game_skill_id skill_id) const -> bool {
	return ext::is_element(m_skill_levels, skill_id);
}

auto skill_data_provider::get_max_level(game_skill_id skill_id) const -> game_skill_level {
	return m_skill_max_levels.find(skill_id)->second;
}

auto skill_data_provider::get_skill(game_skill_id skill, game_skill_level level) const -> const skill_level_info * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_skill_levels, skill), level);
}

auto skill_data_provider::get_mob_skill(game_mob_skill_id skill, game_mob_skill_level level) const -> const mob_skill_level_info * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_mob_skills, skill), level);
}

auto skill_data_provider::get_banish_data(game_mob_id mob_id) const -> const banish_field_info * const {
	return ext::find_value_ptr(m_banish_info, mob_id);
}

auto skill_data_provider::get_morph_data(game_morph_id morph) const -> const morph_info * const {
	return ext::find_value_ptr(m_morph_info, morph);
}

}