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
#include "player_skills.hpp"
#include "common/algorithm.hpp"
#include "common/data/provider/skill.hpp"
#include "common/io/database.hpp"
#include "common/timer/id.hpp"
#include "common/timer/timer.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/game_logic/job.hpp"
#include "common/util/game_logic/player_skill.hpp"
#include "common/util/randomizer.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/map.hpp"
#include "channel_server/map_packet.hpp"
#include "channel_server/mystic_door.hpp"
#include "channel_server/party.hpp"
#include "channel_server/party_packet.hpp"
#include "channel_server/player.hpp"
#include "channel_server/skills.hpp"
#include "channel_server/skills_packet.hpp"

namespace vana {
namespace channel_server {

player_skills::player_skills(ref_ptr<player> player) :
	m_player{player}
{
	load();
}

auto player_skills::load() -> void {
	if (auto player = m_player.lock()) {
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();
		player_skill_info skill;
		game_player_id player_id = player->get_id();
		game_skill_id skill_id = 0;

		soci::rowset<> rs = (sql.prepare
			<< "SELECT s.skill_id, s.points, s.max_level "
			<< "FROM " << db.make_table(vana::table::skills) << " s "
			<< "WHERE s.character_id = :char",
			soci::use(player_id, "char"));

		for (const auto &row : rs) {
			skill_id = row.get<game_skill_id>("skill_id");
			if (vana::util::game_logic::player_skill::is_blessing_of_the_fairy(skill_id)) {
				continue;
			}

			skill = player_skill_info{};
			skill.level = row.get<game_skill_level>("points");
			skill.max_skill_level = channel_server::get_instance().get_skill_data_provider().get_max_level(skill_id);
			skill.player_max_skill_level = row.get<game_skill_level>("max_level");
			m_skills[skill_id] = skill;
		}

		rs = (sql.prepare
			<< "SELECT c.* "
			<< "FROM " << db.make_table(vana::table::cooldowns) << " c "
			<< "WHERE c.character_id = :char",
			soci::use(player_id, "char"));

		for (const auto &row : rs) {
			game_skill_id skill_id = row.get<game_skill_id>("skill_id");
			seconds time_left = seconds{row.get<int16_t>("remaining_time")};
			skills::start_cooldown(player, skill_id, time_left, true);
			m_cooldowns[skill_id] = time_left;
		}

		skill_id = get_blessing_of_the_fairy();

		opt_string blessing_player_name;
		optional<game_player_level> blessing_player_level;

		game_account_id account_id = player->get_account_id();
		game_world_id world_id = player->get_world_id();

		// TODO FIXME skill
		// Allow Cygnus <-> Adventurer selection here or allow it to be ignored
		// That is, some versions only allowed Adv. Blessing to be populated by Cygnus levels and vice versa
		// Some later versions lifted this restriction entirely
		sql.once
			<< "SELECT c.name, c.level "
			<< "FROM " << db.make_table(vana::table::characters) << " c "
			<< "WHERE c.world_id = :world AND c.account_id = :account AND c.character_id <> :char "
			<< "ORDER BY c.level DESC "
			<< "LIMIT 1 ",
			soci::use(account_id, "account"),
			soci::use(world_id, "world"),
			soci::use(player_id, "char"),
			soci::into(blessing_player_name),
			soci::into(blessing_player_level);

		if (blessing_player_level.is_initialized()) {
			skill = player_skill_info{};
			skill.max_skill_level = channel_server::get_instance().get_skill_data_provider().get_max_level(skill_id);
			skill.level = std::min<game_skill_level>(blessing_player_level.get() / 10, skill.max_skill_level);
			m_blessing_player = blessing_player_name.get();
			m_skills[skill_id] = skill;
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::save(bool save_cooldowns) -> void {
	if (auto player = m_player.lock()) {
		using namespace soci;
		game_player_id player_id = player->get_id();
		auto &db = vana::io::database::get_char_db();
		auto &sql = db.get_session();

		game_skill_id skill_id = 0;
		game_skill_level level = 0;
		game_skill_level max_level = 0;
		statement st = (sql.prepare
			<< "REPLACE INTO " << db.make_table(vana::table::skills) << " VALUES (:player, :skill, :level, :max_level)",
			use(player_id, "player"),
			use(skill_id, "skill"),
			use(level, "level"),
			use(max_level, "max_level"));

		for (const auto &kvp : m_skills) {
			if (vana::util::game_logic::player_skill::is_blessing_of_the_fairy(kvp.first)) {
				continue;
			}
			skill_id = kvp.first;
			level = kvp.second.level;
			max_level = kvp.second.player_max_skill_level;
			st.execute(true);
		}

		if (save_cooldowns) {
			sql.once << "DELETE FROM " << db.make_table(vana::table::cooldowns) << " WHERE character_id = :char",
				soci::use(player_id, "char");

			if (m_cooldowns.size() > 0) {
				int16_t remaining_time = 0;
				st = (sql.prepare
					<< "INSERT INTO " << db.make_table(vana::table::cooldowns) << " (character_id, skill_id, remaining_time) "
					<< "VALUES (:char, :skill, :time)",
					use(player_id, "char"),
					use(skill_id, "skill"),
					use(remaining_time, "time"));

				for (const auto &kvp : m_cooldowns) {
					skill_id = kvp.first;
					remaining_time = skills::get_cooldown_time_left(player, kvp.first);
					st.execute(true);
				}
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::add_skill_level(game_skill_id skill_id, game_skill_level amount, bool send_packet) -> bool {
	if (!channel_server::get_instance().get_skill_data_provider().is_valid_skill(skill_id)) {
		return false;
	}

	if (auto player = m_player.lock()) {
		// Keep people from adding too much SP and prevent it from going negative
		auto kvp = m_skills.find(skill_id);
		game_skill_level new_level = (kvp != std::end(m_skills) ? kvp->second.level : 0) + amount;
		game_skill_level max_skill_level = channel_server::get_instance().get_skill_data_provider().get_max_level(skill_id);
		if (new_level > max_skill_level || (vana::util::game_logic::player_skill::is_fourth_job_skill(skill_id) && new_level > get_max_skill_level(skill_id))) {
			return false;
		}

		m_skills[skill_id].level = new_level;
		m_skills[skill_id].max_skill_level = max_skill_level;
		if (send_packet) {
			player->send(packets::skills::add_skill(skill_id, m_skills[skill_id]));
		}
		return true;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_skill_level(game_skill_id skill_id) const -> game_skill_level {
	auto skill = ext::find_value_ptr(m_skills, skill_id);
	return skill == nullptr ? 0 : skill->level;
}

auto player_skills::set_max_skill_level(game_skill_id skill_id, game_skill_level max_level, bool send_packet) -> void {
	// Set max level for 4th job skills
	m_skills[skill_id].player_max_skill_level = max_level;

	if (send_packet) {
		if (auto player = m_player.lock()) {
			player->get_skills()->add_skill_level(skill_id, 0);
		}
		else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
	}
}

auto player_skills::get_max_skill_level(game_skill_id skill_id) const -> game_skill_level {
	// Get max level for 4th job skills
	if (m_skills.find(skill_id) != std::end(m_skills)) {
		const player_skill_info &info = m_skills.find(skill_id)->second;
		if (vana::util::game_logic::player_skill::is_fourth_job_skill(skill_id)) {
			return info.player_max_skill_level;
		}
		return info.max_skill_level;
	}
	return 0;
}

auto player_skills::get_skill_info(game_skill_id skill_id) const -> const data::type::skill_level_info * const {
	auto skill = ext::find_value_ptr(m_skills, skill_id);
	return skill == nullptr ? nullptr : channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, skill->level);
}

auto player_skills::has_skill(game_skill_id skill_id) const -> bool {
	return skill_id != 0 && get_skill_level(skill_id) > 0;
}

auto player_skills::has_elemental_amp() const -> bool {
	return has_skill(get_elemental_amp());
}

auto player_skills::has_achilles() const -> bool {
	return has_skill(get_achilles());
}

auto player_skills::has_energy_charge() const -> bool {
	return has_skill(get_energy_charge());
}

auto player_skills::has_hp_increase() const -> bool {
	return has_skill(get_hp_increase());
}

auto player_skills::has_mp_increase() const -> bool {
	return has_skill(get_mp_increase());
}

auto player_skills::has_venomous_weapon() const -> bool {
	return has_skill(get_venomous_weapon());
}

auto player_skills::has_dark_sight_interruption_skill() const -> bool {
	return has_skill(get_dark_sight_interruption_skill());
}

auto player_skills::has_no_damage_skill() const -> bool {
	return has_skill(get_no_damage_skill());
}

auto player_skills::has_follow_the_lead() const -> bool {
	return has_skill(get_follow_the_lead());
}

auto player_skills::has_legendary_spirit() const -> bool {
	return has_skill(get_legendary_spirit());
}

auto player_skills::has_maker() const -> bool {
	return has_skill(get_maker());
}

auto player_skills::has_blessing_of_the_fairy() const -> bool {
	return has_skill(get_blessing_of_the_fairy());
}

auto player_skills::get_elemental_amp() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::fp_mage:
			case constant::job::id::fp_arch_mage: skill_id = constant::skill::fp_mage::element_amplification; break;
			case constant::job::id::il_mage:
			case constant::job::id::il_arch_mage: skill_id = constant::skill::il_mage::element_amplification; break;
			case constant::job::id::blaze_wizard3:
			case constant::job::id::blaze_wizard4: skill_id = constant::skill::blaze_wizard::element_amplification; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_achilles() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::hero: skill_id = constant::skill::hero::achilles; break;
			case constant::job::id::paladin: skill_id = constant::skill::paladin::achilles; break;
			case constant::job::id::dark_knight: skill_id = constant::skill::dark_knight::achilles; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_energy_charge() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::marauder:
			case constant::job::id::buccaneer: skill_id = constant::skill::marauder::energy_charge; break;
			case constant::job::id::thunder_breaker2:
			case constant::job::id::thunder_breaker3:
			case constant::job::id::thunder_breaker4: skill_id = constant::skill::thunder_breaker::energy_charge; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_advanced_combo() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::hero: skill_id = constant::skill::hero::advanced_combo_attack; break;
			case constant::job::id::dawn_warrior3:
			case constant::job::id::dawn_warrior4: skill_id = constant::skill::dawn_warrior::advanced_combo; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_alchemist() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::hermit:
			case constant::job::id::night_lord: skill_id = constant::skill::hermit::alchemist; break;
			case constant::job::id::night_walker3:
			case constant::job::id::night_walker4: skill_id = constant::skill::night_walker::alchemist; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_hp_increase() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::job::get_job_track(player->get_stats()->get_job())) {
			case constant::job::track::warrior: skill_id = constant::skill::swordsman::improved_max_hp_increase; break;
			case constant::job::track::dawn_warrior: skill_id = constant::skill::dawn_warrior::max_hp_enhancement; break;
			case constant::job::track::thunder_breaker: skill_id = constant::skill::thunder_breaker::improve_max_hp; break;
			case constant::job::track::pirate:
				if ((player->get_stats()->get_job() / 10) == (constant::job::id::brawler / 10)) {
					skill_id = constant::skill::brawler::improve_max_hp;
				}
				break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_mp_increase() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::job::get_job_track(player->get_stats()->get_job())) {
			case constant::job::track::magician: skill_id = constant::skill::magician::improved_max_mp_increase; break;
			case constant::job::track::blaze_wizard: skill_id = constant::skill::blaze_wizard::increasing_max_mp; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_mastery() const -> game_skill_id {
	game_skill_id mastery_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::item::get_item_type(player->get_inventory()->get_equipped_id(constant::equip_slot::weapon))) {
			case constant::item::type::weapon_1h_sword:
			case constant::item::type::weapon_2h_sword:
				switch (player->get_stats()->get_job()) {
					case constant::job::id::fighter:
					case constant::job::id::crusader:
					case constant::job::id::hero: mastery_id = constant::skill::fighter::sword_mastery; break;
					case constant::job::id::page:
					case constant::job::id::white_knight:
					case constant::job::id::paladin: mastery_id = constant::skill::page::sword_mastery; break;
				}
				break;
			case constant::item::type::weapon_1h_axe:
			case constant::item::type::weapon_2h_axe: mastery_id = constant::skill::fighter::axe_mastery; break;
			case constant::item::type::weapon_1h_mace:
			case constant::item::type::weapon_2h_mace: mastery_id = constant::skill::page::bw_mastery; break;
			case constant::item::type::weapon_spear: mastery_id = constant::skill::spearman::spear_mastery; break;
			case constant::item::type::weapon_polearm: mastery_id = constant::skill::spearman::polearm_mastery; break;
			case constant::item::type::weapon_dagger: mastery_id = constant::skill::bandit::dagger_mastery; break;
			case constant::item::type::weapon_knuckle: mastery_id = constant::skill::brawler::knuckler_mastery; break;
			case constant::item::type::weapon_bow: mastery_id = constant::skill::hunter::bow_mastery; break;
			case constant::item::type::weapon_crossbow: mastery_id = constant::skill::crossbowman::crossbow_mastery; break;
			case constant::item::type::weapon_claw: mastery_id = constant::skill::assassin::claw_mastery; break;
			case constant::item::type::weapon_gun: mastery_id = constant::skill::gunslinger::gun_mastery; break;
		}
		return mastery_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_mp_eater() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::fp_wizard:
			case constant::job::id::fp_mage:
			case constant::job::id::fp_arch_mage: skill_id = constant::skill::fp_wizard::mp_eater; break;
			case constant::job::id::il_wizard:
			case constant::job::id::il_mage:
			case constant::job::id::il_arch_mage: skill_id = constant::skill::il_wizard::mp_eater; break;
			case constant::job::id::cleric:
			case constant::job::id::priest:
			case constant::job::id::bishop: skill_id = constant::skill::cleric::mp_eater; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_venomous_weapon() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::night_lord: skill_id = constant::skill::night_lord::venomous_star; break;
			case constant::job::id::shadower: skill_id = constant::skill::shadower::venomous_stab; break;
			case constant::job::id::night_walker3: 
			case constant::job::id::night_walker4: skill_id = constant::skill::night_walker::venom; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_dark_sight_interruption_skill() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::night_walker2:
			case constant::job::id::night_walker3:
			case constant::job::id::night_walker4: skill_id = constant::skill::night_walker::vanish; break;
			case constant::job::id::wind_archer2:
			case constant::job::id::wind_archer3: 
			case constant::job::id::wind_archer4: skill_id = constant::skill::wind_archer::wind_walk; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_no_damage_skill() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::night_lord: skill_id = constant::skill::night_lord::shadow_shifter; break;
			case constant::job::id::shadower: skill_id = constant::skill::shadower::shadow_shifter; break;
			case constant::job::id::hero: skill_id = constant::skill::hero::guardian; break;
			case constant::job::id::paladin: skill_id = constant::skill::paladin::guardian; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_follow_the_lead() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::job::get_job_type(player->get_stats()->get_job())) {
			case job_type::adventurer: skill_id = constant::skill::beginner::follow_the_lead; break;
			case job_type::cygnus: skill_id = constant::skill::noblesse::follow_the_lead; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_legendary_spirit() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::job::get_job_type(player->get_stats()->get_job())) {
			case job_type::adventurer: skill_id = constant::skill::beginner::legendary_spirit; break;
			case job_type::cygnus: skill_id = constant::skill::noblesse::legendary_spirit; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_maker() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::job::get_job_type(player->get_stats()->get_job())) {
			case job_type::adventurer: skill_id = constant::skill::beginner::maker; break;
			case job_type::cygnus: skill_id = constant::skill::noblesse::maker; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_blessing_of_the_fairy() const -> game_skill_id {
	game_skill_id skill_id = 0;
	if (auto player = m_player.lock()) {
		switch (vana::util::game_logic::job::get_job_type(player->get_stats()->get_job())) {
			case job_type::adventurer: skill_id = constant::skill::beginner::blessing_of_the_fairy; break;
			case job_type::cygnus: skill_id = constant::skill::noblesse::blessing_of_the_fairy; break;
		}
		return skill_id;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_rechargeable_bonus() const -> game_slot_qty {
	game_slot_qty bonus = 0;
	if (auto player = m_player.lock()) {
		switch (player->get_stats()->get_job()) {
			case constant::job::id::assassin:
			case constant::job::id::hermit:
			case constant::job::id::night_lord: bonus = get_skill_level(constant::skill::assassin::claw_mastery) * 10; break;
			case constant::job::id::gunslinger:
			case constant::job::id::outlaw:
			case constant::job::id::corsair: bonus = get_skill_level(constant::skill::gunslinger::gun_mastery) * 10; break;
			case constant::job::id::night_walker2:
			case constant::job::id::night_walker3:
			case constant::job::id::night_walker4: bonus = get_skill_level(constant::skill::night_walker::claw_mastery) * 10; break;
		}
		return bonus;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::add_cooldown(game_skill_id skill_id, seconds time) -> void {
	m_cooldowns[skill_id] = time;
}

auto player_skills::remove_cooldown(game_skill_id skill_id) -> void {
	auto kvp = m_cooldowns.find(skill_id);
	if (kvp != std::end(m_cooldowns)) {
		m_cooldowns.erase(kvp);
	}
}

auto player_skills::remove_all_cooldowns() -> void {
	auto dupe = m_cooldowns;
	if (auto player = m_player.lock()) {
		for (const auto &kvp : dupe) {
			if (kvp.first != constant::skill::buccaneer::time_leap) {
				skills::stop_cooldown(player, kvp.first);
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::open_mystic_door(const point &pos, seconds door_time) -> mystic_door_result {
	if (auto current_player = m_player.lock()) {
		party *party = current_player->get_party();
		bool is_displacement = m_mystic_door != nullptr;

		uint8_t zero_based_party_index = 0;
		if (party != nullptr) {
			zero_based_party_index = party->get_zero_based_index_by_member(current_player);
		}

		mystic_door_open_result result = party == nullptr ?
			current_player->get_map()->get_town_mystic_door_portal(current_player) :
			current_player->get_map()->get_town_mystic_door_portal(current_player, zero_based_party_index);

		if (result.result != mystic_door_result::success) {
			return result.result;
		}

		if (is_displacement) {
			if (party != nullptr) {
				party->run_function([&](ref_ptr<player> party_member) {
					if (party_member->get_map_id() == m_mystic_door->get_map_id()) {
						party_member->send(packets::map::remove_door(m_mystic_door, false));
					}
				});
			}
			else {
				current_player->send(packets::map::remove_door(m_mystic_door, false));
			}
		}

		auto town_id = result.town_id;
		auto portal = result.portal;
		m_mystic_door = make_ref_ptr<mystic_door>(current_player, town_id, portal->id, pos, portal->pos, is_displacement, door_time);

		if (party != nullptr) {
			party->run_function([&](ref_ptr<player> party_member) {
				bool send_spawn_packet = false;
				bool in_town = false;
				if (party_member->get_map_id() == m_mystic_door->get_map_id()) {
					send_spawn_packet = true;
				}
				else if (party_member->get_map_id() == m_mystic_door->get_town_id()) {
					send_spawn_packet = true;
					in_town = true;
				}
				if (send_spawn_packet) {
					party_member->send(packets::map::spawn_door(m_mystic_door, false, false));
					party_member->send(packets::party::update_door(zero_based_party_index, m_mystic_door));
				}
			});
		}
		else {
			current_player->send(packets::map::spawn_door(m_mystic_door, false, false));
			current_player->send(packets::map::spawn_portal(m_mystic_door, current_player->get_map_id()));
		}

		vana::timer::timer::create(
			[this](const time_point &now) {
				this->close_mystic_door(true);
			},
			vana::timer::id{vana::timer::type::door_timer},
			current_player->get_timer_container(),
			m_mystic_door->get_door_time());

		return mystic_door_result::success;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::close_mystic_door(bool from_timer) -> void {
	if (auto current_player = m_player.lock()) {
		if (!from_timer) {
			current_player->get_timer_container()->remove_timer(vana::timer::id{vana::timer::type::door_timer});
		}

		ref_ptr<mystic_door> door = m_mystic_door;
		m_mystic_door.reset();

		if (party *party = current_player->get_party()) {
			uint8_t zero_based_party_index = party->get_zero_based_index_by_member(current_player);

			party->run_function([&](ref_ptr<player> party_member) {
				game_map_id member_map = party_member->get_map_id();
				if (member_map == door->get_map_id()) {
					party_member->send(packets::map::remove_door(door, from_timer));
				}
				else if (member_map == door->get_town_id()) {
					party_member->send(packets::party::update_door(zero_based_party_index, m_mystic_door));
				}
			});
		}
		else {
			game_map_id player_map = current_player->get_map_id();
			if (from_timer && (player_map == door->get_map_id() || player_map == door->get_town_id())) {
				current_player->send(packets::map::remove_door(door, true));
				current_player->send(packets::map::remove_portal());
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::get_mystic_door() const -> ref_ptr<mystic_door> {
	return m_mystic_door;
}

auto player_skills::on_join_party(party *party, ref_ptr<player> player_value) -> void {
	if (auto player = m_player.lock()) {
		if (player.get() == player_value.get()) {
			if (m_mystic_door == nullptr) {
				return;
			}

			uint8_t zero_based_party_index = party->get_zero_based_index_by_member(player_value);
			mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(player_value, zero_based_party_index);
			if (result.result != mystic_door_result::success) {
				// ???
				return;
			}

			auto portal = result.portal;
			m_mystic_door = m_mystic_door->with_new_portal(portal->id, portal->pos);

			// The actual door itself doesn't have to be modified on the map if the player_value happens to be there
			// If the player_value is in town, the join party packet takes care of it

			return;
		}

		if (ref_ptr<mystic_door> door = player_value->get_skills()->get_mystic_door()) {
			if (player->get_map_id() == door->get_map_id()) {
				player->send(packets::map::spawn_door(door, false, true));
			}
		}

		if (m_mystic_door != nullptr) {
			uint8_t zero_based_party_index = party->get_zero_based_index_by_member(player);
			mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(player, zero_based_party_index);
			if (result.result != mystic_door_result::success) {
				// ???
				return;
			}

			auto portal = result.portal;
			m_mystic_door = m_mystic_door->with_new_portal(portal->id, portal->pos);

			if (player_value->get_map_id() == m_mystic_door->get_map_id()) {
				player_value->send(packets::map::spawn_door(m_mystic_door, false, true));
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::on_leave_party(party *party, ref_ptr<player> player_value, bool kicked) -> void {
	if (auto player = m_player.lock()) {
		if (player_value.get() == player.get()) {
			if (m_mystic_door == nullptr) {
				return;
			}

			mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(player_value);
			if (result.result != mystic_door_result::success) {
				// ???
				return;
			}

			auto portal = result.portal;
			m_mystic_door = m_mystic_door->with_new_portal(portal->id, portal->pos);

			// The actual door itself doesn't have to be modified on the map if the player happens to be there

			if (player_value->get_map_id() == m_mystic_door->get_town_id()) {
				player_value->send(packets::party::update_door(0, nullptr));
				player_value->send(packets::map::spawn_door(m_mystic_door, true, true));
				player_value->send(packets::map::spawn_portal(m_mystic_door, player_value->get_map_id()));
			}

			return;
		}

		if (ref_ptr<mystic_door> door = player_value->get_skills()->get_mystic_door()) {
			if (player->get_map_id() == door->get_map_id()) {
				player->send(packets::map::remove_door(door, false));
			}
		}

		if (m_mystic_door != nullptr) {
			uint8_t zero_based_party_index = party->get_zero_based_index_by_member(player);
			mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(player, zero_based_party_index);
			if (result.result != mystic_door_result::success) {
				// ???
				return;
			}

			auto portal = result.portal;
			m_mystic_door = m_mystic_door->with_new_portal(portal->id, portal->pos);

			if (player_value->get_map_id() == m_mystic_door->get_map_id()) {
				player_value->send(packets::map::remove_door(m_mystic_door, false));
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::on_party_disband(party *party) -> void {
	if (auto current_player = m_player.lock()) {
		if (m_mystic_door == nullptr) {
			return;
		}

		uint8_t zero_based_party_index = party->get_zero_based_index_by_member(current_player);
		party->run_function([&](ref_ptr<player> party_member) {
			game_map_id member_map = party_member->get_map_id();
			if (member_map == m_mystic_door->get_town_id()) {
				party_member->send(packets::party::update_door(zero_based_party_index, nullptr));
			}
			else if (party_member.get() != current_player.get() && member_map == m_mystic_door->get_map_id()) {
				party_member->send(packets::map::remove_door(m_mystic_door, false));
			}
		});

		mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(current_player);
		if (result.result != mystic_door_result::success) {
			// ???
			return;
		}

		auto portal = result.portal;
		auto new_door = m_mystic_door->with_new_portal(portal->id, portal->pos);

		if (current_player->get_map_id() == new_door->get_town_id()) {
			current_player->send(packets::map::spawn_door(new_door, true, true));
			current_player->send(packets::map::spawn_portal(new_door, current_player->get_map_id()));
		}

		m_mystic_door = new_door;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::on_map_change() const -> void {
	if (auto current_player = m_player.lock()) {
		if (party *party = current_player->get_party()) {
			party->run_function([&](ref_ptr<player> party_member) {
				if (ref_ptr<mystic_door> door = party_member->get_skills()->get_mystic_door()) {
					if (current_player->get_map_id() == door->get_map_id()) {
						current_player->send(packets::map::spawn_door(door, false, true));
					}
				}
			});

			// Unconditional return here since the player is in the party
			return;
		}

		if (m_mystic_door == nullptr) {
			return;
		}

		game_map_id map_id = current_player->get_map_id();
		bool in_town = map_id == m_mystic_door->get_town_id();
		if (map_id == m_mystic_door->get_map_id() || in_town) {
			current_player->send(packets::map::spawn_door(m_mystic_door, in_town, true));
			current_player->send(packets::map::spawn_portal(m_mystic_door, map_id));
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_skills::on_disconnect() -> void {
	if (m_mystic_door != nullptr) {
		close_mystic_door(false);
	}
}

auto player_skills::connect_packet(packet_builder &builder) const -> void {
	// Skill levels
	builder.add<uint16_t>(static_cast<uint16_t>(m_skills.size()));
	for (const auto &kvp : m_skills) {
		builder.add<game_skill_id>(kvp.first);
		builder.add<int32_t>(kvp.second.level);
		if (vana::util::game_logic::player_skill::is_fourth_job_skill(kvp.first)) {
			builder.add<int32_t>(kvp.second.player_max_skill_level);
		}
	}
	// Cooldowns
	builder.add<uint16_t>(static_cast<uint16_t>(m_cooldowns.size()));
	for (const auto &kvp : m_cooldowns) {
		builder.add<game_skill_id>(kvp.first);
		builder.add<int16_t>(static_cast<int16_t>(kvp.second.count()));
	}
}

auto player_skills::connect_packet_for_blessing(packet_builder &builder) const -> void {
	// Orange text wasn't added until sometime after .75 and before .82
	//if (!m_blessing_player.empty()) {
	//	packet.add<bool>(true);
	//	packet.add<string>(m_blessing_player);
	//}
	//else {
	//	packet.add<bool>(false);
	//}
}

}
}