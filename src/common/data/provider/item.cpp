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
#include "common/database.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/initialize_common.hpp"
#include "common/item.hpp"
#include "common/randomizer.hpp"
#include "common/string_utilities.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

namespace vana {
namespace data {
namespace provider {

auto item::load_data(buff &provider) -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Items... ";

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

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare
		<< "SELECT id.*, s.label "
		<< "FROM " << db.make_table("item_data") << " id "
		<< "LEFT JOIN " << db.make_table("strings") << " s ON id.itemid = s.objectid AND s.object_type = :item",
		soci::use(string{"item"}, "item"));

	for (const auto &row : rs) {
		data::type::item_info item;
		utilities::str::run_flags(row.get<opt_string>("flags"), [&item](const string &cmp) {
			if (cmp == "time_limited") item.time_limited = true;
			else if (cmp == "cash_item") item.cash = true;
			else if (cmp == "no_trade") item.no_trade = true;
			else if (cmp == "no_sale") item.no_sale = true;
			else if (cmp == "karma_scissorable") item.karma_scissors = true;
			else if (cmp == "expire_on_logout") item.expire_on_logout = true;
			else if (cmp == "block_pickup") item.block_pickup = true;
			else if (cmp == "quest") item.quest = true;
		});

		game_item_id item_id = row.get<game_item_id>("itemid");
		item.price = row.get<game_mesos>("price");
		item.max_slot = row.get<game_slot_qty>("max_slot_quantity");
		item.maker_level = row.get<game_skill_level>("level_for_maker");
		item.max_obtainable = row.get<int32_t>("max_possession_count");
		item.min_level = row.get<game_player_level>("min_level");
		item.max_level = row.get<game_player_level>("max_level");
		item.exp = row.get<game_experience>("experience");
		item.mesos = row.get<game_mesos>("money");
		item.npc = row.get<game_npc_id>("npc");
		item.name = row.get<string>("label");

		m_item_info[item_id] = item;
	}
}

auto item::load_scrolls() -> void {
	m_scroll_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_scroll_data"));

	for (const auto &row : rs) {
		data::type::scroll_info item;
		game_item_id item_id = row.get<game_item_id>("itemid");
		item.success = row.get<uint16_t>("success");
		item.cursed = row.get<uint16_t>("break_item");
		item.str = row.get<game_stat>("istr");
		item.dex = row.get<game_stat>("idex");
		item.intl = row.get<game_stat>("iint");
		item.luk = row.get<game_stat>("iluk");
		item.hp = row.get<game_health>("ihp");
		item.mp = row.get<game_health>("imp");
		item.watk = row.get<game_stat>("iwatk");
		item.matk = row.get<game_stat>("imatk");
		item.wdef = row.get<game_stat>("iwdef");
		item.mdef = row.get<game_stat>("imdef");
		item.acc = row.get<game_stat>("iacc");
		item.avo = row.get<game_stat>("iavo");
		item.jump = row.get<game_stat>("ijump");
		item.speed = row.get<game_stat>("ispeed");

		utilities::str::run_flags(row.get<opt_string>("flags"), [&item](const string &cmp) {
			if (cmp == "rand_stat") item.rand_stat = true;
			else if (cmp == "recover_slot") item.recover = 1;
			else if (cmp == "warm_support") item.warm_support = true;
			else if (cmp == "prevent_slip") item.prevent_slip = true;
		});

		m_scroll_info[item_id] = item;
	}
}

auto item::load_consumes(buff &provider) -> void {
	m_consume_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_random_morphs"));

	hash_map<game_item_id, vector<data::type::morph_chance_info>> morph_data;
	for (const auto &row : rs) {
		data::type::morph_chance_info morph;
		game_item_id item_id = row.get<game_item_id>("itemid");
		morph.morph = row.get<game_morph_id>("morphid");
		morph.chance = row.get<int8_t>("success");

		morph_data[item_id].push_back(morph);
	}

	rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_consume_data"));

	for (const auto &row : rs) {
		data::type::consume_info item;
		game_item_id item_id = row.get<game_item_id>("itemid");
		item.effect = row.get<uint8_t>("effect");
		item.hp = row.get<game_health>("hp");
		item.mp = row.get<game_health>("mp");
		item.hp_rate = row.get<int16_t>("hp_percentage");
		item.mp_rate = row.get<int16_t>("mp_percentage");
		item.move_to = row.get<game_map_id>("move_to");
		item.dec_hunger = row.get<uint8_t>("decrease_hunger");
		item.dec_fatigue = row.get<uint8_t>("decrease_fatigue");
		item.cp = row.get<uint8_t>("carnival_points");
		item.chance = row.get<uint16_t>("prob");
		item.buff_time = seconds{row.get<int32_t>("buff_time")};
		item.watk = row.get<game_stat>("weapon_attack");
		item.matk = row.get<game_stat>("magic_attack");
		item.wdef = row.get<game_stat>("weapon_defense");
		item.mdef = row.get<game_stat>("magic_defense");
		item.acc = row.get<game_stat>("accuracy");
		item.avo = row.get<game_stat>("avoid");
		item.speed = row.get<game_stat>("speed");
		item.jump = row.get<game_stat>("jump");

		game_morph_id morph_id = row.get<game_morph_id>("morph");
		if (morph_id != 0) {
			data::type::morph_chance_info morph;
			morph.morph = morph_id;
			morph.chance = 100;
			item.morphs.push_back(morph);
		}
		else {
			auto iter = morph_data.find(item_id);
			if (iter != std::end(morph_data)) {
				for (const auto &morph : iter->second) {
					item.morphs.push_back(morph);
				}
			}
		}

		item.ice_resist = row.get<int16_t>("defense_vs_ice");
		item.fire_resist = row.get<int16_t>("defense_vs_fire");
		item.lightning_resist = row.get<int16_t>("defense_vs_lightning");
		item.poison_resist = row.get<int16_t>("defense_vs_poison");
		item.stun_def = row.get<int16_t>("defense_vs_stun");
		item.darkness_def = row.get<int16_t>("defense_vs_darkness");
		item.weakness_def = row.get<int16_t>("defense_vs_weakness");
		item.seal_def = row.get<int16_t>("defense_vs_seal");
		item.curse_def = row.get<int16_t>("defense_vs_curse");

		utilities::str::run_flags(row.get<opt_string>("flags"), [&item](const string &cmp) {
			if (cmp == "auto_consume") item.auto_consume = true;
			else if (cmp == "party_item") item.party = true;
			else if (cmp == "meso_up") item.meso_up = true;
			else if (cmp == "ignore_physical_defense") item.ignore_wdef = true;
			else if (cmp == "ignore_magical_defense") item.ignore_mdef = true;
			else if (cmp == "no_mouse_cancel") item.mouse_cancel = false;
			else if (cmp == "ignore_continent") item.ignore_continent = true;
			else if (cmp == "ghost") item.ghost = true;
			else if (cmp == "barrier") item.barrier = true;
			else if (cmp == "prevent_drowning") item.prevent_drown = true;
			else if (cmp == "prevent_freezing") item.prevent_freeze = true;
			else if (cmp == "override_traction") item.override_traction = true;
			else if (cmp == "drop_up_for_party") item.party_drop_up = true;
		});

		utilities::str::run_flags(row.get<opt_string>("drop_up"), [&item, &row](const string &cmp) {
			if (cmp == "none") return;

			item.drop_up = true;
			if (cmp == "specific_item") item.drop_up_item = row.get<game_item_id>("drop_up_item");
			else if (cmp == "item_range") item.drop_up_item_range = row.get<int16_t>("drop_up_item_range");
		});

		utilities::str::run_flags(row.get<opt_string>("cure_ailments"), [&item](const string &cmp) {
			if (cmp == "darkness") item.ailment |= 0x01;
			else if (cmp == "poison") item.ailment |= 0x02;
			else if (cmp == "curse") item.ailment |= 0x04;
			else if (cmp == "seal") item.ailment |= 0x08;
			else if (cmp == "weakness") item.ailment |= 0x10;
		});

		provider.add_item_info(item_id, item);
		m_consume_info[item_id] = item;
	}
}

auto item::load_map_ranges() -> void {
	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_monster_card_map_ranges"));

	for (const auto &row : rs) {
		data::type::card_map_range_info range;
		game_item_id item_id = row.get<game_item_id>("itemid");
		range.start_map = row.get<game_map_id>("start_map");
		range.end_map = row.get<game_map_id>("end_map");

		m_consume_info[item_id].map_ranges.push_back(range);
	}
}

auto item::load_monster_card_data() -> void {
	m_cards_to_mobs.clear();
	m_mobs_to_cards.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("monster_card_data"));

