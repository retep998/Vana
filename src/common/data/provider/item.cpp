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
#include "item.hpp"
#include "common/algorithm.hpp"
#include "common/data/provider/buff.hpp"
#include "common/data/provider/equip.hpp"
#include "common/data/provider/shop.hpp"
#include "common/data/initialize.hpp"
#include "common/io/database.hpp"
#include "common/item.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

namespace vana {
namespace data {
namespace provider {

auto item::load_data(buff &provider) -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Items... ";

	load_items();
	load_consumes(provider);
	load_map_ranges();
	load_scrolls();
	load_monster_card_data();
	load_item_skills();
	load_summon_bags();
	load_item_rewards();
	load_pets();
	load_pet_interactions();

	std::cout << "DONE" << std::endl;
}

auto item::load_items() -> void {
	m_item_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT id.*, s.label "
		<< "FROM " << db.make_table(vana::data::table::item_data) << " id "
		<< "LEFT JOIN " << db.make_table(vana::data::table::strings) << " s ON id.itemid = s.objectid AND s.object_type = :item",
		soci::use(string{"item"}, "item"));

	for (const auto &row : rs) {
		data::type::item_info info;
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
			if (cmp == "time_limited") info.time_limited = true;
			else if (cmp == "cash_item") info.cash = true;
			else if (cmp == "no_trade") info.no_trade = true;
			else if (cmp == "no_sale") info.no_sale = true;
			else if (cmp == "karma_scissorable") info.karma_scissors = true;
			else if (cmp == "expire_on_logout") info.expire_on_logout = true;
			else if (cmp == "block_pickup") info.block_pickup = true;
			else if (cmp == "quest") info.quest = true;
		});

		info.id = row.get<game_item_id>("itemid");
		info.price = row.get<game_mesos>("price");
		info.max_slot = row.get<game_slot_qty>("max_slot_quantity");
		info.maker_level = row.get<game_skill_level>("level_for_maker");
		info.max_obtainable = row.get<int32_t>("max_possession_count");
		info.min_level = row.get<game_player_level>("min_level");
		info.max_level = row.get<game_player_level>("max_level");
		info.exp = row.get<game_experience>("experience");
		info.mesos = row.get<game_mesos>("money");
		info.npc = row.get<game_npc_id>("npc");
		info.name = row.get<string>("label");

		m_item_info.push_back(info);
	}
}

auto item::load_scrolls() -> void {
	m_scroll_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_scroll_data));

	for (const auto &row : rs) {
		data::type::scroll_info info;
		info.item_id = row.get<game_item_id>("itemid");
		info.success = row.get<uint16_t>("success");
		info.cursed = row.get<uint16_t>("break_item");
		info.str = row.get<game_stat>("istr");
		info.dex = row.get<game_stat>("idex");
		info.intl = row.get<game_stat>("iint");
		info.luk = row.get<game_stat>("iluk");
		info.hp = row.get<game_health>("ihp");
		info.mp = row.get<game_health>("imp");
		info.watk = row.get<game_stat>("iwatk");
		info.matk = row.get<game_stat>("imatk");
		info.wdef = row.get<game_stat>("iwdef");
		info.mdef = row.get<game_stat>("imdef");
		info.acc = row.get<game_stat>("iacc");
		info.avo = row.get<game_stat>("iavo");
		info.jump = row.get<game_stat>("ijump");
		info.speed = row.get<game_stat>("ispeed");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
			if (cmp == "rand_stat") info.rand_stat = true;
			else if (cmp == "recover_slot") info.recover = 1;
			else if (cmp == "warm_support") info.warm_support = true;
			else if (cmp == "prevent_slip") info.prevent_slip = true;
		});

		m_scroll_info.push_back(info);
	}
}

