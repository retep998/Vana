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
#include "equip.hpp"
#include "common/constant/job/track.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

namespace vana {
namespace data {
namespace provider {

auto equip::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Equips... ";

	load_equips();

	std::cout << "DONE" << std::endl;
}

auto equip::load_equips() -> void {
	m_equip_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	// Ugly hack to get the integers instead of scientific notation
	// Note: This is MySQL's crappy behavior
	// It displays scientific notation for only very large values, meaning it's wildly inconsistent and hard to parse
	// We just use the string and send it to a translation function
	soci::rowset<> rs = (sql.prepare
		<< "SELECT *, REPLACE(FORMAT(equip_slots + 0, 0), \",\", \"\") AS equip_slot_flags "
		<< "FROM " << db.make_table(vana::data::table::item_equip_data));

	for (const auto &row : rs) {
		data::type::equip_info equip;

		equip.id = row.get<game_item_id>("itemid");
		equip.attack_speed = row.get<int8_t>("attack_speed");
		equip.healing = row.get<int8_t>("heal_hp");
		equip.slots = row.get<int8_t>("scroll_slots");
		equip.hp_bonus = row.get<game_health>("hp");
		equip.mp_bonus = row.get<game_health>("mp");
		equip.required_str = row.get<game_stat>("req_str");
		equip.required_dex = row.get<game_stat>("req_dex");
		equip.required_int = row.get<game_stat>("req_int");
		equip.required_luk = row.get<game_stat>("req_luk");
		equip.required_fame = row.get<game_fame>("req_fame");
		equip.str_bonus = row.get<game_stat>("strength");
		equip.dex_bonus = row.get<game_stat>("dexterity");
		equip.int_bonus = row.get<game_stat>("intelligence");
		equip.luk_bonus = row.get<game_stat>("luck");
		equip.hands_bonus = row.get<game_stat>("hands");
		equip.watk_bonus = row.get<game_stat>("weapon_attack");
		equip.wdef_bonus = row.get<game_stat>("weapon_defense");
		equip.matk_bonus = row.get<game_stat>("magic_attack");
		equip.mdef_bonus = row.get<game_stat>("magic_defense");
		equip.acc_bonus = row.get<game_stat>("accuracy");
		equip.avo_bonus = row.get<game_stat>("avoid");
		equip.jump_bonus = row.get<game_stat>("jump");
		equip.speed_bonus = row.get<game_stat>("speed");
		equip.taming_mob = row.get<uint8_t>("taming_mob");
		equip.ice_damage = row.get<uint8_t>("inc_ice_damage");
		equip.fire_damage = row.get<uint8_t>("inc_fire_damage");
		equip.lightning_damage = row.get<uint8_t>("inc_lightning_damage");
		equip.poison_damage = row.get<uint8_t>("inc_poison_damage");
		equip.elemental_default = row.get<uint8_t>("elemental_default");
		equip.traction = row.get<double>("traction");
		equip.valid_slots = vana::util::str::atoli(row.get<string>("equip_slot_flags").c_str());

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&equip](const string &cmp) {
			if (cmp == "wear_trade_block") equip.trade_block_on_equip = true;
		});
		vana::util::str::run_flags(row.get<opt_string>("req_job"), [&equip](const string &cmp) {
			if (cmp == "common") equip.valid_jobs.push_back(-1);
			else if (cmp == "beginner") equip.valid_jobs.push_back(constant::job::track::beginner);
			else if (cmp == "warrior") equip.valid_jobs.push_back(constant::job::track::warrior);
			else if (cmp == "magician") equip.valid_jobs.push_back(constant::job::track::magician);
			else if (cmp == "bowman") equip.valid_jobs.push_back(constant::job::track::bowman);
			else if (cmp == "thief") equip.valid_jobs.push_back(constant::job::track::thief);
			else if (cmp == "pirate") equip.valid_jobs.push_back(constant::job::track::pirate);
		});

		m_equip_info.push_back(equip);
	}
}

