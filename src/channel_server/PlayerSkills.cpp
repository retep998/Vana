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
#include "PlayerSkills.hpp"
#include "common/algorithm_temp.hpp"
#include "common/database_temp.hpp"
#include "common/game_constants.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/randomizer_temp.hpp"
#include "common/skill_data_provider.hpp"
#include "common/timer_temp.hpp"
#include "common/timer_id.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Map.hpp"
#include "channel_server/MapPacket.hpp"
#include "channel_server/MysticDoor.hpp"
#include "channel_server/Party.hpp"
#include "channel_server/PartyPacket.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/Skills.hpp"
#include "channel_server/SkillsPacket.hpp"

namespace vana {
namespace channel_server {

player_skills::player_skills(player *player) :
	m_player{player}
{
	load();
}

auto player_skills::load() -> void {
	auto &db = database::get_char_db();
	auto &sql = db.get_session();
	player_skill_info skill;
	game_player_id player_id = m_player->get_id();
	game_skill_id skill_id = 0;

	soci::rowset<> rs = (sql.prepare
		<< "SELECT s.skill_id, s.points, s.max_level "
		<< "FROM " << db.make_table("skills") << " s "
		<< "WHERE s.character_id = :char",
		soci::use(player_id, "char"));

	for (const auto &row : rs) {
		skill_id = row.get<game_skill_id>("skill_id");
		if (game_logic_utilities::is_blessing_of_the_fairy(skill_id)) {
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
		<< "FROM " << db.make_table("cooldowns") << " c "
		<< "WHERE c.character_id = :char",
		soci::use(player_id, "char"));

	for (const auto &row : rs) {
		game_skill_id skill_id = row.get<game_skill_id>("skill_id");
		seconds time_left = seconds{row.get<int16_t>("remaining_time")};
		skills::start_cooldown(ref_ptr<player>{m_player}, skill_id, time_left, true);
		m_cooldowns[skill_id] = time_left;
	}

	skill_id = get_blessing_of_the_fairy();

	opt_string blessing_player_name;
	optional<game_player_level> blessing_player_level;

	game_account_id account_id = m_player->get_account_id();
	game_world_id world_id = m_player->get_world_id();

	// TODO FIXME skill
	// Allow Cygnus <-> Adventurer selection here or allow it to be ignored
	// That is, some versions only allowed Adv. Blessing to be populated by Cygnus levels and vice versa
	// Some later versions lifted this restriction entirely
	sql.once
		<< "SELECT c.name, c.level "
		<< "FROM " << db.make_table("characters") << " c "
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

auto player_skills::save(bool save_cooldowns) -> void {
	using namespace soci;
	game_player_id player_id = m_player->get_id();
	auto &db = database::get_char_db();
	auto &sql = db.get_session();

	game_skill_id skill_id = 0;
	game_skill_level level = 0;
	game_skill_level max_level = 0;
	statement st = (sql.prepare
		<< "REPLACE INTO " << db.make_table("skills") << " VALUES (:player, :skill, :level, :max_level)",
		use(player_id, "player"),
		use(skill_id, "skill"),
		use(level, "level"),
		use(max_level, "max_level"));

	for (const auto &kvp : m_skills) {
		if (game_logic_utilities::is_blessing_of_the_fairy(kvp.first)) {
			continue;
		}
		skill_id = kvp.first;
		level = kvp.second.level;
		max_level = kvp.second.player_max_skill_level;
		st.execute(true);
	}

	if (save_cooldowns) {
		sql.once << "DELETE FROM " << db.make_table("cooldowns") << " WHERE character_id = :char",
			soci::use(player_id, "char");

		if (m_cooldowns.size() > 0) {
			int16_t remaining_time = 0;
			st = (sql.prepare
				<< "INSERT INTO " << db.make_table("cooldowns") << " (character_id, skill_id, remaining_time) "
				<< "VALUES (:char, :skill, :time)",
				use(player_id, "char"),
				use(skill_id, "skill"),
				use(remaining_time, "time"));

			for (const auto &kvp : m_cooldowns) {
				skill_id = kvp.first;
				remaining_time = skills::get_cooldown_time_left(ref_ptr<player>{m_player}, kvp.first);
				st.execute(true);
			}
		}
	}
}

auto player_skills::add_skill_level(game_skill_id skill_id, game_skill_level amount, bool send_packet) -> bool {
	if (!channel_server::get_instance().get_skill_data_provider().is_valid_skill(skill_id)) {
		return false;
	}

	// Keep people from adding too much SP and prevent it from going negative

	auto kvp = m_skills.find(skill_id);
	game_skill_level new_level = (kvp != std::end(m_skills) ? kvp->second.level : 0) + amount;
	game_skill_level max_skill_level = channel_server::get_instance().get_skill_data_provider().get_max_level(skill_id);
	if (new_level > max_skill_level || (game_logic_utilities::is_fourth_job_skill(skill_id) && new_level > get_max_skill_level(skill_id))) {
		return false;
	}

	m_skills[skill_id].level = new_level;
	m_skills[skill_id].max_skill_level = max_skill_level;
	if (send_packet) {
		m_player->send(packets::skills::add_skill(skill_id, m_skills[skill_id]));
	}
	return true;
}

auto player_skills::get_skill_level(game_skill_id skill_id) const -> game_skill_level {
	auto skill = ext::find_value_ptr(m_skills, skill_id);
	return skill == nullptr ? 0 : skill->level;
}

auto player_skills::set_max_skill_level(game_skill_id skill_id, game_skill_level max_level, bool send_packet) -> void {
	// Set max level for 4th job skills
	m_skills[skill_id].player_max_skill_level = max_level;

	if (send_packet) {
		m_player->get_skills()->add_skill_level(skill_id, 0);
	}
}

auto player_skills::get_max_skill_level(game_skill_id skill_id) const -> game_skill_level {
	// Get max level for 4th job skills
	if (m_skills.find(skill_id) != std::end(m_skills)) {
		const player_skill_info &info = m_skills.find(skill_id)->second;
		if (game_logic_utilities::is_fourth_job_skill(skill_id)) {
			return info.player_max_skill_level;
		}
		return info.max_skill_level;
	}
	return 0;
}

auto player_skills::get_skill_info(game_skill_id skill_id) const -> const skill_level_info * const {
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
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::fp_mage:
		case jobs::job_ids::fp_arch_mage: skill_id = vana::skills::fp_mage::element_amplification; break;
		case jobs::job_ids::il_mage:
		case jobs::job_ids::il_arch_mage: skill_id = vana::skills::il_mage::element_amplification; break;
		case jobs::job_ids::blaze_wizard3:
		case jobs::job_ids::blaze_wizard4: skill_id = vana::skills::blaze_wizard::element_amplification; break;
	}
	return skill_id;
}

auto player_skills::get_achilles() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::hero: skill_id = vana::skills::hero::achilles; break;
		case jobs::job_ids::paladin: skill_id = vana::skills::paladin::achilles; break;
		case jobs::job_ids::dark_knight: skill_id = vana::skills::dark_knight::achilles; break;
	}
	return skill_id;
}

auto player_skills::get_energy_charge() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::marauder:
		case jobs::job_ids::buccaneer: skill_id = vana::skills::marauder::energy_charge; break;
		case jobs::job_ids::thunder_breaker2:
		case jobs::job_ids::thunder_breaker3:
		case jobs::job_ids::thunder_breaker4: skill_id = vana::skills::thunder_breaker::energy_charge; break;
	}
	return skill_id;
}