	for (const auto &row : rs) {
		game_item_id card_id = row.get<game_item_id>("cardid");
		game_mob_id mob_id = row.get<game_mob_id>("mobid");

		m_cards_to_mobs.emplace(card_id, mob_id);
		m_mobs_to_cards.emplace(mob_id, card_id);
	}
}

auto item::load_item_skills() -> void {
	m_skillbooks.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_skills"));

	for (const auto &row : rs) {
		data::type::skillbook_info book;
		game_item_id item_id = row.get<game_item_id>("itemid");
		book.skill_id = row.get<game_skill_id>("skillid");
		book.req_level = row.get<game_skill_level>("req_skill_level");
		book.max_level = row.get<game_skill_level>("master_level");
		book.chance = row.get<int8_t>("chance");

		m_skillbooks[item_id].push_back(book);
	}
}

auto item::load_summon_bags() -> void {
	m_summon_bags.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_summons"));

	for (const auto &row : rs) {
		data::type::summon_bag_info summon;
		game_item_id item_id = row.get<game_item_id>("itemid");
		summon.mob_id = row.get<game_mob_id>("mobid");
		summon.chance = row.get<uint16_t>("chance");

		m_summon_bags[item_id].push_back(summon);
	}
}

auto item::load_item_rewards() -> void {
	m_item_rewards.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_reward_data"));

	for (const auto &row : rs) {
		data::type::item_reward_info reward;
		game_item_id item_id = row.get<game_item_id>("itemid");
		reward.reward_id = row.get<game_item_id>("rewardid");
		reward.prob = row.get<uint16_t>("prob");
		reward.quantity = row.get<int16_t>("quantity");
		reward.effect = row.get<string>("effect");

		m_item_rewards[item_id].push_back(reward);
	}
}

