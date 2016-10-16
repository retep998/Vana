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
#include "mob.hpp"
#include "common/algorithm.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace vana {
namespace data {
namespace provider {

auto mob::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Mobs... ";

	load_attacks();
	load_skills();
	load_mobs();
	load_summons();

	std::cout << "DONE" << std::endl;
}

auto mob::load_attacks() -> void {
	m_attacks.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::mob_attacks));

	for (const auto &row : rs) {
		data::type::mob_attack_info mob_attack;

		game_mob_id mob_id = row.get<game_mob_id>("mobid");
		mob_attack.id = row.get<int8_t>("attackid");
		mob_attack.mp_consume = row.get<uint8_t>("mp_cost");
		mob_attack.mp_burn = row.get<uint16_t>("mp_burn");
		mob_attack.disease = row.get<game_mob_skill_id>("mob_skillid");
		mob_attack.level = row.get<game_mob_skill_level>("mob_skill_level");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&mob_attack](const string &cmp) {
			if (cmp == "deadly") mob_attack.deadly_attack = true;
		});
		vana::util::str::run_enum(row.get<string>("attack_type"), [&mob_attack](const string &cmp) {
			if (cmp == "normal") mob_attack.attack_type = data::type::mob_attack_type::normal;
			else if (cmp == "projectile") mob_attack.attack_type = data::type::mob_attack_type::projectile;
			else if (cmp == "single_target") mob_attack.attack_type = data::type::mob_attack_type::single_target;
			else if (cmp == "area_effect") mob_attack.attack_type = data::type::mob_attack_type::area_effect;
			else if (cmp == "area_effect_plus") mob_attack.attack_type = data::type::mob_attack_type::area_effect_plus;
		});

		bool found = false;
		for (auto &mob : m_attacks) {
			if (mob.first == mob_id) {
				mob.second.push_back(mob_attack);
				found = true;
				break;
			}
		}
		if (!found) {
			vector<data::type::mob_attack_info> new_value{mob_attack};
			m_attacks.push_back(std::make_pair(mob_id, new_value));
		}
	}
}

auto mob::load_skills() -> void {
	m_skills.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::mob_skills));

	for (const auto &row : rs) {
		data::type::mob_skill_info mob_skill;
		game_mob_id mob_id = row.get<game_mob_id>("mobid");
		mob_skill.skill_id = row.get<game_mob_skill_id>("skillid");
		mob_skill.level = row.get<game_mob_skill_level>("skill_level");
		mob_skill.effect_after = milliseconds{row.get<int16_t>("effect_delay")};

		bool found = false;
		for (auto &mob : m_skills) {
			if (mob.first == mob_id) {
				mob.second.push_back(mob_skill);
				found = true;
				break;
			}
		}
		if (!found) {
			vector<data::type::mob_skill_info> new_value{mob_skill};
			m_skills.push_back(std::make_pair(mob_id, new_value));
		}
	}
}