auto item::load_consumes(buff &provider) -> void {
	m_consume_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_random_morphs));

	hash_map<game_item_id, vector<data::type::morph_chance_info>> morph_data;
	for (const auto &row : rs) {
		data::type::morph_chance_info info;
		game_item_id item_id = row.get<game_item_id>("itemid");
		info.morph = row.get<game_morph_id>("morphid");
		info.chance = row.get<int8_t>("success");

		morph_data[item_id].push_back(info);
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_consume_data));

	for (const auto &row : rs) {
		data::type::consume_info info;
		info.item_id = row.get<game_item_id>("itemid");
		info.effect = row.get<uint8_t>("effect");
		info.hp = row.get<game_health>("hp");
		info.mp = row.get<game_health>("mp");
		info.hp_rate = row.get<int16_t>("hp_percentage");
		info.mp_rate = row.get<int16_t>("mp_percentage");
		info.move_to = row.get<game_map_id>("move_to");
		info.dec_hunger = row.get<uint8_t>("decrease_hunger");
		info.dec_fatigue = row.get<uint8_t>("decrease_fatigue");
		info.cp = row.get<uint8_t>("carnival_points");
		info.chance = row.get<uint16_t>("prob");
		info.buff_time = seconds{row.get<int32_t>("buff_time")};
		info.watk = row.get<game_stat>("weapon_attack");
		info.matk = row.get<game_stat>("magic_attack");
		info.wdef = row.get<game_stat>("weapon_defense");
		info.mdef = row.get<game_stat>("magic_defense");
		info.acc = row.get<game_stat>("accuracy");
		info.avo = row.get<game_stat>("avoid");
		info.speed = row.get<game_stat>("speed");
		info.jump = row.get<game_stat>("jump");

		game_morph_id morph_id = row.get<game_morph_id>("morph");
		if (morph_id != 0) {
			data::type::morph_chance_info morph;
			morph.morph = morph_id;
			morph.chance = 100;
			info.morphs.push_back(morph);
		}
		else {
			auto iter = morph_data.find(info.item_id);
			if (iter != std::end(morph_data)) {
				for (const auto &morph : iter->second) {
					info.morphs.push_back(morph);
				}
			}
		}

		info.ice_resist = row.get<int16_t>("defense_vs_ice");
		info.fire_resist = row.get<int16_t>("defense_vs_fire");
		info.lightning_resist = row.get<int16_t>("defense_vs_lightning");
		info.poison_resist = row.get<int16_t>("defense_vs_poison");
		info.stun_def = row.get<int16_t>("defense_vs_stun");
		info.darkness_def = row.get<int16_t>("defense_vs_darkness");
		info.weakness_def = row.get<int16_t>("defense_vs_weakness");
		info.seal_def = row.get<int16_t>("defense_vs_seal");
		info.curse_def = row.get<int16_t>("defense_vs_curse");

		vana::util::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
			if (cmp == "auto_consume") info.auto_consume = true;
			else if (cmp == "party_item") info.party = true;
			else if (cmp == "meso_up") info.meso_up = true;
			else if (cmp == "ignore_physical_defense") info.ignore_wdef = true;
			else if (cmp == "ignore_magical_defense") info.ignore_mdef = true;
			else if (cmp == "no_mouse_cancel") info.mouse_cancel = false;
			else if (cmp == "ignore_continent") info.ignore_continent = true;
			else if (cmp == "ghost") info.ghost = true;
			else if (cmp == "barrier") info.barrier = true;
			else if (cmp == "prevent_drowning") info.prevent_drown = true;
			else if (cmp == "prevent_freezing") info.prevent_freeze = true;
			else if (cmp == "override_traction") info.override_traction = true;
			else if (cmp == "drop_up_for_party") info.party_drop_up = true;
		});

		vana::util::str::run_flags(row.get<opt_string>("drop_up"), [&info, &row](const string &cmp) {
			if (cmp == "none") return;

			info.drop_up = true;
			if (cmp == "specific_item") info.drop_up_item = row.get<game_item_id>("drop_up_item");
			else if (cmp == "item_range") info.drop_up_item_range = row.get<int16_t>("drop_up_item_range");
		});

		vana::util::str::run_flags(row.get<opt_string>("cure_ailments"), [&info](const string &cmp) {
			if (cmp == "darkness") info.ailment |= 0x01;
			else if (cmp == "poison") info.ailment |= 0x02;
			else if (cmp == "curse") info.ailment |= 0x04;
			else if (cmp == "seal") info.ailment |= 0x08;
			else if (cmp == "weakness") info.ailment |= 0x10;
		});

		provider.add_item_info(info.item_id, info);
		m_consume_info.push_back(info);
	}
}

auto item::load_map_ranges() -> void {
	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_monster_card_map_ranges));

	for (const auto &row : rs) {
		data::type::card_map_range_info info;
		game_item_id item_id = row.get<game_item_id>("itemid");
		info.start_map = row.get<game_map_id>("start_map");
		info.end_map = row.get<game_map_id>("end_map");

		bool found = false;
		for (auto &item : m_consume_info) {
			if (item.item_id == item_id) {
				found = true;
				item.map_ranges.push_back(info);
				break;
			}
		}

		if (!found) THROW_CODE_EXCEPTION(codepath_invalid_exception);
	}
}

auto item::load_monster_card_data() -> void {
	m_mob_to_card_mapping.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::monster_card_data));

	for (const auto &row : rs) {
		game_item_id card_id = row.get<game_item_id>("cardid");
		game_mob_id mob_id = row.get<game_mob_id>("mobid");

		m_mob_to_card_mapping.emplace_back(card_id, mob_id);
	}
}