auto item::load_pets() -> void {
	m_pet_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_pet_data"));

	for (const auto &row : rs) {
		data::type::pet_info pet;
		game_item_id item_id = row.get<game_item_id>("itemid");
		pet.name = row.get<string>("default_name");
		pet.hunger = row.get<int32_t>("hunger");
		pet.life = row.get<int32_t>("life");
		pet.limited_life = row.get<int32_t>("limited_life");
		pet.evolve_item = row.get<game_item_id>("evolution_item");
		pet.evolve_level = row.get<int8_t>("req_level_for_evolution");
		utilities::str::run_flags(row.get<opt_string>("flags"), [&pet](const string &cmp) {
			if (cmp == "no_revive") pet.no_revive = true;
			else if (cmp == "no_move_to_cash_shop") pet.no_storing_in_cash_shop = true;
			else if (cmp == "auto_react") pet.auto_react = true;
		});

		m_pet_info[item_id] = pet;
	}
}

auto item::load_pet_interactions() -> void {
	m_pet_interact_info.clear();

	auto &db = database::get_data_db();
	auto &sql = db.get_session();
	soci::rowset<> rs = (sql.prepare << "SELECT * FROM " << db.make_table("item_pet_interactions"));

	for (const auto &row : rs) {
		data::type::pet_interact_info interaction;
		game_item_id item_id = row.get<game_item_id>("itemid");
		int32_t command_id = row.get<int32_t>("command");
		interaction.increase = row.get<int16_t>("closeness");
		interaction.prob = row.get<uint32_t>("success");

		m_pet_interact_info[item_id][command_id] = interaction;
	}
}

