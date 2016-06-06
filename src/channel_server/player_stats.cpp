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
#include "player_stats.hpp"
#include "common/algorithm.hpp"
#include "common/equip_data_provider.hpp"
#include "common/game_constants.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/inter_header.hpp"
#include "common/misc_utilities.hpp"
#include "common/packet_reader.hpp"
#include "common/packet_wrapper.hpp"
#include "common/randomizer.hpp"
#include "common/skill_data_provider.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance_temp.hpp"
#include "channel_server/inventory_temp.hpp"
#include "channel_server/inventory_packet.hpp"
#include "channel_server/levels_packet.hpp"
#include "channel_server/maps_temp.hpp"
#include "channel_server/party_temp.hpp"
#include "channel_server/player_temp.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/players_packet.hpp"
#include "channel_server/summon_handler.hpp"
#include <iostream>
#include <limits>
#include <string>

namespace vana {
namespace channel_server {

player_stats::player_stats(player *player, game_player_level level, game_job_id job, game_fame fame, game_stat str, game_stat dex, game_stat intl, game_stat luk, game_stat ap, game_health_ap hp_mp_ap, game_stat sp, game_health hp, game_health max_hp, game_health mp, game_health max_mp, game_experience exp) :
	m_player{player},
	m_level{level},
	m_job{job},
	m_fame{fame},
	m_str{str},
	m_dex{dex},
	m_int{intl},
	m_luk{luk},
	m_ap{ap},
	m_hp_mp_ap{hp_mp_ap},
	m_sp{sp},
	m_hp{hp},
	m_max_hp{max_hp},
	m_mp{mp},
	m_max_mp{max_mp},
	m_exp{exp}
{
	if (is_dead()) {
		m_hp = stats::default_hp;
	}
}

auto player_stats::is_dead() const -> bool {
	return m_hp == stats::min_hp;
}

// Equip stat bonus handling
auto player_stats::update_bonuses(bool update_equips, bool is_loading) -> void {
	if (m_maple_warrior > 0) {
		set_maple_warrior(m_maple_warrior);
	}
	if (update_equips) {
		m_equip_bonuses = bonus_set();
		for (const auto &kvp : m_equip_stats) {
			const equip_bonus &info = kvp.second;
			if (channel_server::get_instance().get_equip_data_provider().can_equip(info.id, m_player->get_gender(), get_job(), get_str(true), get_dex(true), get_int(true), get_luk(true), get_fame())) {
				m_equip_bonuses.hp += info.hp;
				m_equip_bonuses.mp += info.mp;
				m_equip_bonuses.str += info.str;
				m_equip_bonuses.dex += info.dex;
				m_equip_bonuses.intl += info.intl;
				m_equip_bonuses.luk += info.luk;
			}
		}
	}

	if (m_hyper_body_x > 0) {
		set_hyper_body_hp(m_hyper_body_x);
	}
	if (m_hyper_body_y > 0) {
		set_hyper_body_mp(m_hyper_body_y);
	}

	if (!is_loading) {
		// Adjust current HP/MP down if necessary
		if (get_hp() > get_max_hp()) {
			set_hp(get_hp());
		}
		if (get_mp() > get_max_mp()) {
			set_mp(get_mp());
		}
	}
}

auto player_stats::set_equip(game_inventory_slot slot, item *equip, bool is_loading) -> void {
	slot = std::abs(slot);
	if (equip != nullptr) {
		m_equip_stats[slot].id = equip->get_id();
		m_equip_stats[slot].hp = equip->get_hp();
		m_equip_stats[slot].mp = equip->get_mp();
		m_equip_stats[slot].str = equip->get_str();
		m_equip_stats[slot].dex = equip->get_dex();
		m_equip_stats[slot].intl = equip->get_int();
		m_equip_stats[slot].luk = equip->get_luk();
	}
	else {
		m_equip_stats.erase(slot);
	}

	update_bonuses(true, is_loading);
}

// Data acquisition
auto player_stats::connect_packet(packet_builder &builder) -> void {
	builder.add<game_player_level>(get_level());
	builder.add<game_job_id>(get_job());
	builder.add<game_stat>(get_str());
	builder.add<game_stat>(get_dex());
	builder.add<game_stat>(get_int());
	builder.add<game_stat>(get_luk());
	builder.add<game_health>(get_hp());
	builder.add<game_health>(get_max_hp(true));
	builder.add<game_health>(get_mp());
	builder.add<game_health>(get_max_mp(true));
	builder.add<game_stat>(get_ap());
	builder.add<game_stat>(get_sp());
	builder.add<game_experience>(get_exp());
	builder.add<game_fame>(get_fame());
}

auto player_stats::get_max_hp(bool without_bonus) -> game_health {
	if (!without_bonus) {
		return static_cast<game_health>(std::min<int32_t>(m_max_hp + m_equip_bonuses.hp + m_buff_bonuses.hp, stats::max_max_hp));
	}
	return m_max_hp;
}

auto player_stats::get_max_mp(bool without_bonus) -> game_health {
	if (!without_bonus) {
		return static_cast<game_health>(std::min<int32_t>(m_max_mp + m_equip_bonuses.mp + m_buff_bonuses.mp, stats::max_max_mp));
	}
	return m_max_mp;
}

auto player_stats::stat_utility(int32_t test) -> int16_t {
	return static_cast<int16_t>(std::min<int32_t>(std::numeric_limits<int16_t>::max(), test));
}

auto player_stats::get_str(bool with_bonus) -> game_stat {
	if (with_bonus) {
		return stat_utility(m_str + m_buff_bonuses.str + m_equip_bonuses.str);
	}
	return m_str;
}

auto player_stats::get_dex(bool with_bonus) -> game_stat {
	if (with_bonus) {
		return stat_utility(m_dex + m_buff_bonuses.dex + m_equip_bonuses.dex);
	}
	return m_dex;
}

auto player_stats::get_int(bool with_bonus) -> game_stat {
	if (with_bonus) {
		return stat_utility(m_int + m_buff_bonuses.intl + m_equip_bonuses.intl);
	}
	return m_int;
}

auto player_stats::get_luk(bool with_bonus) -> game_stat {
	if (with_bonus) {
		return stat_utility(m_luk + m_buff_bonuses.luk + m_equip_bonuses.luk);
	}
	return m_luk;
}

// Data modification
auto player_stats::check_hp_mp() -> void {
	if (m_hp > get_max_hp()) {
		m_hp = get_max_hp();
	}
	if (m_mp > get_max_mp()) {
		m_mp = get_max_mp();
	}
}

auto player_stats::set_level(game_player_level level) -> void {
	m_level = level;
	m_player->send(packets::player::update_stat(stats::level, level));
	m_player->send_map(packets::level_up(m_player->get_id()));
	channel_server::get_instance().get_player_data_provider().update_player_level(ref_ptr<player>{m_player});
}

auto player_stats::set_hp(game_health hp, bool send_packet) -> void {
	m_hp = ext::constrain_range<game_health>(hp, stats::min_hp, get_max_hp());
	if (send_packet) {
		m_player->send(packets::player::update_stat(stats::hp, m_hp));
	}
	modified_hp();
}

auto player_stats::modify_hp(int32_t hp_mod, bool send_packet) -> void {
	int32_t temp_hp = m_hp + hp_mod;
	temp_hp = ext::constrain_range<int32_t>(temp_hp, stats::min_hp, get_max_hp());
	m_hp = static_cast<game_health>(temp_hp);

	if (send_packet) {
		m_player->send(packets::player::update_stat(stats::hp, m_hp));
	}
	modified_hp();
}

auto player_stats::damage_hp(int32_t damage_hp) -> void {
	m_hp = std::max<int32_t>(stats::min_hp, static_cast<int32_t>(m_hp) - damage_hp);
	m_player->send(packets::player::update_stat(stats::hp, m_hp));
	modified_hp();
}

auto player_stats::modified_hp() -> void {
	if (party *p = m_player->get_party()) {
		p->show_hp_bar(ref_ptr<player>{m_player});
	}
	m_player->get_active_buffs()->check_berserk();
	if (m_hp == stats::min_hp) {
		if (instance *inst = m_player->get_instance()) {
			inst->player_death(m_player->get_id());
		}
		lose_exp();
		m_player->get_summons()->for_each([this](summon *summon) {
			summon_handler::remove_summon(ref_ptr<player>{m_player}, summon->get_id(), false, summon_messages::disappearing);
		});
	}
}

auto player_stats::set_mp(game_health mp, bool send_packet) -> void {
	if (!m_player->get_active_buffs()->has_infinity()) {
		m_mp = ext::constrain_range<game_health>(mp, stats::min_mp, get_max_mp());
	}
	m_player->send(packets::player::update_stat(stats::mp, m_mp, send_packet));
}

auto player_stats::modify_mp(int32_t mp_mod, bool send_packet) -> void {
	if (!m_player->get_active_buffs()->has_infinity()) {
		int32_t temp_mp = m_mp + mp_mod;
		temp_mp = ext::constrain_range<int32_t>(temp_mp, stats::min_mp, get_max_mp());
		m_mp = static_cast<game_health>(temp_mp);
	}
	m_player->send(packets::player::update_stat(stats::mp, m_mp, send_packet));
}

auto player_stats::damage_mp(int32_t damage_mp) -> void {
	if (!m_player->get_active_buffs()->has_infinity()) {
		m_mp = std::max<int32_t>(stats::min_mp, static_cast<int32_t>(m_mp) - damage_mp);
	}
	m_player->send(packets::player::update_stat(stats::mp, m_mp, false));
}

auto player_stats::set_sp(game_stat sp) -> void {
	m_sp = sp;
	m_player->send(packets::player::update_stat(stats::sp, sp));
}

auto player_stats::set_ap(game_stat ap) -> void {
	m_ap = ap;
	m_player->send(packets::player::update_stat(stats::ap, ap));
}

auto player_stats::set_job(game_job_id job) -> void {
	m_job = job;
	m_player->send(packets::player::update_stat(stats::job, job));
	m_player->send_map(packets::job_change(m_player->get_id()));
	channel_server::get_instance().get_player_data_provider().update_player_job(ref_ptr<player>{m_player});
}

auto player_stats::set_str(game_stat str) -> void {
	m_str = str;
	m_player->send(packets::player::update_stat(stats::str, str));
}

auto player_stats::set_dex(game_stat dex) -> void {
	m_dex = dex;
	m_player->send(packets::player::update_stat(stats::dex, dex));
}

auto player_stats::set_int(game_stat intl) -> void {
	m_int = intl;
	m_player->send(packets::player::update_stat(stats::intl, intl));
}

auto player_stats::set_luk(game_stat luk) -> void {
	m_luk = luk;
	m_player->send(packets::player::update_stat(stats::luk, luk));
}

auto player_stats::set_maple_warrior(int16_t mod) -> void {
	m_buff_bonuses.str = (m_str * mod) / 100;
	m_buff_bonuses.dex = (m_dex * mod) / 100;
	m_buff_bonuses.intl = (m_int * mod) / 100;
	m_buff_bonuses.luk = (m_luk * mod) / 100;
	if (m_maple_warrior != mod) {
		m_maple_warrior = mod;
		update_bonuses();
	}
}

auto player_stats::set_max_hp(game_health max_hp) -> void {
	m_max_hp = ext::constrain_range(max_hp, stats::min_max_hp, stats::max_max_hp);
	m_player->send(packets::player::update_stat(stats::max_hp, m_max_hp));
	modified_hp();
}

auto player_stats::set_max_mp(game_health max_mp) -> void {
	m_max_mp = ext::constrain_range(max_mp, stats::min_max_mp, stats::max_max_mp);
	m_player->send(packets::player::update_stat(stats::max_mp, m_max_mp));
}

auto player_stats::set_hyper_body_hp(int16_t mod) -> void {
	m_hyper_body_x = mod;
	m_buff_bonuses.hp = std::min<uint16_t>((m_max_hp + m_equip_bonuses.hp) * mod / 100, stats::max_max_hp);
	m_player->send(packets::player::update_stat(stats::max_hp, m_max_hp));
	if (mod == 0) {
		set_hp(get_hp());
	}
	if (party *p = m_player->get_party()) {
		p->show_hp_bar(ref_ptr<player>{m_player});
	}
	m_player->get_active_buffs()->check_berserk();
}

auto player_stats::set_hyper_body_mp(int16_t mod) -> void {
	m_hyper_body_y = mod;
	m_buff_bonuses.mp = std::min<uint16_t>((m_max_mp + m_equip_bonuses.mp) * mod / 100, stats::max_max_mp);
	m_player->send(packets::player::update_stat(stats::max_mp, m_max_mp));
	if (mod == 0) {
		set_mp(get_mp());
	}
}

auto player_stats::modify_max_hp(game_health mod) -> void {
	m_max_hp = std::min<game_health>(m_max_hp + mod, stats::max_max_hp);
	m_player->send(packets::player::update_stat(stats::max_hp, m_max_hp));
}

auto player_stats::modify_max_mp(game_health mod) -> void {
	m_max_mp = std::min<game_health>(m_max_mp + mod, stats::max_max_mp);
	m_player->send(packets::player::update_stat(stats::max_mp, m_max_mp));
}

auto player_stats::set_exp(game_experience exp) -> void {
	m_exp = std::max(exp, 0);
	m_player->send(packets::player::update_stat(stats::exp, m_exp));
}

auto player_stats::set_fame(game_fame fame) -> void {
	m_fame = ext::constrain_range(fame, stats::min_fame, stats::max_fame);
	m_player->send(packets::player::update_stat(stats::fame, fame));
}

auto player_stats::lose_exp() -> void {
	if (!game_logic_utilities::is_beginner_job(get_job()) && get_level() < game_logic_utilities::get_max_level(get_job()) && m_player->get_map_id() != vana::maps::sorcerers_room) {
		game_slot_qty charms = m_player->get_inventory()->get_item_amount(items::safety_charm);
		if (charms > 0) {
			inventory::take_item(ref_ptr<player>{m_player}, items::safety_charm, 1);
			// TODO FIXME REVIEW
			charms = --charms;
			m_player->send(packets::inventory::use_charm(static_cast<uint8_t>(charms)));
			return;
		}
		map *loc = m_player->get_map();
		int8_t exp_loss = 10;
		if (loc->lose_one_percent()) {
			exp_loss = 1;
		}
		else {
			switch (game_logic_utilities::get_job_line(get_job())) {
				case jobs::job_lines::magician:
					exp_loss = 7;
					break;
				case jobs::job_lines::thief:
					exp_loss = 5;
					break;
			}
		}
		game_experience exp = get_exp();
		exp -= static_cast<game_experience>(static_cast<int64_t>(get_exp(get_level())) * exp_loss / 100);
		set_exp(exp);
	}
}

// Level related functions
auto player_stats::give_exp(uint64_t exp, bool in_chat, bool white) -> void {
	game_job_id full_job = get_job();
	game_player_level level = get_level();
	game_player_level job_max = game_logic_utilities::get_max_level(full_job);
	if (level >= job_max) {
		// Do not give EXP to characters of max level or over
		return;
	}

	uint64_t cur_exp = get_exp() + exp;
	if (exp > 0) {
		uint64_t exp_counter = exp;
		uint64_t batch_size = std::numeric_limits<game_experience>::max();
		while (exp_counter > 0) {
			game_experience allocate = static_cast<game_experience>(std::min(exp_counter, batch_size));
			m_player->send(packets::show_exp(allocate, white, in_chat));
			exp_counter -= allocate;
		}
	}

	if (cur_exp >= get_exp(level)) {
		bool cygnus = game_logic_utilities::is_cygnus(full_job);
		game_player_level levels_gained = 0;
		game_player_level levels_max = channel_server::get_instance().get_config().max_multi_level;
		game_stat ap_gain = 0;
		game_stat sp_gain = 0;
		game_health hp_gain = 0;
		game_health mp_gain = 0;
		int8_t job_line = game_logic_utilities::get_job_line(full_job);
		game_stat intl = get_int(true) / 10;
		game_health x = 0; // X value for Improving *P Increase skills, cached, only needs to be set once

		while (cur_exp >= get_exp(level) && levels_gained < levels_max) {
			cur_exp -= get_exp(get_level());
			level++;
			levels_gained++;
			if (cygnus && level <= stats::cygnus_ap_cutoff) {
				ap_gain += stats::ap_per_cygnus_level;
			}
			else {
				ap_gain += stats::ap_per_level;
			}
			switch (job_line) {
				case jobs::job_lines::beginner:
					hp_gain += level_hp(stats::base_hp::beginner);
					mp_gain += level_mp(stats::base_mp::beginner, intl);
					break;
				case jobs::job_lines::warrior:
					if (levels_gained == 1 && m_player->get_skills()->has_hp_increase()) {
						x = get_x(m_player->get_skills()->get_hp_increase());
					}
					hp_gain += level_hp(stats::base_hp::warrior, x);
					mp_gain += level_mp(stats::base_mp::warrior, intl);
					break;
				case jobs::job_lines::magician:
					if (levels_gained == 1 && m_player->get_skills()->has_mp_increase()) {
						x = get_x(m_player->get_skills()->get_mp_increase());
					}
					hp_gain += level_hp(stats::base_hp::magician);
					mp_gain += level_mp(stats::base_mp::magician, 2 * x + intl);
					break;
				case jobs::job_lines::bowman:
					hp_gain += level_hp(stats::base_hp::bowman);
					mp_gain += level_mp(stats::base_mp::bowman, intl);
					break;
				case jobs::job_lines::thief:
					hp_gain += level_hp(stats::base_hp::thief);
					mp_gain += level_mp(stats::base_mp::thief, intl);
					break;
				case jobs::job_lines::pirate:
					if (levels_gained == 1 && m_player->get_skills()->has_hp_increase()) {
						x = get_x(m_player->get_skills()->get_hp_increase());
					}
					hp_gain += level_hp(stats::base_hp::pirate, x);
					mp_gain += level_mp(stats::base_mp::pirate, intl);
					break;
				default: // GM
					hp_gain += stats::base_hp::gm;
					mp_gain += stats::base_mp::gm;
			}
			if (!game_logic_utilities::is_beginner_job(full_job)) {
				sp_gain += stats::sp_per_level;
			}
			if (level >= job_max) {
				// Do not let people level past the level cap
				cur_exp = 0;
				break;
			}
		}

		if (cur_exp >= get_exp(level)) {
			// If the desired number of level ups have passed and they're still above, we chop
			cur_exp = get_exp(level) - 1;
		}

		// Check if the m_player has leveled up at all, it is possible that the user hasn't leveled up if multi-level limit is 0
		if (levels_gained) {
			modify_max_hp(hp_gain);
			modify_max_mp(mp_gain);
			set_level(level);
			set_ap(get_ap() + ap_gain);
			set_sp(get_sp() + sp_gain);

			// Let Hyper Body remain on during a level up, as it should
			auto active_buffs = m_player->get_active_buffs();
			auto hyper_body_hp = active_buffs->get_hyper_body_hp_source();
			auto hyper_body_mp = active_buffs->get_hyper_body_mp_source();
			if (hyper_body_hp.is_initialized()) {
				set_hyper_body_hp(active_buffs->get_buff_skill_info(hyper_body_hp.get())->x);
			}
			if (hyper_body_mp.is_initialized()) {
				set_hyper_body_mp(active_buffs->get_buff_skill_info(hyper_body_hp.get())->y);
			}

			set_hp(get_max_hp());
			set_mp(get_max_mp());
			m_player->set_level_date();
			if (get_level() == job_max && !m_player->is_gm()) {
				out_stream message;
				message << "[Congrats] " << m_player->get_name() << " has reached Level "
						<< static_cast<int16_t>(job_max) << "! Congratulate "
						<< m_player->get_name() << " on such an amazing achievement!";

				channel_server::get_instance().send_world(
					vana::packets::prepend(packets::player::show_message(message.str(), packets::player::notice_types::blue), [](packet_builder &builder) {
						builder.add<packet_header>(IMSG_TO_ALL_CHANNELS);
						builder.add<packet_header>(IMSG_TO_ALL_PLAYERS);
					}));
			}
		}
	}

	// By this point, the EXP should be a valid EXP in the range of 0 to game_experience max
	set_exp(static_cast<game_experience>(cur_exp));
}

auto player_stats::add_stat(packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	int32_t type = reader.get<int32_t>();
	if (get_ap() == 0) {
		// Hacking
		return;
	}
	m_player->send(packets::stat_ok());
	add_stat(type);
}

auto player_stats::add_stat_multi(packet_reader &reader) -> void {
	reader.skip<game_tick_count>();
	uint32_t amount = reader.get<uint32_t>();

	m_player->send(packets::stat_ok());

	for (uint32_t i = 0; i < amount; i++) {
		int32_t type = reader.get<int32_t>();
		int32_t value = reader.get<int32_t>();

		if (value < 0 || get_ap() < value) {
			// Hacking
			return;
		}

		add_stat(type, static_cast<int16_t>(value)); // Prefer a single cast to countless casts/modification down the line
	}
}

auto player_stats::add_stat(int32_t type, int16_t mod, bool is_reset) -> void {
	game_stat max_stat = channel_server::get_instance().get_config().max_stats;
	bool is_subtract = mod < 0;
	switch (type) {
		case stats::str:
			if (get_str() >= max_stat) {
				return;
			}
			set_str(get_str() + mod);
			break;
		case stats::dex:
			if (get_dex() >= max_stat) {
				return;
			}
			set_dex(get_dex() + mod);
			break;
		case stats::intl:
			if (get_int() >= max_stat) {
				return;
			}
			set_int(get_int() + mod);
			break;
		case stats::luk:
			if (get_luk() >= max_stat) {
				return;
			}
			set_luk(get_luk() + mod);
			break;
		case stats::max_hp:
		case stats::max_mp: {
			if (type == stats::max_hp && get_max_hp(true) >= stats::max_max_hp) {
				return;
			}
			if (type == stats::max_mp && get_max_mp(true) >= stats::max_max_mp) {
				return;
			}
			if (is_subtract && get_hp_mp_ap() == 0) {
				// Hacking
				return;
			}
			int8_t job = game_logic_utilities::get_job_track(get_job());
			game_health hp_gain = 0;
			game_health mp_gain = 0;
			game_health y = 0;
			switch (job) {
				case jobs::job_tracks::beginner:
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::beginner_ap);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::beginner_ap);
					break;
				case jobs::job_tracks::warrior:
					if (m_player->get_skills()->has_hp_increase()) {
						y = get_y(m_player->get_skills()->get_hp_increase());
					}
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::warrior_ap, y);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::warrior_ap);
					break;
				case jobs::job_tracks::magician:
					if (m_player->get_skills()->has_mp_increase()) {
						y = get_y(m_player->get_skills()->get_mp_increase());
					}
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::magician_ap);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::magician_ap, 2 * y);
					break;
				case jobs::job_tracks::bowman:
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::bowman_ap);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::bowman_ap);
					break;
				case jobs::job_tracks::thief:
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::thief_ap);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::thief_ap);
					break;
				case jobs::job_tracks::pirate:
					if (m_player->get_skills()->has_hp_increase()) {
						y = get_y(m_player->get_skills()->get_hp_increase());
					}
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::pirate_ap, y);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::pirate_ap);
					break;
				default: // GM
					hp_gain = ap_reset_hp(is_reset, is_subtract, stats::base_hp::gm_ap);
					mp_gain = ap_reset_mp(is_reset, is_subtract, stats::base_mp::gm_ap);
					break;
			}
			set_hp_mp_ap(get_hp_mp_ap() + mod);
			switch (type) {
				case stats::max_hp: modify_max_hp(hp_gain); break;
				case stats::max_mp: modify_max_mp(mp_gain); break;
			}

			auto active_buffs = m_player->get_active_buffs();
			auto hyper_body_hp = active_buffs->get_hyper_body_hp_source();
			auto hyper_body_mp = active_buffs->get_hyper_body_mp_source();
			if (hyper_body_hp.is_initialized()) {
				set_hyper_body_hp(active_buffs->get_buff_skill_info(hyper_body_hp.get())->x);
			}
			if (hyper_body_mp.is_initialized()) {
				set_hyper_body_mp(active_buffs->get_buff_skill_info(hyper_body_hp.get())->y);
			}

			set_hp(get_hp());
			set_mp(get_mp());
			break;
		}
		default:
			// Hacking, one assumes
			break;
	}
	if (!is_reset) {
		set_ap(get_ap() - mod);
	}
	update_bonuses();
}

