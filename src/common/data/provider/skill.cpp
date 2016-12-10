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
#include "skill.hpp"
#include "common/algorithm.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/constant/mob_skill.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto skill::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Skills... ";

	load_player_skills();
	load_player_skill_levels();
	load_mob_skills();
	load_mob_summons();
	load_banish_data();
	load_morphs();

	std::cout << "DONE" << std::endl;
}

auto skill::load_player_skills() -> void {
	m_skill_levels.clear();
	m_skill_max_levels.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::skill_player_data));

	for (const auto &row : rs) {
		game_skill_id skill_id = row.get<game_skill_id>("skillid");

		m_skill_levels.emplace_back(skill_id, vector<data::type::skill_level_info>{});
		m_skill_max_levels.emplace_back(skill_id, 1);
	}
}

auto skill::load_player_skill_levels() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::skill_player_level_data));

	for (const auto &row : rs) {
		data::type::skill_level_info info;
		game_skill_id skill_id = row.get<game_skill_id>("skillid");
		game_skill_level skill_level = row.get<game_skill_level>("skill_level");

		info.level = skill_level;
		info.mob_count = row.get<int8_t>("mob_count");
		info.hit_count = row.get<int8_t>("hit_count");
		info.range = row.get<int16_t>("range");
		info.buff_time = seconds{row.get<int32_t>("buff_time")};
		info.mp = row.get<game_health>("mp_cost");
		info.hp = row.get<game_health>("hp_cost");
		info.damage = row.get<int16_t>("damage");
		info.fixed_damage = row.get<game_damage>("fixed_damage");
		info.critical_damage = row.get<uint8_t>("critical_damage");
		info.mastery = row.get<int8_t>("mastery");
		info.optional_item = row.get<game_item_id>("optional_item_cost");
		info.item = row.get<game_item_id>("item_cost");
		info.item_count = row.get<game_slot_qty>("item_count");
		info.bullet_consume = row.get<game_slot_qty>("bullet_cost");
		info.money_consume = row.get<int16_t>("money_cost");
		info.x = row.get<int16_t>("x_property");
		info.y = row.get<int16_t>("y_property");
		info.speed = row.get<game_stat>("speed");
		info.jump = row.get<game_stat>("jump");
		info.str = row.get<game_stat>("str");
		info.w_atk = row.get<game_stat>("weapon_atk");
		info.w_def = row.get<game_stat>("weapon_def");
		info.m_atk = row.get<game_stat>("magic_atk");
		info.m_def = row.get<game_stat>("magic_def");
		info.acc = row.get<game_stat>("accuracy");
		info.avo = row.get<game_stat>("avoid");
		info.hp_prop = row.get<uint16_t>("hp");
		info.mp_prop = row.get<uint16_t>("mp");
		info.prop = row.get<uint16_t>("prop");
		info.morph = row.get<game_morph_id>("morph");
		info.dimensions = rect{
			point{row.get<game_coord>("ltx"), row.get<game_coord>("lty")},
			point{row.get<game_coord>("rbx"), row.get<game_coord>("rby")}
		};
		info.cool_time = seconds{row.get<int32_t>("cooldown_time")};

		bool found = false;
		for (auto &skill_level_value : m_skill_levels) {
			if (skill_level_value.first == skill_id) {
				found = true;
				skill_level_value.second.push_back(info);
				break;
			}
		}

		if (!found) {
			vector<data::type::skill_level_info> current;
			current.push_back(info);
			m_skill_levels.emplace_back(skill_id, current);
		}

		found = false;
		for (auto &max_skill_level : m_skill_max_levels) {
			if (max_skill_level.first == skill_id) {
				found = true;
				if (skill_level > max_skill_level.second) {
					max_skill_level.second = skill_level;
				}
				break;
			}
		}

		if (!found) {
			m_skill_max_levels.emplace_back(skill_id, skill_level);
		}
	}
}

auto skill::load_mob_skills() -> void {
	m_mob_skills.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::skill_mob_data));

	for (const auto &row : rs) {
		data::type::mob_skill_level_info mob_level;
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

		bool found = false;
		for (auto &skill : m_mob_skills) {
			if (skill.first == skill_id) {
				found = true;
				skill.second.push_back(mob_level);
				break;
			}
		}

		if (!found) {
			vector<data::type::mob_skill_level_info> current;
			current.push_back(mob_level);
			m_mob_skills.emplace_back(skill_id, current);
		}
	}
}

auto skill::load_mob_summons() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::skill_mob_summons));

	for (const auto &row : rs) {
		game_mob_skill_level level = row.get<game_mob_skill_level>("level");
		game_mob_id mob_id = row.get<game_mob_id>("mobid");

		bool any = false;
		for (auto &skill : m_mob_skills) {
			if (skill.first != constant::mob_skill::summon) {
				continue;
			}

			for (auto &skill_level : skill.second) {
				if (skill_level.level != level) {
					continue;
				}

				any = true;
				skill_level.summons.push_back(mob_id);
			}
		}

		if (!any) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto skill::load_banish_data() -> void {
	m_banish_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::skill_mob_banish_data));

	for (const auto &row : rs) {
		data::type::banish_field_info banish;
		banish.mob_id = row.get<game_mob_id>("mobid");
		banish.message = row.get<string>("message");
		banish.field = row.get<game_map_id>("destination");
		banish.portal = row.get<string>("portal");

		m_banish_info.push_back(banish);
	}
}

auto skill::load_morphs() -> void {
	m_morph_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::morph_data));

	for (const auto &row : rs) {
		data::type::morph_info morph;
		morph.id = row.get<game_morph_id>("morphid");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&morph](const string &cmp) {
			if (cmp == "superman") morph.superman = true;
		});

		morph.speed = row.get<uint8_t>("speed");
		morph.jump = row.get<uint8_t>("jump");
		morph.traction = row.get<double>("traction");
		morph.swim = row.get<double>("swim");

		m_morph_info.push_back(morph);
	}
}

auto skill::is_valid_skill(game_skill_id skill_id) const -> bool {
	return ext::any_of(m_skill_levels, [&skill_id](auto value) { return value.first == skill_id; });
}

auto skill::get_max_level(game_skill_id skill_id) const -> game_skill_level {
	for (const auto &skill : m_skill_max_levels) {
		if (skill.first == skill_id) {
			return skill.second;
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

auto skill::get_skill(game_skill_id skill, game_skill_level level) const -> const data::type::skill_level_info * const {
	auto skill_ptr = ext::find_value_ptr_if(m_skill_levels, [&skill](auto value) { return value.first == skill; });
	if (skill_ptr == nullptr) return nullptr;
	return ext::find_value_ptr_if(
		skill_ptr->second,
		[&level](auto value) { return value.level == level; });
}

auto skill::get_mob_skill(game_mob_skill_id skill, game_mob_skill_level level) const -> const data::type::mob_skill_level_info * const {
	auto skill_ptr = ext::find_value_ptr_if(m_mob_skills, [&skill](auto value) { return value.first == skill; });
	if (skill_ptr == nullptr) return nullptr;
	return ext::find_value_ptr_if(
		skill_ptr->second,
		[&level](auto value) { return value.level == level; });
}

auto skill::get_banish_data(game_mob_id mob_id) const -> const data::type::banish_field_info * const {
	return ext::find_value_ptr_if(
		m_banish_info,
		[&mob_id](auto value) { return value.mob_id == mob_id; });
}

auto skill::get_morph_data(game_morph_id morph) const -> const data::type::morph_info * const {
	return ext::find_value_ptr_if(
		m_morph_info,
		[&morph](auto value) { return value.id == morph; });
}

}
}
}