auto player_skills::get_advanced_combo() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::hero: skill_id = vana::skills::hero::advanced_combo_attack; break;
		case jobs::job_ids::dawn_warrior3:
		case jobs::job_ids::dawn_warrior4: skill_id = vana::skills::dawn_warrior::advanced_combo; break;
	}
	return skill_id;
}

auto player_skills::get_alchemist() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::hermit:
		case jobs::job_ids::night_lord: skill_id = vana::skills::hermit::alchemist; break;
		case jobs::job_ids::night_walker3:
		case jobs::job_ids::night_walker4: skill_id = vana::skills::night_walker::alchemist; break;
	}
	return skill_id;
}

auto player_skills::get_hp_increase() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (game_logic_utilities::get_job_track(m_player->get_stats()->get_job())) {
		case jobs::job_tracks::warrior: skill_id = vana::skills::swordsman::improved_max_hp_increase; break;
		case jobs::job_tracks::dawn_warrior: skill_id = vana::skills::dawn_warrior::max_hp_enhancement; break;
		case jobs::job_tracks::thunder_breaker: skill_id = vana::skills::thunder_breaker::improve_max_hp; break;
		case jobs::job_tracks::pirate:
			if ((m_player->get_stats()->get_job() / 10) == (jobs::job_ids::brawler / 10)) {
				skill_id = vana::skills::brawler::improve_max_hp;
			}
			break;
	}
	return skill_id;
}