auto equip::set_equip_stats(vana::item *equip, stat_variance policy, bool is_gm, bool is_item_initialization) const -> void {
	const data::type::equip_info &ei = get_equip_info(equip->get_id());
	if (is_item_initialization) {
		equip->set_slots(ei.slots);
	}

	auto get_stat = 
		[policy, is_item_initialization, is_gm](int16_t base_equip_amount, int16_t equip_amount) -> int16_t {
			int16_t amount = is_item_initialization ? base_equip_amount : equip_amount;

			if (amount == 0 || policy == stat_variance::none) {
				return amount;
			}

			bool increase_only = false;
			if (policy == stat_variance::only_increase_with_great_chance) {
				if (!is_gm && vana::util::randomizer::rand<int8_t>(10, 1) <= 3) {
					return amount;
				}
				increase_only = true;
			}
			else if (policy == stat_variance::only_increase_with_amazing_chance) {
				if (!is_gm && vana::util::randomizer::rand<int8_t>(10, 1) == 1) {
					return amount;
				}
				increase_only = true;
			}

			int16_t variance = -1;
			switch (policy) {
				case stat_variance::gachapon:
					variance = std::min<int16_t>(amount / 5 + 1, 7);
					break;
				case stat_variance::chaos_normal:
					variance = 5;
					break;
				case stat_variance::chaos_high:
					variance = 7;
					break;
				default:
					variance = std::min<int16_t>(amount / 10 + 1, 5);
			}

			if (!is_gm) {
				// This code turns a single event into a series of events to give a normal distribution
				// e.g. 1 event of [0, 5] is turned into 7 events of [0, 1]
				// This makes it like flipping 7 coins instead of rolling a single die

				std::binomial_distribution<> dist{variance + 2, .5};
				variance = vana::util::randomizer::rand(dist) - 2;
			}

			if (variance <= 0) {
				return amount;
			}

			if (is_gm || increase_only || vana::util::randomizer::rand<bool>()) {
				return amount + variance;
			}

			return amount - variance;
		};

	equip->set_str(get_stat(ei.str_bonus, equip->get_str()));
	equip->set_dex(get_stat(ei.dex_bonus, equip->get_dex()));
	equip->set_int(get_stat(ei.int_bonus, equip->get_int()));
	equip->set_luk(get_stat(ei.luk_bonus, equip->get_luk()));
	equip->set_hp(get_stat(ei.hp_bonus, equip->get_hp()));
	equip->set_mp(get_stat(ei.mp_bonus, equip->get_mp()));
	equip->set_watk(get_stat(ei.watk_bonus, equip->get_watk()));
	equip->set_matk(get_stat(ei.matk_bonus, equip->get_matk()));
	equip->set_wdef(get_stat(ei.wdef_bonus, equip->get_wdef()));
	equip->set_mdef(get_stat(ei.mdef_bonus, equip->get_mdef()));
	equip->set_accuracy(get_stat(ei.acc_bonus, equip->get_accuracy()));
	equip->set_avoid(get_stat(ei.avo_bonus, equip->get_avoid()));
	equip->set_hands(get_stat(ei.hands_bonus, equip->get_hands()));
	equip->set_jump(get_stat(ei.jump_bonus, equip->get_jump()));
	equip->set_speed(get_stat(ei.speed_bonus, equip->get_speed()));
}

auto equip::can_equip(game_item_id item_id, game_gender_id gender, game_job_id job, game_stat str, game_stat dex, game_stat intl, game_stat luk, game_fame fame) const -> bool {
	const data::type::equip_info &ei = get_equip_info(item_id);
	return str >= ei.required_str && dex >= ei.required_dex && intl >= ei.required_int && luk >= ei.required_luk && fame >= ei.required_fame;
}

auto equip::is_valid_slot(game_item_id equip_id, game_inventory_slot target) const -> bool {
	const data::type::equip_info &ei = get_equip_info(equip_id);
	return (ei.valid_slots & (1ULL << (target - 1))) != 0;
}

auto equip::get_slots(game_item_id equip_id) const -> int8_t {
	return get_equip_info(equip_id).slots;
}

auto equip::get_equip_info(game_item_id equip_id) const -> const data::type::equip_info & {
	for (const auto &equip : m_equip_info) {
		if (equip.id == equip_id) {
			return equip;
		}
	}

	THROW_CODE_EXCEPTION(codepath_invalid_exception);
}

}
}
}