auto item::load_item_skills() -> void {
	m_skillbooks.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_skills));

	for (const auto &row : rs) {
		data::type::skillbook_info info;
		game_item_id item_id = row.get<game_item_id>("itemid");
		info.skill_id = row.get<game_skill_id>("skillid");
		info.req_level = row.get<game_skill_level>("req_skill_level");
		info.max_level = row.get<game_skill_level>("master_level");
		info.chance = row.get<int8_t>("chance");

		bool found = false;
		for (auto &book : m_skillbooks) {
			if (book.first == item_id) {
				found = true;
				book.second.push_back(info);
				break;
			}
		}

		if (!found) {
			vector<data::type::skillbook_info> current;
			current.push_back(info);
			m_skillbooks.emplace_back(item_id, current);
		}
	}
}

auto item::load_summon_bags() -> void {
	m_summon_bags.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_summons));

	for (const auto &row : rs) {
		data::type::summon_bag_info info;
		game_item_id item_id = row.get<game_item_id>("itemid");
		info.mob_id = row.get<game_mob_id>("mobid");
		info.chance = row.get<uint16_t>("chance");

		bool found = false;
		for (auto &summon : m_summon_bags) {
			if (summon.first == item_id) {
				found = true;
				summon.second.push_back(info);
				break;
			}
		}

		if (!found) {
			vector<data::type::summon_bag_info> current;
			current.push_back(info);
			m_summon_bags.emplace_back(item_id, current);
		}
	}
}

auto item::load_item_rewards() -> void {
	m_item_rewards.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_reward_data));

	for (const auto &row : rs) {
		data::type::item_reward_info info;
		game_item_id item_id = row.get<game_item_id>("itemid");
		info.reward_id = row.get<game_item_id>("rewardid");
		info.prob = row.get<uint16_t>("prob");
		info.quantity = row.get<int16_t>("quantity");
		info.effect = row.get<string>("effect");

		bool found = false;
		for (auto &reward : m_item_rewards) {
			if (reward.first == item_id) {
				found = true;
				reward.second.push_back(info);
				break;
			}
		}

		if (!found) {
			vector<data::type::item_reward_info> current;
			current.push_back(info);
			m_item_rewards.emplace_back(item_id, current);
		}
	}
}

auto item::load_pets() -> void {
	m_pet_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_pet_data));

	for (const auto &row : rs) {
		data::type::pet_info info;
		info.item_id = row.get<game_item_id>("itemid");
		info.name = row.get<string>("default_name");
		info.hunger = row.get<int32_t>("hunger");
		info.life = row.get<int32_t>("life");
		info.limited_life = row.get<int32_t>("limited_life");
		info.evolve_item = row.get<game_item_id>("evolution_item");
		info.evolve_level = row.get<int8_t>("req_level_for_evolution");
		vana::util::str::run_flags(row.get<opt_string>("flags"), [&info](const string &cmp) {
			if (cmp == "no_revive") info.no_revive = true;
			else if (cmp == "no_move_to_cash_shop") info.no_storing_in_cash_shop = true;
			else if (cmp == "auto_react") info.auto_react = true;
		});

		m_pet_info.push_back(info);
	}
}

auto item::load_pet_interactions() -> void {
	m_pet_interact_info.clear();

	auto &db = vana::io::database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table(vana::data::table::item_pet_interactions));

	for (const auto &row : rs) {
		data::type::pet_interact_info info;
		info.item_id = row.get<game_item_id>("itemid");
		info.command_id = row.get<int32_t>("command");
		info.increase = row.get<int16_t>("closeness");
		info.prob = row.get<uint32_t>("success");

		m_pet_interact_info.push_back(info);
	}
}

auto item::get_card_id(game_mob_id mob_id) const -> optional<game_item_id> {
	auto kvp =
		ext::find_value_ptr_if(m_mob_to_card_mapping, [&mob_id](auto value) { return value.second == mob_id; });

	if (kvp == nullptr) {
		return {};
	}
	return kvp->first;
}

auto item::get_mob_id(game_item_id card_id) const -> optional<game_mob_id> {
	auto kvp =
		ext::find_value_ptr_if(m_mob_to_card_mapping, [&card_id](auto value) { return value.first == card_id; });

	if (kvp == nullptr) {
		return {};
	}
	return kvp->second;
}