auto player_skills::get_mp_increase() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (game_logic_utilities::get_job_track(m_player->get_stats()->get_job())) {
		case jobs::job_tracks::magician: skill_id = vana::skills::magician::improved_max_mp_increase; break;
		case jobs::job_tracks::blaze_wizard: skill_id = vana::skills::blaze_wizard::increasing_max_mp; break;
	}
	return skill_id;
}

auto player_skills::get_mastery() const -> game_skill_id {
	game_skill_id mastery_id = 0;
	switch (game_logic_utilities::get_item_type(m_player->get_inventory()->get_equipped_id(equip_slots::weapon))) {
		case items::types::weapon_1h_sword:
		case items::types::weapon_2h_sword:
			switch (m_player->get_stats()->get_job()) {
				case jobs::job_ids::fighter:
				case jobs::job_ids::crusader:
				case jobs::job_ids::hero: mastery_id = vana::skills::fighter::sword_mastery; break;
				case jobs::job_ids::page:
				case jobs::job_ids::white_knight:
				case jobs::job_ids::paladin: mastery_id = vana::skills::page::sword_mastery; break;
			}
			break;
		case items::types::weapon_1h_axe:
		case items::types::weapon_2h_axe: mastery_id = vana::skills::fighter::axe_mastery; break;
		case items::types::weapon_1h_mace:
		case items::types::weapon_2h_mace: mastery_id = vana::skills::page::bw_mastery; break;
		case items::types::weapon_spear: mastery_id = vana::skills::spearman::spear_mastery; break;
		case items::types::weapon_polearm: mastery_id = vana::skills::spearman::polearm_mastery; break;
		case items::types::weapon_dagger: mastery_id = vana::skills::bandit::dagger_mastery; break;
		case items::types::weapon_knuckle: mastery_id = vana::skills::brawler::knuckler_mastery; break;
		case items::types::weapon_bow: mastery_id = vana::skills::hunter::bow_mastery; break;
		case items::types::weapon_crossbow: mastery_id = vana::skills::crossbowman::crossbow_mastery; break;
		case items::types::weapon_claw: mastery_id = vana::skills::assassin::claw_mastery; break;
		case items::types::weapon_gun: mastery_id = vana::skills::gunslinger::gun_mastery; break;
	}
	return mastery_id;
}

auto player_skills::get_mp_eater() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::fp_wizard:
		case jobs::job_ids::fp_mage:
		case jobs::job_ids::fp_arch_mage: skill_id = vana::skills::fp_wizard::mp_eater; break;
		case jobs::job_ids::il_wizard:
		case jobs::job_ids::il_mage:
		case jobs::job_ids::il_arch_mage: skill_id = vana::skills::il_wizard::mp_eater; break;
		case jobs::job_ids::cleric:
		case jobs::job_ids::priest:
		case jobs::job_ids::bishop: skill_id = vana::skills::cleric::mp_eater; break;
	}
	return skill_id;
}

auto player_skills::get_venomous_weapon() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::night_lord: skill_id = vana::skills::night_lord::venomous_star; break;
		case jobs::job_ids::shadower: skill_id = vana::skills::shadower::venomous_stab; break;
		case jobs::job_ids::night_walker3: 
		case jobs::job_ids::night_walker4: skill_id = vana::skills::night_walker::venom; break;
	}
	return skill_id;
}

auto player_skills::get_dark_sight_interruption_skill() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::night_walker2:
		case jobs::job_ids::night_walker3:
		case jobs::job_ids::night_walker4: skill_id = vana::skills::night_walker::vanish; break;
		case jobs::job_ids::wind_archer2:
		case jobs::job_ids::wind_archer3: 
		case jobs::job_ids::wind_archer4: skill_id = vana::skills::wind_archer::wind_walk; break;
	}
	return skill_id;
}