auto item::get_card_id(game_mob_id mob_id) const -> optional<game_item_id> {
	auto kvp = m_mobs_to_cards.find(mob_id);
	if (kvp == std::end(m_mobs_to_cards)) {
		return {};
	}
	return kvp->second;
}

auto item::get_mob_id(game_item_id card_id) const -> optional<game_mob_id> {
	auto kvp = m_cards_to_mobs.find(card_id);
	if (kvp == std::end(m_cards_to_mobs)) {
		return {};
	}
	return kvp->second;
}

auto item::scroll_item(const equip &provider, game_item_id scroll_id, vana::item *equip, bool white_scroll, bool gm_scroller, int8_t &succeed, bool &cursed) const -> hacking_result {
	auto kvp = m_scroll_info.find(scroll_id);
	if (kvp == std::end(m_scroll_info)) {
		return hacking_result::definitely_hacking;
	}

	auto &item_info = kvp->second;

	bool scroll_takes_slot = !(item_info.prevent_slip || item_info.warm_support || item_info.recover);
	if (scroll_takes_slot && equip->get_slots() == 0) {
		return hacking_result::definitely_hacking;
	}

	if (item_info.prevent_slip || item_info.warm_support) {
		succeed = 0;
		if (gm_scroller || randomizer::percentage<uint16_t>() < item_info.success) {
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
			if (gm_scroller || randomizer::percentage<uint16_t>() < item_info.success) {
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
			if (gm_scroller || randomizer::percentage<uint16_t>() < item_info.success) {
				// Give back slot(s)
				equip->inc_slots(recover_slots);
				succeed = 1;
			}
		}
	}
	else {
		if (game_logic_utilities::item_type_to_scroll_type(equip->get_id()) != game_logic_utilities::get_scroll_type(scroll_id)) {
			// Hacking, equip slot different from the scroll slot
			return hacking_result::definitely_hacking;
		}
		if (equip->get_slots() > 0) {
			succeed = 0;
			if (gm_scroller || randomizer::percentage<uint16_t>() < item_info.success) {
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
		if (item_info.cursed > 0 && randomizer::percentage<uint16_t>() < item_info.cursed) {
			cursed = true;
		}
	}

	if (scroll_takes_slot && (!white_scroll || succeed == 1)) {
		equip->dec_slots();
	}

	return hacking_result::not_hacking;
}

auto item::get_item_info(game_item_id item_id) const -> const data::type::item_info * const {
	return ext::find_value_ptr(m_item_info, item_id);
}

auto item::get_consume_info(game_item_id item_id) const -> const data::type::consume_info * const {
	return ext::find_value_ptr(m_consume_info, item_id);
}

auto item::get_pet_info(game_item_id item_id) const -> const data::type::pet_info * const {
	return ext::find_value_ptr(m_pet_info, item_id);
}

auto item::get_interaction(game_item_id item_id, int32_t action) const -> const data::type::pet_interact_info * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_pet_interact_info, item_id), action);
}

auto item::get_item_skills(game_item_id item_id) const -> const vector<data::type::skillbook_info> * const {
	return ext::find_value_ptr(m_skillbooks, item_id);
}

auto item::get_item_rewards(game_item_id item_id) const -> const vector<data::type::item_reward_info> * const {
	return ext::find_value_ptr(m_item_rewards, item_id);
}

auto item::get_item_summons(game_item_id item_id) const -> const vector<data::type::summon_bag_info> * const {
	return ext::find_value_ptr(m_summon_bags, item_id);
}

}
}
}