auto item::scroll_item(const equip &provider, game_item_id scroll_id, vana::item *equip, bool white_scroll, bool gm_scroller, int8_t &succeed, bool &cursed) const -> hacking_result {
	auto kvp = ext::find_value_ptr_if(
		m_scroll_info,
		[&scroll_id](auto value) { return value.item_id == scroll_id; });

	if (kvp == nullptr) {
		return hacking_result::definitely_hacking;
	}

	auto &item_info = *kvp;

	bool scroll_takes_slot = !(item_info.prevent_slip || item_info.warm_support || item_info.recover);
	if (scroll_takes_slot && equip->get_slots() == 0) {
		return hacking_result::definitely_hacking;
	}

	if (item_info.prevent_slip || item_info.warm_support) {
		succeed = 0;
		if (gm_scroller || vana::util::randomizer::percentage<uint16_t>() < item_info.success) {
			if (item_info.prevent_slip) {
				equip->set_prevent_slip(true);
			}
			else {
				equip->set_warm_support(true);
			}
			succeed = 1;
		}
	}
	else if (item_info.rand_stat) {
		if (equip->get_slots() > 0) {
			succeed = 0;
			if (gm_scroller || vana::util::randomizer::percentage<uint16_t>() < item_info.success) {
				provider.set_equip_stats(equip, stat_variance::chaos_normal, gm_scroller, false);

				equip->inc_scrolls();
				succeed = 1;
			}
		}
	}
	else if (item_info.recover > 0) {
		// Apparently global doesn't let you use these scrolls on hammer slots
		//int8_t maxSlots = provider.get_slots(equip->get_id()) + static_cast<int8_t>(equip->get_hammers());
		int8_t max_slots = provider.get_slots(equip->get_id());
		int8_t max_recoverable_slots = max_slots - equip->get_scrolls();
		int8_t recover_slots = std::min(item_info.recover, max_recoverable_slots);
		if (recover_slots > 0) {
			succeed = 0;
			if (gm_scroller || vana::util::randomizer::percentage<uint16_t>() < item_info.success) {
				// Give back slot(s)
				equip->inc_slots(recover_slots);
				succeed = 1;
			}
		}
	}
	else {
		if (vana::util::game_logic::item::item_type_to_scroll_type(equip->get_id()) != vana::util::game_logic::item::get_scroll_type(scroll_id)) {
			// Hacking, equip slot different from the scroll slot
			return hacking_result::definitely_hacking;
		}
		if (equip->get_slots() > 0) {
			succeed = 0;
			if (gm_scroller || vana::util::randomizer::percentage<uint16_t>() < item_info.success) {
				succeed = 1;
				equip->add_str(item_info.str);
				equip->add_dex(item_info.dex);
				equip->add_int(item_info.intl);
				equip->add_luk(item_info.luk);
				equip->add_hp(item_info.hp);
				equip->add_mp(item_info.mp);
				equip->add_watk(item_info.watk);
				equip->add_matk(item_info.matk);
				equip->add_wdef(item_info.wdef);
				equip->add_mdef(item_info.mdef);
				equip->add_accuracy(item_info.acc);
				equip->add_avoid(item_info.avo);
				equip->add_hands(item_info.hand);
				equip->add_jump(item_info.jump);
				equip->add_speed(item_info.speed);
				equip->inc_scrolls();
			}
		}
	}

	if (succeed == 0) {
		if (item_info.cursed > 0 && vana::util::randomizer::percentage<uint16_t>() < item_info.cursed) {
			cursed = true;
		}
	}

	if (scroll_takes_slot && (!white_scroll || succeed == 1)) {
		equip->dec_slots();
	}

	return hacking_result::not_hacking;
}

auto item::get_item_info(game_item_id item_id) const -> const data::type::item_info * const {
	return ext::find_value_ptr_if(
		m_item_info,
		[&item_id](auto value) { return value.id == item_id; });
}

auto item::get_consume_info(game_item_id item_id) const -> const data::type::consume_info * const {
	return ext::find_value_ptr_if(
		m_consume_info,
		[&item_id](auto value) { return value.item_id == item_id; });
}

auto item::get_pet_info(game_item_id item_id) const -> const data::type::pet_info * const {
	return ext::find_value_ptr_if(
		m_pet_info,
		[&item_id](auto value) { return value.item_id == item_id; });
}

auto item::get_interaction(game_item_id item_id, int32_t action) const -> const data::type::pet_interact_info * const {
	return ext::find_value_ptr_if(
		m_pet_interact_info,
		[&item_id, &action](auto value) { return value.item_id == item_id && value.command_id == action; });
}

auto item::get_item_skills(game_item_id item_id) const -> const vector<data::type::skillbook_info> * const {
	return &ext::find_value_ptr_if(
		m_skillbooks,
		[&item_id](auto value) { return value.first == item_id; })->second;
}

auto item::get_item_rewards(game_item_id item_id) const -> const vector<data::type::item_reward_info> * const {
	return &ext::find_value_ptr_if(
		m_item_rewards,
		[&item_id](auto value) { return value.first == item_id; })->second;
}

auto item::get_item_summons(game_item_id item_id) const -> const vector<data::type::summon_bag_info> * const {
	return &ext::find_value_ptr_if(
		m_summon_bags,
		[&item_id](auto value) { return value.first == item_id; })->second;
}

}
}
}