auto player_skills::get_no_damage_skill() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::night_lord: skill_id = vana::skills::night_lord::shadow_shifter; break;
		case jobs::job_ids::shadower: skill_id = vana::skills::shadower::shadow_shifter; break;
		case jobs::job_ids::hero: skill_id = vana::skills::hero::guardian; break;
		case jobs::job_ids::paladin: skill_id = vana::skills::paladin::guardian; break;
	}
	return skill_id;
}

auto player_skills::get_follow_the_lead() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (game_logic_utilities::get_job_type(m_player->get_stats()->get_job())) {
		case jobs::job_type::adventurer: skill_id = vana::skills::beginner::follow_the_lead; break;
		case jobs::job_type::cygnus: skill_id = vana::skills::noblesse::follow_the_lead; break;
	}
	return skill_id;
}

auto player_skills::get_legendary_spirit() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (game_logic_utilities::get_job_type(m_player->get_stats()->get_job())) {
		case jobs::job_type::adventurer: skill_id = vana::skills::beginner::legendary_spirit; break;
		case jobs::job_type::cygnus: skill_id = vana::skills::noblesse::legendary_spirit; break;
	}
	return skill_id;
}

auto player_skills::get_maker() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (game_logic_utilities::get_job_type(m_player->get_stats()->get_job())) {
		case jobs::job_type::adventurer: skill_id = vana::skills::beginner::maker; break;
		case jobs::job_type::cygnus: skill_id = vana::skills::noblesse::maker; break;
	}
	return skill_id;
}

auto player_skills::get_blessing_of_the_fairy() const -> game_skill_id {
	game_skill_id skill_id = 0;
	switch (game_logic_utilities::get_job_type(m_player->get_stats()->get_job())) {
		case jobs::job_type::adventurer: skill_id = vana::skills::beginner::blessing_of_the_fairy; break;
		case jobs::job_type::cygnus: skill_id = vana::skills::noblesse::blessing_of_the_fairy; break;
	}
	return skill_id;
}

auto player_skills::get_rechargeable_bonus() const -> game_slot_qty {
	game_slot_qty bonus = 0;
	switch (m_player->get_stats()->get_job()) {
		case jobs::job_ids::assassin:
		case jobs::job_ids::hermit:
		case jobs::job_ids::night_lord: bonus = get_skill_level(vana::skills::assassin::claw_mastery) * 10; break;
		case jobs::job_ids::gunslinger:
		case jobs::job_ids::outlaw:
		case jobs::job_ids::corsair: bonus = get_skill_level(vana::skills::gunslinger::gun_mastery) * 10; break;
		case jobs::job_ids::night_walker2:
		case jobs::job_ids::night_walker3:
		case jobs::job_ids::night_walker4: bonus = get_skill_level(vana::skills::night_walker::claw_mastery) * 10; break;
	}
	return bonus;
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
	for (const auto &kvp : dupe) {
		if (kvp.first != vana::skills::buccaneer::time_leap) {
			skills::stop_cooldown(ref_ptr<player>{m_player}, kvp.first);
		}
	}
}

auto player_skills::open_mystic_door(const point &pos, seconds door_time) -> mystic_door_result {
	party *party = m_player->get_party();
	bool is_displacement = m_mystic_door != nullptr;

	uint8_t zero_based_party_index = 0;
	if (party != nullptr) {
		zero_based_party_index = party->get_zero_based_index_by_member(ref_ptr<player>{m_player});
	}

	mystic_door_open_result result = party == nullptr ?
		m_player->get_map()->get_town_mystic_door_portal(ref_ptr<player>{m_player}) :
		m_player->get_map()->get_town_mystic_door_portal(ref_ptr<player>{m_player}, zero_based_party_index);

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
			m_player->send(packets::map::remove_door(m_mystic_door, false));
		}
	}

	auto town_id = result.town_id;
	auto portal = result.portal;
	m_mystic_door = make_ref_ptr<mystic_door>(ref_ptr<player>{m_player}, town_id, portal->id, pos, portal->pos, is_displacement, door_time);

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
		m_player->send(packets::map::spawn_door(m_mystic_door, false, false));
		m_player->send(packets::map::spawn_portal(m_mystic_door, m_player->get_map_id()));
	}

	vana::timer::timer::create(
		[this](const time_point &now) {
			this->close_mystic_door(true);
		},
		vana::timer::id{timer_type::door_timer},
		m_player->get_timer_container(),
		m_mystic_door->get_door_time());

	return mystic_door_result::success;
}