auto mob::load_mobs() -> void {
	m_mob_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::mob_data));

	for (const auto &row : rs) {
		auto mob = make_ref_ptr<data::type::mob_info>();

		mob->id = row.get<game_mob_id>("mobid");
		mob->level = row.get<uint16_t>("mob_level");
		mob->hp = row.get<uint32_t>("hp");
		mob->mp = row.get<uint32_t>("mp");
		mob->hp_recovery = row.get<uint32_t>("hp_recovery");
		mob->mp_recovery = row.get<uint32_t>("mp_recovery");
		mob->self_destruction = row.get<int32_t>("explode_hp");
		mob->exp = row.get<game_experience>("experience");
		mob->link = row.get<game_mob_id>("link");
		mob->buff = row.get<game_item_id>("death_buff");
		mob->remove_after = row.get<int32_t>("death_after");
		mob->hp_color = row.get<int8_t>("hp_bar_color");
		mob->hp_background_color = row.get<int8_t>("hp_bar_bg_color");
		mob->carnival_points = row.get<int8_t>("carnival_points");
		mob->avo = row.get<int16_t>("avoidability");
		mob->acc = row.get<int16_t>("accuracy");
		mob->speed = row.get<int16_t>("speed");
		mob->chase_speed = row.get<int16_t>("chase_speed");
		mob->w_atk = row.get<int16_t>("physical_attack");
		mob->w_def = row.get<int16_t>("physical_defense");
		mob->m_atk = row.get<int16_t>("magical_attack");
		mob->m_def = row.get<int16_t>("magical_defense");
		mob->traction = row.get<double>("traction");
		mob->damaged_by_skill = row.get<game_skill_id>("damaged_by_skill_only");
		mob->damaged_by_mob = row.get<game_mob_id>("damaged_by_mob_only");
		mob->knockback = row.get<int32_t>("knockback");
		mob->summon_type = row.get<int16_t>("summon_type");
		mob->fixed_damage = row.get<game_damage>("fixed_damage");

		auto get_element = [&row](const string &modifier) -> mob_elemental_attribute {
			mob_elemental_attribute ret;
			vana::util::str::run_enum(row.get<string>(modifier), [&ret](const string &cmp) {
				if (cmp == "normal") ret = mob_elemental_attribute::normal;
				else if (cmp == "immune") ret = mob_elemental_attribute::immune;
				else if (cmp == "strong") ret = mob_elemental_attribute::strong;
				else if (cmp == "weak") ret = mob_elemental_attribute::weak;
			});
			return ret;
		};

		mob->ice_attr = get_element("ice_modifier");
		mob->fire_attr = get_element("fire_modifier");
		mob->poison_attr = get_element("poison_modifier");
		mob->lightning_attr = get_element("lightning_modifier");
		mob->holy_attr = get_element("holy_modifier");
		mob->non_elem_attr = get_element("nonelemental_modifier");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&mob](const string &cmp) {
			if (cmp == "boss") mob->boss = true;
			else if (cmp == "undead") mob->undead = true;
			else if (cmp == "flying") mob->flying = true;
			else if (cmp == "friendly") mob->friendly = true;
			else if (cmp == "public_reward") mob->public_reward = true;
			else if (cmp == "explosive_reward") mob->explosive_reward = true;
			else if (cmp == "invincible") mob->invincible = true;
			else if (cmp == "auto_aggro") mob->auto_aggro = true;
			else if (cmp == "damaged_by_normal_attacks_only") mob->only_normal_attacks = true;
			else if (cmp == "no_remove_on_death") mob->keep_corpse = true;
			else if (cmp == "cannot_damage_player") mob->can_do_bump_damage = false;
			else if (cmp == "player_cannot_damage") mob->damageable = false;
		});

		mob->can_freeze = (!mob->boss && mob->ice_attr != mob_elemental_attribute::immune && mob->ice_attr != mob_elemental_attribute::strong);
		mob->can_poison = (!mob->boss && mob->poison_attr != mob_elemental_attribute::immune && mob->poison_attr != mob_elemental_attribute::strong);

		// Skill count relies on skills being loaded first
		for (const auto &skill : m_skills) {
			if (skill.first == mob->id) {
				mob->skill_count = static_cast<uint8_t>(skill.second.size());
				break;
			}
		}

		m_mob_info.push_back(mob);
	}
}

auto mob::load_summons() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::mob_summons));

	for (const auto &row : rs) {
		game_mob_id mob_id = row.get<game_mob_id>("mobid");
		game_mob_id summon_id = row.get<game_mob_id>("summonid");

		for (const auto &mob : m_mob_info) {
			if (mob->id == mob_id) {
				mob->summon.push_back(summon_id);
				break;
			}
		}
	}
}

auto mob::mob_exists(game_mob_id mob_id) const -> bool {
	for (const auto &mob : m_mob_info) {
		if (mob->id == mob_id) {
			return true;
		}
	}

	return false;
}

auto mob::get_mob_info(game_mob_id mob_id) const -> ref_ptr<const data::type::mob_info> {
	for (const auto &info : m_mob_info) {
		if (info->id == mob_id) {
			return info;
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

auto mob::get_mob_attack(game_mob_id mob_id, uint8_t index) const -> const data::type::mob_attack_info * const {
	for (const auto &mob : m_attacks) {
		if (mob.first == mob_id) {
			return &mob.second[index];
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

auto mob::get_mob_skill(game_mob_id mob_id, uint8_t index) const -> const data::type::mob_skill_info * const {
	for (const auto &mob : m_skills) {
		if (mob.first == mob_id) {
			return &mob.second[index];
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

auto mob::get_skills(game_mob_id mob_id) const -> const vector<data::type::mob_skill_info> & {
	for (const auto &mob : m_skills) {
		if (mob.first == mob_id) {
			return mob.second;
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

}
}
}