auto player_stats::rand_hp() -> game_health {
	return randomizer::rand<game_health>(stats::base_hp::variation); // Max HP range per class (e.g. Beginner is 8-12)
}

auto player_stats::rand_mp() -> game_health {
	return randomizer::rand<game_health>(stats::base_mp::variation); // Max MP range per class (e.g. Beginner is 6-8)
}

auto player_stats::get_x(game_skill_id skill_id) -> int16_t {
	return m_player->get_skills()->get_skill_info(skill_id)->x;
}

auto player_stats::get_y(game_skill_id skill_id) -> int16_t {
	return m_player->get_skills()->get_skill_info(skill_id)->y;
}

auto player_stats::ap_reset_hp(bool is_reset, bool is_subtract, int16_t val, int16_t s_val) -> int16_t {
	return (is_reset ? (is_subtract ? -(s_val + val + stats::base_hp::variation) : val) : level_hp(val, s_val));
}

auto player_stats::ap_reset_mp(bool is_reset, bool is_subtract, int16_t val, int16_t s_val) -> int16_t {
	return (is_reset ? (is_subtract ? -(s_val + val + stats::base_mp::variation) : val) : level_mp(val, s_val));
}

auto player_stats::level_hp(game_health val, game_health bonus) -> game_health {
	return rand_hp() + val + bonus;
}

auto player_stats::level_mp(game_health val, game_health bonus) -> game_health {
	return rand_mp() + val + bonus;
}

auto player_stats::get_exp(game_player_level level) -> game_experience {
	return stats::player_exp[level - 1];
}

}
}