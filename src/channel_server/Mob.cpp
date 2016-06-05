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
#include "Mob.hpp"
#include "common_temp/Algorithm.hpp"
#include "common_temp/GameConstants.hpp"
#include "common_temp/MiscUtilities.hpp"
#include "common_temp/MpEaterData.hpp"
#include "common_temp/Randomizer.hpp"
#include "common_temp/TimeUtilities.hpp"
#include "common_temp/Timer.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/DropHandler.hpp"
#include "channel_server/Instance.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Mist.hpp"
#include "channel_server/MobsPacket.hpp"
#include "channel_server/Party.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/PlayerDataProvider.hpp"
#include "channel_server/PlayerPacket.hpp"
#include "channel_server/Skills.hpp"
#include "channel_server/SkillsPacket.hpp"
#include "channel_server/StatusInfo.hpp"
#include <functional>
#include <initializer_list>

namespace vana {
namespace channel_server {

mob::mob(game_map_object map_mob_id, game_map_id map_id, game_mob_id mob_id, view_ptr<mob> owner, const point &pos, int32_t spawn_id, bool faces_left, game_foothold_id foothold, mob_control_status control_status) :
	movable_life{foothold, pos, faces_left ? 1 : 2},
	m_map_mob_id{map_mob_id},
	m_map_id{map_id},
	m_spawn_id{spawn_id},
	m_mob_id{mob_id},
	m_owner{owner},
	m_info{channel_server::get_instance().get_mob_data_provider().get_mob_info(mob_id)},
	m_control_status{control_status}
{
	m_hp = get_max_hp();
	m_mp = get_max_mp();
	if (!can_fly()) {
		m_origin_foothold = foothold;
	}

	m_total_health = m_hp;

	m_status = status_effects::mob::empty;
	status_info empty{status_effects::mob::empty, 0, 0, seconds{0}};
	m_statuses[empty.status] = empty;

	if (m_info->hp_recovery > 0 || m_info->mp_recovery > 0) {
		int32_t hp_recovery = m_info->hp_recovery;
		int32_t mp_recovery = m_info->mp_recovery;
		vana::timer::timer::create(
			[this, hp_recovery, mp_recovery](const time_point &now) { this->natural_heal(hp_recovery, mp_recovery); },
			vana::timer::id{timer_type::mob_heal_timer},
			get_timers(), seconds{1}, seconds{10});
	}
	if (m_info->remove_after > 0) {
		vana::timer::timer::create(
			[this](const time_point &now) { this->kill(); },
			vana::timer::id{timer_type::mob_remove_timer, m_map_mob_id},
			get_timers(), seconds{m_info->remove_after});
	}
}

auto mob::natural_heal(int32_t hp_heal, int32_t mp_heal) -> void {
	if (hp_heal > 0 && get_hp() < get_max_hp()) {
		int32_t hp = get_hp() + hp_heal;
		int32_t sponge_hp = hp_heal;
		if (hp < 0 || hp > get_max_hp()) {
			sponge_hp = get_max_hp() - get_hp();
			hp = get_max_hp();
		}
		m_hp = hp;
		m_total_health += sponge_hp;
		if (auto sponge = m_sponge.lock()) {
			sponge->m_hp += sponge_hp;
		}
	}
	if (mp_heal > 0 && m_mp < get_max_mp()) {
		int32_t mp = get_mp() + mp_heal;
		if (mp < 0 || mp > get_max_mp()) {
			mp = get_max_mp();
		}
		m_mp = mp;
	}
}

auto mob::apply_damage(game_player_id player_id, game_damage damage, bool poison) -> void {
	damage = std::max(damage, 0);
	if (damage > m_hp) {
		damage = m_hp - poison; // Keep HP from hitting 0 for poison and from going below 0
	}

	m_damages[player_id] += damage;
	m_hp -= damage;

	if (!poison) {
		// HP bar packet does nothing for showing damage when poison is damaging for whatever reason
		auto player = channel_server::get_instance().get_player_data_provider().get_player(player_id);
		map *map = get_map();

		uint8_t percent = static_cast<uint8_t>(m_hp * 100 / m_info->hp);

		if (m_info->has_hp_bar()) {
			// Boss HP bars - Horntail's damage sponge isn't a boss in the data
			map->send(packets::mobs::show_boss_hp(shared_from_this()));
		}
		else if (m_info->boss) {
			// Minibosses
			map->send(packets::mobs::show_hp(m_map_mob_id, percent));
		}
		else if (m_info->friendly) {
			map->send(packets::mobs::damage_friendly_mob(shared_from_this(), damage));
		}
		else if (player != nullptr) {
			player->send(packets::mobs::show_hp(m_map_mob_id, percent));
		}

		// Need to preserve the pointer through mob deletion in die()
		auto sponge = m_sponge.lock();
		if (m_hp == stats::min_hp) {
			die(player);
		}
		if (sponge != nullptr) {
			sponge->apply_damage(player_id, damage, false);
			// Apply damage after you can be sure that all the units are linked and ready
		}
	}
	else if (m_hp == 1) {
		remove_status(status_effects::mob::poison);
	}
}

auto mob::apply_web_damage() -> void {
	game_damage web_damage = get_max_hp() / (50 - m_web_level);
	if (web_damage >= m_hp) {
		// Keep HP from hitting 0
		web_damage = m_hp - 1;
	}
	if (web_damage != 0) {
		m_damages[m_web_player_id] += web_damage;
		m_hp -= web_damage;
		get_map()->send(packets::mobs::hurt_mob(m_map_mob_id, web_damage));
	}
}

auto mob::add_status(game_player_id player_id, vector<status_info> &status_info) -> void {
	int32_t added_status = 0;
	vector<int32_t> reflection;
	map *map = get_map();

	for (auto &info : status_info) {
		int32_t c_status = info.status;
		bool already_has_status = m_statuses.find(c_status) != std::end(m_statuses);
		switch (c_status) {
			case status_effects::mob::poison: // Status effects that do not renew
			case status_effects::mob::doom:
				if (already_has_status) {
					continue;
				}
				break;
			case status_effects::mob::shadow_web:
				m_web_player_id = player_id;
				m_web_level = static_cast<game_skill_level>(info.val);
				map->add_webbed_mob(get_map_mob_id());
				break;
			case status_effects::mob::magic_attack_up:
				switch (info.skill_id) {
					case vana::skills::night_lord::taunt:
					case vana::skills::shadower::taunt: {
						m_taunt_effect = (100 - info.val) + 100;
						// Value passed as 100 - x, so 100 - value will = x
						break;
					}
				}
				break;
			case status_effects::mob::venomous_weapon:
				m_venom_count++;
				if (already_has_status) {
					info.val += m_statuses[c_status].val; // Increase the damage
				}
				break;
			case status_effects::mob::weapon_damage_reflect:
			case status_effects::mob::magic_damage_reflect:
				reflection.push_back(info.reflection);
				break;
		}

		m_statuses[c_status] = info;
		added_status += c_status;

		switch (c_status) {
			case status_effects::mob::poison:
			case status_effects::mob::venomous_weapon:
			case status_effects::mob::ninja_ambush:
				game_damage poison_damage = info.val;
				vana::timer::timer::create(
					[this, player_id, poison_damage](const time_point &now) {
						this->apply_damage(player_id, poison_damage, true);
					},
					vana::timer::id{timer_type::mob_status_timer, c_status, 1},
					get_timers(), seconds{1}, seconds{1});
				break;
		}

		// We add some milliseconds to our times in order to allow poisons to not end one hit early
		vana::timer::timer::create(
			[this, c_status](const time_point &now) { this->remove_status(c_status, true); },
			vana::timer::id{timer_type::mob_status_timer, c_status},
			get_timers(), milliseconds{info.time.count() * 1000 + 100});
	}

	// Calculate new status mask
	m_status = 0;
	for (const auto &kvp : m_statuses) {
		m_status |= kvp.first;
	}
	map->send(packets::mobs::apply_status(m_map_mob_id, added_status, status_info, 300, reflection));
}

auto mob::remove_status(int32_t status, bool from_timer) -> void {
	auto kvp = m_statuses.find(status);
	if (kvp != std::end(m_statuses) && get_hp() > 0) {
		const status_info &stat = kvp->second;
		map *map = get_map();
		switch (status) {
			case status_effects::mob::shadow_web:
				m_web_level = 0;
				m_web_player_id = 0;
				map->remove_webbed_mob(get_map_mob_id());
				break;
			case status_effects::mob::magic_attack_up:
				switch (stat.skill_id) {
					case vana::skills::night_lord::taunt:
					case vana::skills::shadower::taunt:
						m_taunt_effect = 100;
						break;
				}
				break;
			case status_effects::mob::venomous_weapon:
				m_venom_count = 0;
				// Intentional fallthrough
			case status_effects::mob::poison:
				// Stop poison damage timer
				get_timers()->remove_timer(vana::timer::id{timer_type::mob_status_timer, status, 1});
				break;
		}
		if (!from_timer) {
			get_timers()->remove_timer(vana::timer::id{timer_type::mob_status_timer, status});
		}
		m_status -= status;
		m_statuses.erase(kvp);
		map->send(packets::mobs::remove_status(m_map_mob_id, status));
	}
}

auto mob::has_weapon_reflection() const -> bool {
	int32_t mask = status_effects::mob::weapon_damage_reflect;
	return (m_status & mask) != 0;
}

auto mob::has_magic_reflection() const -> bool {
	int32_t mask = status_effects::mob::magic_damage_reflect;
	return (m_status & mask) != 0;
}

auto mob::has_immunity() const -> bool {
	int32_t mask = status_effects::mob::weapon_immunity | status_effects::mob::magic_immunity | status_effects::mob::weapon_damage_reflect | status_effects::mob::magic_damage_reflect;
	return (m_status & mask) != 0;
}

auto mob::has_status(int32_t status) const -> bool {
	return (m_status & status) != 0;
}

auto mob::get_status_value(int32_t status) -> optional<status_info> {
	auto kvp = m_statuses.find(status);
	return kvp != std::end(m_statuses) ? kvp->second : optional<status_info>{};
}

auto mob::get_status_bits() const -> int32_t {
	return m_status;
}

auto mob::get_status_info() const -> const ord_map<int32_t, status_info> & {
	return m_statuses;
}

auto mob::get_magic_reflection() -> optional<status_info> {
	return get_status_value(status_effects::mob::magic_damage_reflect);
}

auto mob::get_weapon_reflection() -> optional<status_info> {
	return get_status_value(status_effects::mob::weapon_damage_reflect);
}

auto mob::set_controller(ref_ptr<player> control, mob_spawn_type spawn, ref_ptr<player> display) -> void {
	end_control();

	m_controller = control;
	if (control != nullptr) {
		control->send(packets::mobs::request_control(shared_from_this(), spawn));
	}
	else if (get_control_status() == mob_control_status::none) {
		if (display != nullptr) {
			display->send(packets::mobs::request_control(shared_from_this(), spawn));
		}
		else {
			get_map()->send(packets::mobs::request_control(shared_from_this(), spawn));
		}
	}

	m_anticipated_skill = 0;
	m_anticipated_skill_level = 0;
	m_anticipated_skill_player_id = 0;
	m_skill_feasible = false;
}

auto mob::end_control() -> void {
	// TODO FIXME resource
	// is_disconnecting should not be necessary here, but it requires a great deal of structural fixing to properly fix
	if (m_controller != nullptr && m_controller->get_map_id() == get_map_id() && !m_controller->is_disconnecting()) {
		m_controller->send(packets::mobs::end_control_mob(m_map_mob_id));
	}
}

auto mob::die(ref_ptr<player> player, bool from_explosion) -> void {
	map *map = get_map();

	end_control();

	while (m_markers.size() > 0) {
		auto marker = m_markers[0];
		auto source = marker->get_active_buffs()->get_homing_beacon_source();
		if (source.is_initialized()) {
			auto &buff_source = source.get();
			marker->get_active_buffs()->remove_buff(
				buff_source,
				buffs::preprocess_buff(marker, buff_source, seconds{0}));
		}

		m_markers.erase(m_markers.begin());
	}

	game_player_id highest_damager = distribute_exp_and_get_drop_recipient(player);

	// Ending of death stuff
	drop_handler::do_drops(highest_damager, m_map_id, get_level(), m_mob_id, get_pos(), has_explosive_drop(), has_ffa_drop(), get_taunt_effect());

	if (player != nullptr) {
		party *party = player->get_party();
		if (party != nullptr) {
			auto members = party->get_party_members(m_map_id);
			for (const auto &member : members) {
				member->get_quests()->update_quest_mob(m_mob_id);
			}
		}
		else {
			player->get_quests()->update_quest_mob(m_mob_id);
		}
	}

	map->mob_death(shared_from_this(), from_explosion);
}

auto mob::explode() -> void {
	die(nullptr, true);
}

auto mob::kill() -> void {
	apply_damage(0, get_hp());
}

auto mob::consume_mp(int32_t mp) -> void {
	m_mp = std::max(m_mp - mp, 0);
}

auto mob::distribute_exp_and_get_drop_recipient(ref_ptr<player> killer) -> game_player_id {
	game_player_id highest_damager = 0;
	uint64_t highest_damage = 0;

	if (m_damages.size() > 0) {
		struct party_exp {
			party_exp() :
				total_exp{0},
				party{nullptr},
				highest_damager{nullptr},
				highest_damage{0},
				min_hit_level{stats::player_levels}
			{
			}

			game_player_level min_hit_level;
			uint64_t total_exp;
			uint64_t highest_damage;
			ref_ptr<player> highest_damager;
			party *party;
		};

		hash_map<game_party_id, party_exp> parties;

		int32_t mob_exp_rate = channel_server::get_instance().get_config().rates.mob_exp_rate;

		for (const auto &kvp : m_damages) {
			game_player_id damager_id = kvp.first;
			uint64_t damage = kvp.second;
			if (damage > highest_damage) {
				// Find the highest damager to give drop ownership
				highest_damager = damager_id;
				highest_damage = damage;
			}

			auto damager = channel_server::get_instance().get_player_data_provider().get_player(damager_id);
			if (damager == nullptr || damager->get_map_id() != m_map_id || damager->get_stats()->is_dead()) {
				// Only give EXP if the damager is in the same channel, on the same map and is alive
				continue;
			}
			game_player_level damager_level = damager->get_stats()->get_level();
			party *damager_party = damager->get_party();

			uint64_t exp = static_cast<uint64_t>(m_info->exp) * ((8 * damage / m_total_health) + (damager == killer ? 2 : 0)) / 10;
			if (damager_party != nullptr) {
				game_party_id party_id = damager_party->get_id();
				auto kvp = parties.find(party_id);
				if (kvp == std::end(parties)) {
					party_exp new_party;
					new_party.total_exp = 0;
					new_party.party = damager_party;
					kvp = parties.emplace(party_id, new_party).first;
				}

				party_exp &damaging_party = kvp->second;
				damaging_party.total_exp += exp;

				if (damager_level < damaging_party.min_hit_level) {
					damaging_party.min_hit_level = damager_level;
				}
				if (damage > damaging_party.highest_damage) {
					damaging_party.highest_damager = damager;
					damaging_party.highest_damage = damage;
				}
			}
			else {
				// Account for EXP increasing junk
				int16_t hs_rate = damager->get_active_buffs()->get_holy_symbol_rate();
				exp = exp * get_taunt_effect() / 100;
				exp *= mob_exp_rate;
				exp += ((exp * hs_rate) / 100);
				damager->get_stats()->give_exp(exp, false, (damager == killer));
			}
		}

		if (parties.size() > 0) {
			for (const auto &kvp : parties) {
				const party_exp &info = kvp.second;
				party *damager_party = info.party;
				vector<ref_ptr<player>> party_members = damager_party->get_party_members(get_map_id());
				uint16_t total_level = 0;
				uint16_t leech_count = 0;
				for (const auto &party_member : party_members) {
					game_player_level damager_level = party_member->get_stats()->get_level();
					if (damager_level < (info.min_hit_level - 5) && damager_level < (get_level() - 5)) {
						continue;
					}
					total_level += damager_level;
					leech_count++;
				}
				for (const auto &party_member : party_members) {
					game_player_level damager_level = party_member->get_stats()->get_level();
					if (damager_level < (info.min_hit_level - 5) && damager_level < (get_level() - 5)) {
						continue;
					}
					uint64_t exp = static_cast<uint64_t>(m_info->exp) * ((8 * damager_level / total_level) + (party_member == info.highest_damager ? 2 : 0)) / 10;
					int16_t hs_rate = party_member->get_active_buffs()->get_holy_symbol_rate();
					exp = exp * get_taunt_effect() / 100;
					exp *= mob_exp_rate;
					exp += ((exp * hs_rate) / 100);
					party_member->get_stats()->give_exp(exp, false, (party_member == killer));
				}
			}
		}
	}
	return highest_damager;
}

auto mob::skill_heal(int32_t heal_hp, int32_t heal_range) -> void {
	if (is_sponge()) {
		return;
	}
	int32_t amount = randomizer::range<int32_t>(heal_hp, heal_range);
	int32_t original = amount;

	if (m_hp + amount > get_max_hp()) {
		amount = get_max_hp() - m_hp;
		m_hp = get_max_hp();
	}
	else {
		m_hp += amount;
	}
	m_total_health += amount;

	if (auto sponge = m_sponge.lock()) {
		heal_hp = sponge->get_hp() + amount;
		heal_hp = ext::constrain_range<int32_t>(heal_hp, stats::min_hp, sponge->get_max_hp());
		sponge->m_hp = heal_hp;
	}

	get_map()->send(packets::mobs::heal_mob(m_map_mob_id, original));
}

auto mob::dispel_buffs() -> void {
	auto statuses = {
		status_effects::mob::watk, status_effects::mob::wdef,
		status_effects::mob::matk, status_effects::mob::mdef,
		status_effects::mob::acc, status_effects::mob::avoid,
		status_effects::mob::speed,
	};

	for (const auto &status : statuses) {
		remove_status(status);
	}
}

auto mob::do_crash_skill(game_skill_id skill_id) -> void {
	switch (skill_id) {
		case vana::skills::crusader::armor_crash: remove_status(status_effects::mob::wdef); break;
		case vana::skills::white_knight::magic_crash: remove_status(status_effects::mob::matk); break;
		case vana::skills::dragon_knight::power_crash: remove_status(status_effects::mob::watk); break;
	}
}

auto mob::mp_eat(ref_ptr<player> player, mp_eater_data *mp) -> void {
	if (m_mp_eater_count < 3 && get_mp() > 0 && randomizer::percentage<uint16_t>() < mp->prop) {
		mp->used = true;
		int32_t eaten_mp = get_max_mp() * mp->x / 100;

		eaten_mp = std::min<int32_t>(eaten_mp, get_mp());
		m_mp = get_mp() - eaten_mp;

		eaten_mp = std::min<int32_t>(eaten_mp, stats::max_max_mp);
		player->get_stats()->modify_mp(eaten_mp);

		player->send_map(packets::skills::show_skill_effect(player->get_id(), mp->skill_id));
		m_mp_eater_count++;
	}
}

auto mob::add_marker(ref_ptr<player> player) -> void {
	m_markers.push_back(player);
}

auto mob::remove_marker(ref_ptr<player> player) -> void {
	for (size_t i = 0; i < m_markers.size(); i++) {
		auto test = m_markers[i];
		if (test == player) {
			m_markers.erase(m_markers.begin() + i);
			return;
		}
	}
}

auto mob::choose_random_skill(ref_ptr<player> player, game_mob_skill_id &skill_id, game_mob_skill_level &skill_level) -> void {
	if (m_info->skill_count == 0 || m_anticipated_skill != 0 || !can_cast_skills()) {
		return;
	}

	time_point now = utilities::time::get_now();
	if (utilities::time::get_distance_in_seconds(now, m_last_skill_use) < seconds{3}) {
		return;
	}

	vector<const mob_skill_info *> viable_skills;
	auto &skills = channel_server::get_instance().get_mob_data_provider().get_skills(get_mob_id_or_link());
	for (const auto &info : skills) {
		bool stop = false;
		auto mob_skill = channel_server::get_instance().get_skill_data_provider().get_mob_skill(info.skill_id, info.level);

		switch (info.skill_id) {
			case mob_skills::weapon_attack_up:
			case mob_skills::weapon_attack_up_aoe:
				stop = has_status(status_effects::mob::watk);
				break;
			case mob_skills::magic_attack_up:
			case mob_skills::magic_attack_up_aoe:
				stop = has_status(status_effects::mob::matk);
				break;
			case mob_skills::weapon_defense_up:
			case mob_skills::weapon_defense_up_aoe:
				stop = has_status(status_effects::mob::wdef);
				break;
			case mob_skills::magic_defense_up:
			case mob_skills::magic_defense_up_aoe:
				stop = has_status(status_effects::mob::mdef);
				break;
			case mob_skills::weapon_immunity:
			case mob_skills::magic_immunity:
			case mob_skills::weapon_damage_reflect:
			case mob_skills::magic_damage_reflect:
				stop = has_immunity();
				break;
			case mob_skills::mc_speed_up:
				stop = has_status(status_effects::mob::speed);
				break;
			case mob_skills::summon:
				stop = static_cast<int16_t>(m_spawns.size()) > mob_skill->limit;
				break;
		}

		if (stop) {
			continue;
		}

		auto kvp = m_skill_use.find(info.skill_id);
		if (kvp != std::end(m_skill_use)) {
			time_point target_time = kvp->second + seconds{mob_skill->cooldown};
			stop = now < target_time;
		}

		if (!stop) {
			double current_mob_hp_percentage = static_cast<double>(get_hp()) * 100. / static_cast<double>(get_max_hp());
			stop = current_mob_hp_percentage > static_cast<double>(mob_skill->hp);
		}

		if (!stop) {
			viable_skills.push_back(&info);
		}
	}

	if (viable_skills.size() == 0) {
		return;
	}

	auto skill = *randomizer::select(viable_skills);
	skill_id = skill->skill_id;
	skill_level = skill->level;
	m_anticipated_skill = skill_id;
	m_anticipated_skill_level = skill_level;
	m_anticipated_skill_player_id = player->get_id();
}

auto mob::reset_anticipated_skill() -> void {
	m_anticipated_skill = 0;
	m_anticipated_skill_level = 0;
	m_anticipated_skill_player_id = 0;
}

auto mob::use_anticipated_skill() -> result {
	game_mob_skill_id skill_id = m_anticipated_skill;
	game_mob_skill_level level = m_anticipated_skill_level;

	reset_anticipated_skill();

	if (!can_cast_skills()) {
		return result::failure;
	}

	time_point now = utilities::time::get_now();
	m_skill_use[skill_id] = now;
	m_last_skill_use = now;

	auto &channel = channel_server::get_instance();
	auto skill_level_info = channel.get_skill_data_provider().get_mob_skill(skill_id, level);

	auto &skills = channel.get_mob_data_provider().get_skills(m_mob_id);
	milliseconds delay = milliseconds{0};
	for (const auto &skill : skills) {
		if (skill.skill_id == skill_id && skill.level == level) {
			delay = skill.effect_after;
			break;
		}
	}

	consume_mp(skill_level_info->mp);

	rect skill_area = skill_level_info->dimensions.move(get_pos());
	map *map = get_map();
	vector<status_info> statuses;
	bool aoe = false;

	switch (skill_id) {
		case mob_skills::weapon_attack_up_aoe:
			aoe = true;
		case mob_skills::weapon_attack_up:
			statuses.emplace_back(status_effects::mob::watk, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::magic_attack_up_aoe:
			aoe = true;
		case mob_skills::magic_attack_up:
			statuses.emplace_back(status_effects::mob::matk, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::weapon_defense_up_aoe:
			aoe = true;
		case mob_skills::weapon_defense_up:
			statuses.emplace_back(status_effects::mob::wdef, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::magic_defense_up_aoe:
			aoe = true;
		case mob_skills::magic_defense_up:
			statuses.emplace_back(status_effects::mob::mdef, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::heal_aoe:
			map->heal_mobs(skill_level_info->x, skill_level_info->y, skill_area);
			break;
		case mob_skills::seal:
		case mob_skills::darkness:
		case mob_skills::weakness:
		case mob_skills::stun:
		case mob_skills::curse:
		case mob_skills::poison:
		case mob_skills::slow:
		case mob_skills::seduce:
		case mob_skills::crazy_skull:
		case mob_skills::zombify: {
			auto func = [skill_id, level, delay](ref_ptr<player> player) {
				buffs::add_buff(player, skill_id, level, delay);
			};
			map->run_function_players(skill_area, skill_level_info->prop, skill_level_info->count, func);
			break;
		}
		case mob_skills::dispel: {
			map->run_function_players(skill_area, skill_level_info->prop, [](ref_ptr<player> player) {
				player->get_active_buffs()->dispel_buffs();
			});
			break;
		}
		case mob_skills::send_to_town: {
			if (auto banish_info = channel.get_skill_data_provider().get_banish_data(get_mob_id())) {
				game_map_id field = banish_info->field;
				string message = banish_info->message;
				const portal_info * const portal = maps::get_map(field)->query_portal_name(banish_info->portal);

				auto func = [&message, &field, &portal](ref_ptr<player> player) {
					if (!message.empty()) {
						player->send(packets::player::show_message(message, packets::player::notice_types::blue));
					}
					player->set_map(field, portal);
				};
				map->run_function_players(skill_area, skill_level_info->prop, skill_level_info->count, func);
			}
			else {
				game_player_id player_id = m_anticipated_skill_player_id;
				game_mob_id mob_id = get_mob_id_or_link();
				channel.log(log_type::hacking, [&](out_stream &str) {
					str << "Likely hacking by player ID " << player_id << ". "
						<< "SendToTown used on an invalid mob: " << mob_id;
				});
				return result::failure;
			}
			break;
		}
		case mob_skills::poison_mist:
			new mist{get_map_id(), this, seconds{skill_level_info->time}, skill_area, skill_id, level};
			break;
		case mob_skills::weapon_immunity:
			statuses.emplace_back(status_effects::mob::weapon_immunity, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::magic_immunity:
			statuses.emplace_back(status_effects::mob::magic_immunity, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::weapon_damage_reflect:
			statuses.emplace_back(status_effects::mob::weapon_immunity, skill_level_info->x, skill_id, level, skill_level_info->time);
			statuses.emplace_back(status_effects::mob::weapon_damage_reflect, skill_level_info->x, skill_id, level, skill_level_info->y, skill_level_info->time);
			break;
		case mob_skills::magic_damage_reflect:
			statuses.emplace_back(status_effects::mob::magic_immunity, skill_level_info->x, skill_id, level, skill_level_info->time);
			statuses.emplace_back(status_effects::mob::magic_damage_reflect, skill_level_info->x, skill_id, level, skill_level_info->y, skill_level_info->time);
			break;
		case mob_skills::any_damage_reflect:
			statuses.emplace_back(status_effects::mob::weapon_immunity, skill_level_info->x, skill_id, level, skill_level_info->time);
			statuses.emplace_back(status_effects::mob::magic_immunity, skill_level_info->x, skill_id, level, skill_level_info->time);
			statuses.emplace_back(status_effects::mob::weapon_damage_reflect, skill_level_info->x, skill_id, level, skill_level_info->y, skill_level_info->time);
			statuses.emplace_back(status_effects::mob::magic_damage_reflect, skill_level_info->x, skill_id, level, skill_level_info->y, skill_level_info->time);
			break;
		case mob_skills::mc_speed_up:
			statuses.emplace_back(status_effects::mob::speed, skill_level_info->x, skill_id, level, skill_level_info->time);
			break;
		case mob_skills::summon:
			map->mob_summon_skill_used(shared_from_this(), skill_level_info);
			break;
	}

	if (statuses.size() > 0) {
		if (aoe) {
			map->status_mobs(statuses, skill_area);
		}
		else {
			add_status(0, statuses);
		}
	}

	return result::successful;
}

auto mob::can_cast_skills() const -> bool {
	return !(has_status(status_effects::mob::freeze) || has_status(status_effects::mob::stun) || has_status(status_effects::mob::shadow_web) || has_status(status_effects::mob::seal));
}

auto mob::is_sponge(game_mob_id mob_id) -> bool {
	switch (mob_id) {
		case mobs::horntail_sponge: return true;
	}
	return false;
}

auto mob::spawns_sponge(game_mob_id mob_id) -> bool {
	switch (mob_id) {
		case mobs::summon_horntail: return true;
	}
	return false;
}

auto mob::get_map() const -> map * {
	return maps::get_map(m_map_id);
}

}
}