auto player_skills::close_mystic_door(bool from_timer) -> void {
	if (!from_timer) {
		m_player->get_timer_container()->remove_timer(vana::timer::id{timer_type::door_timer});
	}

	ref_ptr<mystic_door> door = m_mystic_door;
	m_mystic_door.reset();

	if (party *party = m_player->get_party()) {
		uint8_t zero_based_party_index = party->get_zero_based_index_by_member(ref_ptr<player>{m_player});

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
		game_map_id player_map = m_player->get_map_id();
		if (from_timer && (player_map == door->get_map_id() || player_map == door->get_town_id())) {
			m_player->send(packets::map::remove_door(door, true));
			m_player->send(packets::map::remove_portal());
		}
	}
}

auto player_skills::get_mystic_door() const -> ref_ptr<mystic_door> {
	return m_mystic_door;
}

auto player_skills::on_join_party(party *party, ref_ptr<player> player_value) -> void {
	if (player_value.get() == m_player) {
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
		if (m_player->get_map_id() == door->get_map_id()) {
			m_player->send(packets::map::spawn_door(door, false, true));
		}
	}

	if (m_mystic_door != nullptr) {
		uint8_t zero_based_party_index = party->get_zero_based_index_by_member(ref_ptr<player>{m_player});
		mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(ref_ptr<player>{m_player}, zero_based_party_index);
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

auto player_skills::on_leave_party(party *party, ref_ptr<player> player_value, bool kicked) -> void {
	if (player_value.get() == m_player) {
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
		if (m_player->get_map_id() == door->get_map_id()) {
			m_player->send(packets::map::remove_door(door, false));
		}
	}

	if (m_mystic_door != nullptr) {
		uint8_t zero_based_party_index = party->get_zero_based_index_by_member(ref_ptr<player>{m_player});
		mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(ref_ptr<player>{m_player}, zero_based_party_index);
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

auto player_skills::on_party_disband(party *party) -> void {
	if (m_mystic_door == nullptr) {
		return;
	}

	uint8_t zero_based_party_index = party->get_zero_based_index_by_member(ref_ptr<player>{m_player});
	party->run_function([&](ref_ptr<player> party_member) {
		game_map_id member_map = party_member->get_map_id();
		if (member_map == m_mystic_door->get_town_id()) {
			party_member->send(packets::party::update_door(zero_based_party_index, nullptr));
		}
		else if (party_member.get() != m_player && member_map == m_mystic_door->get_map_id()) {
			party_member->send(packets::map::remove_door(m_mystic_door, false));
		}
	});

	mystic_door_open_result result = m_mystic_door->get_map()->get_town_mystic_door_portal(ref_ptr<player>{m_player});
	if (result.result != mystic_door_result::success) {
		// ???
		return;
	}

	auto portal = result.portal;
	auto new_door = m_mystic_door->with_new_portal(portal->id, portal->pos);

	if (m_player->get_map_id() == new_door->get_town_id()) {
		m_player->send(packets::map::spawn_door(new_door, true, true));
		m_player->send(packets::map::spawn_portal(new_door, m_player->get_map_id()));
	}

	m_mystic_door = new_door;
}

auto player_skills::on_map_change() const -> void {
	if (party *party = m_player->get_party()) {
		party->run_function([&](ref_ptr<player> party_member) {
			if (ref_ptr<mystic_door> door = party_member->get_skills()->get_mystic_door()) {
				if (m_player->get_map_id() == door->get_map_id()) {
					m_player->send(packets::map::spawn_door(door, false, true));
				}
			}
		});

		// Unconditional return here since the player is in the party
		return;
	}

	if (m_mystic_door == nullptr) {
		return;
	}

	game_map_id map_id = m_player->get_map_id();
	bool in_town = map_id == m_mystic_door->get_town_id();
	if (map_id == m_mystic_door->get_map_id() || in_town) {
		m_player->send(packets::map::spawn_door(m_mystic_door, in_town, true));
		m_player->send(packets::map::spawn_portal(m_mystic_door, map_id));
	}
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
		if (game_logic_utilities::is_fourth_job_skill(kvp.first)) {
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