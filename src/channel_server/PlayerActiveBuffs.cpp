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
#include "PlayerActiveBuffs.hpp"
#include "common/game_constants.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/packet_reader.hpp"
#include "common/randomizer.hpp"
#include "common/skill_data_provider.hpp"
#include "common/time_utilities.hpp"
#include "common/timer.hpp"
#include "common/timer_container.hpp"
#include "channel_server/BuffsPacket.hpp"
#include "channel_server/ChannelServer.hpp"
#include "channel_server/Maps.hpp"
#include "channel_server/Player.hpp"
#include "channel_server/Skills.hpp"
#include "channel_server/SkillsPacket.hpp"
#include <functional>

namespace vana {
namespace channel_server {

struct buff_run_action {
	buff_run_action(buff_source source) :
		source{source}
	{
	}

	auto operator()(const time_point &now) -> void {
		switch (act) {
			case buff_action::heal: skills::heal(player, value, source); break;
			case buff_action::hurt: skills::hurt(player, value, source); break;
			default: throw not_implemented_exception{"action type"};
		}
	}

	int64_t value = 0;
	buff_source source;
	ref_ptr<player> player = nullptr;
	buff_action act;
};

auto player_active_buffs::local_buff_info::to_source() const -> buff_source {
	switch (type) {
		case buff_source_type::item:
			return buff_source::from_item(identifier);
		case buff_source_type::skill:
			return buff_source::from_skill(
				static_cast<game_skill_id>(identifier),
				static_cast<game_skill_level>(level));
		case buff_source_type::mob_skill:
			return buff_source::from_mob_skill(
				static_cast<game_mob_skill_id>(identifier),
				static_cast<game_mob_skill_level>(level));
	}
	throw not_implemented_exception{"buff_source_type"};
}

player_active_buffs::player_active_buffs(player *player) :
	m_player{player}
{
}

// Buff skills
auto player_active_buffs::translate_to_source(int32_t buff_id) const -> buff_source {
	if (buff_id < 0) {
		return buff_source::from_item(-buff_id);
	}
	if (game_logic_utilities::is_mob_skill(buff_id)) {
		return buff_source::from_mob_skill(
			buff_id,
			static_cast<game_mob_skill_level>(get_buff_level(buff_source_type::mob_skill, buff_id)));
	}
	return buff_source::from_skill(
		buff_id,
		static_cast<game_skill_level>(get_buff_level(buff_source_type::skill, buff_id)));
}

auto player_active_buffs::translate_to_packet(const buff_source &source) const -> int32_t {
	switch (source.get_type()) {
		case buff_source_type::item: return -source.get_id();
		case buff_source_type::skill: return source.get_id();
		case buff_source_type::mob_skill: return (source.get_mob_skill_level() << 16) | source.get_id();
	}
	throw not_implemented_exception{"buff_source_type"};
}

auto player_active_buffs::add_buff(const buff_source &source, const buff &buff, const seconds &time) -> result {
	bool has_timer = true;
	bool displaces = true;
	const auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto &skill_provider = channel_server::get_instance().get_skill_data_provider();
	auto skill = source.get_skill_data(skill_provider);
	auto mob_skill = source.get_mob_skill_data(skill_provider);

	switch (source.get_type()) {
		case buff_source_type::item:
			// Intentionally blank
			break;
		case buff_source_type::mob_skill: {
			game_mob_skill_id skill_id = source.get_mob_skill_id();
			int32_t mask_bit = calculate_debuff_mask_bit(skill_id);
			m_debuff_mask |= mask_bit;
			break;
		}
		case buff_source_type::skill: {
			game_skill_id skill_id = source.get_skill_id();
			game_skill_level skill_level = source.get_skill_level();
			switch (source.get_skill_id()) {
				case vana::skills::beginner::monster_rider:
				case vana::skills::noblesse::monster_rider: {
					m_mount_item_id = m_player->get_inventory()->get_equipped_id(equip_slots::mount);
					if (m_mount_item_id == 0) {
						// Hacking
						return result::failure;
					}
					game_item_id saddle = m_player->get_inventory()->get_equipped_id(equip_slots::saddle);
					if (saddle == 0) {
						// Hacking
						return result::failure;
					}
					break;
				}
				case vana::skills::corsair::battleship:
					m_mount_item_id = items::battleship_mount;
					break;
				case vana::skills::hero::enrage:
					if (m_combo != vana::skills::max_advanced_combo_orbs) {
						// Hacking
						return result::failure;
					}
					break;
			}

			break;
		}
		default: throw not_implemented_exception{"buff_source_type"};
	}

	// Extract any useful bits for us
	for (const auto &info : buff.get_buff_info()) {
		if (info == basics.homing_beacon) {
			has_timer = false;
			displaces = false;
		}
		else if (info == basics.combo) {
			m_combo = 0;
		}
		else if (info == basics.zombify) {
			if (mob_skill == nullptr) throw not_implemented_exception{"zombify buff_source_type"};
			m_zombify_potency = mob_skill->x;
		}
		else if (info == basics.maple_warrior) {
			if (skill == nullptr) throw not_implemented_exception{"maple warrior buff_source_type"};
			// Take into account Maple Warrior for tracking stats if things are equippable, damage calculations, etc.
			m_player->get_stats()->set_maple_warrior(skill->x);
		}
		else if (info == basics.hyper_body_hp) {
			if (skill == nullptr) throw not_implemented_exception{"hyper body h_p buff_source_type"};
			m_player->get_stats()->set_hyper_body_hp(skill->x);
		}
		else if (info == basics.hyper_body_mp) {
			if (skill == nullptr) throw not_implemented_exception{"hyper body m_p buff_source_type"};
			m_player->get_stats()->set_hyper_body_mp(skill->y);
		}
	}

	vana::timer::id buff_timer_id{timer_type::buff_timer, static_cast<int32_t>(source.get_type()), source.get_id()};
	if (has_timer) {
		// Get rid of timers/same buff if they currently exist
		m_player->get_timer_container()->remove_timer(buff_timer_id);
	}

	if (buff.any_acts()) {
		for (const auto &info : buff.get_buff_info()) {
			if (!info.has_act()) continue;
			vana::timer::id act_id{timer_type::skill_act_timer, info.get_bit_position()};
			m_player->get_timer_container()->remove_timer(act_id);
		}
	}

	for (size_t i = 0; i < m_buffs.size(); i++) {
		auto &existing = m_buffs[i];
		if (existing.type == source.get_type() && existing.identifier == source.get_id()) {
			m_buffs.erase(std::begin(m_buffs) + i);
			break;
		}
	}

	if (displaces) {
		// Displace bit positions
		// It is implicitly assumed in the buffs system that only one buff may "own" a particular bit position at once
		// Therefore, if you use Haste and then a Speed Potion, Haste will still apply to jump while the potion will apply to speed
		// This means that we should be keeping track of which bit positions are currently applicable to any given buff
		for (size_t i = 0; i < m_buffs.size(); i++) {
			auto &existing = m_buffs[i];
			const auto &existing_buff_info = existing.raw.get_buff_info();
			vector<uint8_t> displaced_bits;
			for (const auto &existing_info : existing_buff_info) {
				for (const auto &info : buff.get_buff_info()) {
					if (info == existing_info) {
						// NOTE
						// This code assumes that there will not be two of a particular bit position allocated currently
						displaced_bits.push_back(info.get_bit_position());
					}
				}
			}

			if (displaced_bits.size() > 0) {
				vector<buff_info> applicable;
				vector<uint8_t> displaced_act_bit_positions;

				for (const auto &existing_info : existing_buff_info) {
					bool found = false;
					for (auto bit : displaced_bits) {
						if (bit == existing_info) {
							found = true;
							break;
						}
					}
					if (!found) {
						applicable.push_back(existing_info);
					}
					else if (existing_info.has_act()) {
						displaced_act_bit_positions.push_back(existing_info.get_bit_position());
					}
				}

				for (const auto &bit : displaced_act_bit_positions) {
					vana::timer::id act_id{timer_type::skill_act_timer, bit};
					m_player->get_timer_container()->remove_timer(act_id);
				}

				if (applicable.size() == 0) {
					vana::timer::id id{timer_type::buff_timer, static_cast<int32_t>(existing.type), existing.identifier};
					m_player->get_timer_container()->remove_timer(id);

					m_buffs.erase(std::begin(m_buffs) + i);
					i--;
				}
				else {
					existing.raw = existing.raw.with_buffs(applicable);
				}
			}
		}
	}

	if (has_timer) {
		vana::timer::timer::create(
			[this, source](const time_point &now) {
				skills::stop_skill(m_player, source, true);
			},
			buff_timer_id,
			m_player->get_timer_container(),
			time);

		if (buff.any_acts()) {
			for (const auto &info : buff.get_buff_info()) {
				if (!info.has_act()) continue;

				buff_run_action run_act{source};
				run_act.player = m_player;
				run_act.act = info.get_act();
				run_act.value = buffs::get_value(
					m_player,
					source,
					seconds{0},
					info.get_bit_position(),
					info.get_act_value(),
					2).value;

				vana::timer::id act_id{timer_type::skill_act_timer, info.get_bit_position()};
				vana::timer::timer::create(
					run_act,
					act_id,
					m_player->get_timer_container(),
					seconds{0},
					duration_cast<milliseconds>(info.get_act_interval()));
			}
		}
	}

	local_buff_info local;
	local.raw = buff;
	local.type = source.get_type();
	local.identifier = source.get_id();
	switch (source.get_type()) {
		case buff_source_type::item:
		case buff_source_type::skill: local.level = source.get_skill_level(); break;
		case buff_source_type::mob_skill: local.level = source.get_mob_skill_level(); break;
		default: throw not_implemented_exception{"buff_source_type"};
	}

	m_buffs.push_back(local);

	m_player->send_map(
		packets::add_buff(
			m_player->get_id(),
			translate_to_packet(source),
			time,
			buffs::convert_to_packet(m_player, source, time, buff),
			0));

	return result::successful;
}

auto player_active_buffs::remove_buff(const buff_source &source, const buff &buff, bool from_timer) -> void {
	if (!from_timer) {
		vana::timer::id id{timer_type::buff_timer, static_cast<int32_t>(source.get_type()), source.get_id()};
		m_player->get_timer_container()->remove_timer(id);
	}

	auto basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	size_t size = m_buffs.size();
	for (size_t i = 0; i < size; i++) {
		const auto &info = m_buffs[i];
		if (info.type == source.get_type() && info.identifier == source.get_id()) {
			m_player->send_map(
				packets::end_buff(
					m_player->get_id(),
					buffs::convert_to_packet_types(info.raw)));

			for (const auto &act_info : info.raw.get_buff_info()) {
				if (!act_info.has_act()) continue;
				vana::timer::id act_id{timer_type::skill_act_timer, act_info.get_bit_position()};
				m_player->get_timer_container()->remove_timer(act_id);
			}

			for (const auto &buff_info : info.raw.get_buff_info()) {
				if (buff_info == basics.mount) {
					m_mount_item_id = 0;
				}
				else if (buff_info == basics.energy_charge) {
					m_energy_charge = 0;
				}
				else if (buff_info == basics.combo) {
					m_combo = 0;
				}
				else if (buff_info == basics.zombify) {
					m_zombify_potency = 0;
				}
				else if (buff_info == basics.homing_beacon) {
					reset_homing_beacon_mob();
				}
				else if (buff_info == basics.maple_warrior) {
					m_player->get_stats()->set_maple_warrior(0);
				}
				else if (buff_info == basics.hyper_body_hp) {
					m_player->get_stats()->set_hyper_body_hp(0);
				}
				else if (buff_info == basics.hyper_body_mp) {
					m_player->get_stats()->set_hyper_body_mp(0);
				}
			}

			switch (info.type) {
				case buff_source_type::mob_skill: {
					game_mob_skill_id skill_id = source.get_mob_skill_id();
					int32_t mask_bit = calculate_debuff_mask_bit(skill_id);
					m_debuff_mask -= mask_bit;
					break;
				}
			}

			m_buffs.erase(m_buffs.begin() + i);
			break;
		}
	}
}

auto player_active_buffs::remove_buffs() -> void {
	while (m_buffs.size() > 0) {
		local_buff_info &buff = *std::begin(m_buffs);
		remove_buff(buff.to_source(), buff.raw);
	}
}

auto player_active_buffs::get_buff_seconds_remaining(buff_source_type type, int32_t buff_id) const -> seconds {
	vana::timer::id id{timer_type::buff_timer, static_cast<int32_t>(type), buff_id};
	return m_player->get_timer_container()->get_remaining_time<seconds>(id);
}

auto player_active_buffs::get_buff_seconds_remaining(const buff_source &source) const -> seconds {
	return get_buff_seconds_remaining(source.get_type(), source.get_id());
}

// Debuffs
auto player_active_buffs::remove_debuff(game_mob_skill_id skill_id) -> void {
	int32_t mask_bit = calculate_debuff_mask_bit(skill_id);
	if ((m_debuff_mask & mask_bit) != 0) {
		skills::stop_skill(
			m_player,
			buff_source::from_mob_skill(
				skill_id,
				get_buff_level(buff_source_type::mob_skill, skill_id)),
			false);
	}
}

auto player_active_buffs::use_debuff_healing_item(int32_t mask) -> void {
	if ((mask & status_effects::player::seal) != 0) {
		remove_debuff(mob_skills::seal);
	}
	if ((mask & status_effects::player::poison) != 0) {
		remove_debuff(mob_skills::poison);
	}
	if ((mask & status_effects::player::curse) != 0) {
		remove_debuff(mob_skills::curse);
	}
	if ((mask & status_effects::player::darkness) != 0) {
		remove_debuff(mob_skills::darkness);
	}
	if ((mask & status_effects::player::weakness) != 0) {
		remove_debuff(mob_skills::weakness);
	}
}

auto player_active_buffs::get_zombified_potency(int16_t base_potency) -> int16_t {
	if ((m_debuff_mask & status_effects::player::zombify) != 0) {
		return static_cast<int16_t>(
			static_cast<int32_t>(base_potency)
			* m_zombify_potency
			/ 100);
	}
	return base_potency;
}

auto player_active_buffs::use_player_dispel() -> void {
	remove_debuff(mob_skills::seal);
	remove_debuff(mob_skills::slow);
	remove_debuff(mob_skills::darkness);
	remove_debuff(mob_skills::weakness);
	remove_debuff(mob_skills::curse);
	remove_debuff(mob_skills::poison);
}

auto player_active_buffs::calculate_debuff_mask_bit(game_mob_skill_id skill_id) const -> int32_t {
	switch (skill_id) {
		case mob_skills::seal: return status_effects::player::seal;
		case mob_skills::darkness: return status_effects::player::darkness;
		case mob_skills::weakness: return status_effects::player::weakness;
		case mob_skills::stun: return status_effects::player::stun;
		case mob_skills::curse: return status_effects::player::curse;
		case mob_skills::poison: return status_effects::player::poison;
		case mob_skills::slow: return status_effects::player::slow;
		case mob_skills::seduce: return status_effects::player::seduce;
		case mob_skills::zombify: return status_effects::player::zombify;
		case mob_skills::crazy_skull: return status_effects::player::crazy_skull;
	}
	return 0;
}

auto player_active_buffs::get_map_buff_values() -> buff_packet_structure {
	auto &buff_provider = channel_server::get_instance().get_buff_data_provider();
	auto &basics = buff_provider.get_buffs_by_effect();
	buff_packet_structure result;

	using tuple_type = tuple<uint8_t, const buff_info *, buff_source>;
	vector<tuple_type> map_buffs;

	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.get_buff_info()) {
			if (info.has_map_info()) {
				map_buffs.emplace_back(
					info.get_bit_position(),
					&info,
					buff.to_source());
			}
		}
	}

	std::sort(std::begin(map_buffs), std::end(map_buffs), [](const tuple_type &a, const tuple_type &b) -> bool {
		return std::get<0>(a) < std::get<0>(b);
	});

	for (const auto &tup : map_buffs) {
		const auto &info = std::get<1>(tup);
		const auto &source = std::get<2>(tup);

		result.types[info->get_buff_byte()] |= info->get_buff_type();
		result.values.push_back(buffs::get_value(
			m_player,
			source,
			get_buff_seconds_remaining(source),
			info->get_bit_position(),
			info->get_map_info()));
	}

	return result;
}

// Active skill levels
auto player_active_buffs::get_buff_level(buff_source_type type, int32_t buff_id) const -> game_skill_level {
	for (const auto &buff : m_buffs) {
		if (buff.type != type) continue;
		if (buff.identifier != buff_id) continue;
		return static_cast<game_skill_level>(buff.level);
	}
	return 0;
}

auto player_active_buffs::get_buff_skill_info(const buff_source &source) const -> const skill_level_info * const {
	if (source.get_type() != buff_source_type::skill) throw std::invalid_argument{"source must be buff_source_type::skill"};
	return source.get_skill_data(channel_server::get_instance().get_skill_data_provider());
}

auto player_active_buffs::stop_skill(const buff_source &source) -> void {
	skills::stop_skill(m_player, source);
}

// Buff addition/removal
auto player_active_buffs::dispel_buffs() -> void {
	if (m_player->has_gm_benefits()) {
		return;
	}

	vector<buff_source> stop_skills;
	for (const auto &buff : m_buffs) {
		if (buff.type == buff_source_type::skill) {
			stop_skills.push_back(buff.to_source());
		}
	}

	for (const auto &skill : stop_skills) {
		skills::stop_skill(m_player, skill);
	}
}

// Specific skill stuff
auto player_active_buffs::get_battleship_hp() const -> int32_t {
	return m_battleship_hp;
}

auto player_active_buffs::reset_battleship_hp() -> void {
	game_skill_level ship_level = m_player->get_skills()->get_skill_level(vana::skills::corsair::battleship);
	game_player_level player_level = m_player->get_stats()->get_level();
	m_battleship_hp = game_logic_utilities::get_battleship_hp(ship_level, player_level);
}

auto player_active_buffs::get_homing_beacon_mob() const -> game_map_object {
	return m_marked_mob;
}

auto player_active_buffs::reset_homing_beacon_mob(game_map_object map_mob_id) -> void {
	map *map = m_player->get_map();
	if (m_marked_mob != 0) {
		if (ref_ptr<mob> mob = map->get_mob(map_mob_id)) {
			auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
			auto source = get_buff_source(basics.homing_beacon);
			auto &buff_source = source.get();

			mob->remove_marker(m_player);
			m_player->send_map(
				packets::end_buff(
					m_player->get_id(),
					buffs::convert_to_packet_types(
						buffs::preprocess_buff(
							m_player,
							buff_source,
							seconds{0}))));
		}
	}
	m_marked_mob = map_mob_id;
	if (map_mob_id != 0) {
		map->get_mob(map_mob_id)->add_marker(m_player);
	}
}

auto player_active_buffs::reset_combo() -> void {
	set_combo(0);
}

auto player_active_buffs::set_combo(uint8_t combo) -> void {
	m_combo = combo;

	auto source = get_combo_source();
	auto &buff_source = source.get();
	seconds time_left = get_buff_seconds_remaining(buff_source);

	m_player->send_map(
		packets::add_buff(
			m_player->get_id(),
			buff_source.get_id(),
			time_left,
			buffs::convert_to_packet(
				m_player,
				buff_source,
				time_left,
				buffs::preprocess_buff(m_player, buff_source, time_left)),
			0));
}

auto player_active_buffs::add_combo() -> void {
	auto source = get_combo_source();
	if (source.is_initialized()) {
		auto &buff_source = source.get();
		game_skill_id adv_skill = m_player->get_skills()->get_advanced_combo();
		game_skill_level adv_combo = m_player->get_skills()->get_skill_level(adv_skill);
		auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(
			adv_combo > 0 ? adv_skill : buff_source.get_skill_id(),
			adv_combo > 0 ? adv_combo : buff_source.get_skill_level());

		int8_t max_combo = static_cast<int8_t>(skill->x);
		if (m_combo == max_combo) {
			return;
		}

		if (adv_combo > 0 && randomizer::percentage<uint16_t>() < skill->prop) {
			m_combo += 1;
		}
		m_combo += 1;
		if (m_combo > max_combo) {
			m_combo = max_combo;
		}

		set_combo(m_combo);
	}
}

auto player_active_buffs::get_combo() const -> uint8_t {
	return m_combo;
}

auto player_active_buffs::get_berserk() const -> bool {
	return m_berserk;
}

auto player_active_buffs::check_berserk(bool display) -> void {
	if (m_player->get_stats()->get_job() == jobs::job_ids::dark_knight) {
		// Berserk calculations
		game_skill_id skill_id = vana::skills::dark_knight::berserk;
		game_skill_level level = m_player->get_skills()->get_skill_level(skill_id);
		if (level > 0) {
			int16_t hp_percentage = m_player->get_stats()->get_max_hp() * channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level)->x / 100;
			game_health hp = m_player->get_stats()->get_hp();
			bool change = false;
			if (m_berserk && hp > hp_percentage) {
				m_berserk = false;
				change = true;
			}
			else if (!m_berserk && hp <= hp_percentage) {
				m_berserk = true;
				change = true;
			}
			if (change || display) {
				m_player->send_map(packets::skills::show_berserk(m_player->get_id(), level, m_berserk));
			}
		}
	}
}

auto player_active_buffs::get_energy_charge_level() const -> int16_t {
	return m_energy_charge;
}

auto player_active_buffs::increase_energy_charge_level(int8_t targets) -> void {
	if (m_energy_charge == stats::max_energy_charge_level) {
		// Buff is currently engaged
		return;
	}

	if (targets > 0) {
		stop_energy_charge_timer();

		game_skill_id skill_id = m_player->get_skills()->get_energy_charge();
		auto info = m_player->get_skills()->get_skill_info(skill_id);
		m_energy_charge += info->x * targets;
		m_energy_charge = std::min(m_energy_charge, stats::max_energy_charge_level);

		if (m_energy_charge == stats::max_energy_charge_level) {
			buffs::add_buff(m_player, skill_id, m_player->get_skills()->get_skill_level(skill_id), 0);
		}
		else {
			start_energy_charge_timer();
			buff_source source = buff_source::from_skill(skill_id, info->level);
			buff buff{{channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect().energy_charge}};
			m_player->send(
				packets::add_buff(
					m_player->get_id(),
					translate_to_packet(source),
					seconds{0},
					buffs::convert_to_packet(m_player, source, seconds{0}, buff),
					0));
		}
	}
}

auto player_active_buffs::decrease_energy_charge_level() -> void {
	m_energy_charge -= stats::energy_charge_decay;
	m_energy_charge = std::max<int16_t>(m_energy_charge, 0);
	if (m_energy_charge > 0) {
		start_energy_charge_timer();
	}

	game_skill_id skill_id = m_player->get_skills()->get_energy_charge();
	auto info = m_player->get_skills()->get_skill_info(skill_id);
	buff_source source = buff_source::from_skill(skill_id, info->level);
	buff buff{{channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect().energy_charge}};
	m_player->send(
		packets::add_buff(
			m_player->get_id(),
			translate_to_packet(source),
			seconds{0},
			buffs::convert_to_packet(m_player, source, seconds{0}, buff),
			0));
}

auto player_active_buffs::start_energy_charge_timer() -> void {
	game_skill_id skill_id = m_player->get_skills()->get_energy_charge();
	m_energy_charge_timer_counter++;
	vana::timer::id id{timer_type::energy_charge_timer, skill_id, m_energy_charge_timer_counter};
	vana::timer::timer::create(
		[this](const time_point &now) {
			this->decrease_energy_charge_level();
		},
		id,
		m_player->get_timer_container(),
		seconds{10});
}

auto player_active_buffs::stop_energy_charge_timer() -> void {
	game_skill_id skill_id = m_player->get_skills()->get_energy_charge();
	vana::timer::id id{timer_type::energy_charge_timer, skill_id, m_energy_charge_timer_counter};
	m_player->get_timer_container()->remove_timer(id);
}

auto player_active_buffs::stop_booster() -> void {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto source = get_buff_source(basics.booster);
	if (source.is_initialized()) {
		stop_skill(source.get());
	}
}

auto player_active_buffs::stop_charge() -> void {
	auto source = get_charge_source();
	if (source.is_initialized()) {
		stop_skill(source.get());
	}
}

auto player_active_buffs::stop_bullet_skills() -> void {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();

	auto soul_arrow = get_buff_source(basics.soul_arrow);
	if (soul_arrow.is_initialized()) {
		stop_skill(soul_arrow.get());
	}

	auto shadow_stars = get_buff_source(basics.shadow_stars);
	if (shadow_stars.is_initialized()) {
		stop_skill(shadow_stars.get());
	}
}

auto player_active_buffs::has_buff(buff_source_type type, int32_t buff_id) const -> bool {
	for (const auto &buff : m_buffs) {
		if (buff.type != type) continue;
		if (buff.identifier != buff_id) continue;
		return true;
	}
	return false;
}

auto player_active_buffs::has_buff(const buff_info &buff) const -> bool {
	return has_buff(buff.get_bit_position());
}

auto player_active_buffs::has_buff(uint8_t bit_position) const -> bool {
	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.get_buff_info()) {
			if (bit_position == info) {
				return true;
			}
		}
	}
	return false;
}

auto player_active_buffs::get_buff_source(const buff_info &buff) const -> optional<buff_source> {
	return get_buff(buff.get_bit_position());
}

auto player_active_buffs::get_buff(uint8_t bit_position) const -> optional<buff_source> {
	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.get_buff_info()) {
			if (bit_position == info) {
				return buff.to_source();
			}
		}
	}
	return optional<buff_source>{};
}

auto player_active_buffs::has_ice_charge() const -> bool {
	auto source = get_charge_source();
	if (!source.is_initialized()) return false;
	auto &buff_source = source.get();
	if (buff_source.get_type() != buff_source_type::skill) throw not_implemented_exception{"ice charge buff_source_type"};
	game_skill_id skill_id = buff_source.get_skill_id();
	return
		skill_id == vana::skills::white_knight::bw_ice_charge ||
		skill_id == vana::skills::white_knight::sword_ice_charge;
}

auto player_active_buffs::get_pickpocket_counter() -> int32_t {
	return ++m_pickpocket_counter;
}

auto player_active_buffs::has_infinity() const -> bool {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return has_buff(basics.infinity);
}

auto player_active_buffs::is_using_gm_hide() const -> bool {
	return has_buff(buff_source_type::skill, vana::skills::super_gm::hide);
}

auto player_active_buffs::has_shadow_partner() const -> bool {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return has_buff(basics.shadow_partner);
}

auto player_active_buffs::has_shadow_stars() const -> bool {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return has_buff(basics.shadow_stars);
}

auto player_active_buffs::has_soul_arrow() const -> bool {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return has_buff(basics.soul_arrow);
}

auto player_active_buffs::has_holy_shield() const -> bool {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return has_buff(basics.holy_shield);
}

auto player_active_buffs::is_cursed() const -> bool {
	return (m_debuff_mask & status_effects::player::curse) > 0;
}

auto player_active_buffs::get_holy_symbol_rate() const -> int16_t {
	int16_t val = 0;
	auto source = get_holy_symbol_source();
	if (source.is_initialized()) {
		auto &buff_source = source.get();
		if (buff_source.get_type() != buff_source_type::skill) throw not_implemented_exception{"holy symbol buff_source_type"};
		val = get_buff_skill_info(buff_source)->x;
	}
	return val;
}

auto player_active_buffs::get_magic_guard_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.magic_guard);
}

auto player_active_buffs::get_meso_guard_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.meso_guard);
}

auto player_active_buffs::get_meso_up_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.meso_up);
}

auto player_active_buffs::get_homing_beacon_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.homing_beacon);
}

auto player_active_buffs::get_combo_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.combo);
}

auto player_active_buffs::get_charge_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.charge);
}

auto player_active_buffs::get_dark_sight_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto dark_sight = get_buff_source(basics.dark_sight);
	if (dark_sight.is_initialized()) return dark_sight;
	return get_buff_source(basics.wind_walk);
}

auto player_active_buffs::get_pickpocket_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.pickpocket);
}

auto player_active_buffs::get_hamstring_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.hamstring);
}

auto player_active_buffs::get_blind_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.blind);
}

auto player_active_buffs::get_concentrate_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.concentrate);
}

auto player_active_buffs::get_holy_symbol_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.holy_symbol);
}

auto player_active_buffs::get_power_stance_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto ret = get_buff_source(basics.power_stance);
	if (ret.is_initialized()) return ret;
	ret = get_buff_source(basics.energy_charge);
	return ret;
}

auto player_active_buffs::get_hyper_body_hp_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.hyper_body_hp);
}

auto player_active_buffs::get_hyper_body_mp_source() const -> optional<buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.hyper_body_mp);
}

auto player_active_buffs::get_mount_item_id() const -> game_item_id {
	return m_mount_item_id;
}

auto player_active_buffs::end_morph() -> void {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto source = get_buff_source(basics.morph);
	if (source.is_initialized()) {
		stop_skill(source.get());
	}
}

auto player_active_buffs::swap_weapon() -> void {
	stop_booster();
	stop_charge();
	stop_bullet_skills();
}

auto player_active_buffs::take_damage(game_damage damage) -> void {
	if (damage <= 0) return;

	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto source = get_buff_source(basics.morph);
	if (source.is_initialized()) {
		auto &buff_source = source.get();
		if (buff_source.get_type() == buff_source_type::item) {
			stop_skill(buff_source);
		}
	}

	auto battleship_level = get_buff_level(buff_source_type::skill, vana::skills::corsair::battleship);
	if (battleship_level > 0) {
		m_battleship_hp -= damage / 10;
		auto source = buff_source::from_skill(vana::skills::corsair::battleship, battleship_level);

		if (m_battleship_hp <= 0) {
			m_battleship_hp = 0;
			seconds cool_time = get_buff_skill_info(source)->cool_time;
			skills::start_cooldown(m_player, source.get_skill_id(), cool_time);
			stop_skill(source);
		}
		else {
			packets::add_buff(
				m_player->get_id(),
				source.get_skill_id(),
				seconds{0},
				buffs::convert_to_packet(
					m_player,
					source,
					seconds{0},
					buffs::preprocess_buff(m_player, source, seconds{0})),
				0);
		}
	}
}

auto player_active_buffs::get_transfer_packet() const -> packet_builder {
	packet_builder builder;
	builder
		.add<int8_t>(m_combo)
		.add<int16_t>(m_energy_charge)
		.add<int32_t>(m_battleship_hp)
		.add<int32_t>(m_debuff_mask)
		.add<game_item_id>(m_mount_item_id);

	// Current buff info (IDs, times, levels)
	builder.add<uint16_t>(static_cast<uint16_t>(m_buffs.size()));
	for (const auto &buff : m_buffs) {
		auto &raw = buff.raw;

		builder.add<buff_source_type>(buff.type);
		builder.add<int32_t>(buff.identifier);
		switch (buff.type) {
			case buff_source_type::item:
			case buff_source_type::skill:
				builder.add<game_skill_level>(buff.level);
				break;
			case buff_source_type::mob_skill:
				builder.add<game_mob_skill_level>(static_cast<game_mob_skill_level>(buff.level));
				break;
			default: throw not_implemented_exception{"buff_source_type"};
		}
		builder.add<seconds>(get_buff_seconds_remaining(buff.type, buff.identifier));

		auto &buffs = raw.get_buff_info();
		builder.add<uint8_t>(static_cast<uint8_t>(buffs.size()));
		for (const auto &info : buffs) {
			builder.add<uint8_t>(info.get_bit_position());
		}
	}

	return builder;
}

auto player_active_buffs::parse_transfer_packet(packet_reader &reader) -> void {
	// Map entry buff info
	m_combo = reader.get<uint8_t>();
	m_energy_charge = reader.get<int16_t>();
	m_battleship_hp = reader.get<int32_t>();
	m_debuff_mask = reader.get<int32_t>();
	m_mount_item_id = reader.get<game_item_id>();

	// Current player skill/item buff info
	size_t size = reader.get<uint16_t>();
	for (size_t i = 0; i < size; ++i) {
		buff_source_type type = reader.get<buff_source_type>();
		int32_t identifier = reader.get<int32_t>();
		int32_t level = 0;
		switch (type) {
			case buff_source_type::item:
			case buff_source_type::skill: {
				level = reader.get<game_skill_level>();
				break;
			}
			case buff_source_type::mob_skill: {
				level = reader.get<game_mob_skill_level>();
				break;
			}
			default: throw not_implemented_exception{"buff_source_type"};
		}

		local_buff_info buff;
		buff.type = type;
		buff.identifier = identifier;
		buff.level = level;

		seconds time_left = reader.get<seconds>();
		vector<uint8_t> valid_bits;
		uint8_t valid_bit_size = reader.get<uint8_t>();
		for (uint8_t i = 0; i < valid_bit_size; i++) {
			valid_bits.push_back(reader.get<uint8_t>());
		}

		buff_source source = buff.to_source();
		int32_t packet_skill_id = translate_to_packet(source);
		buff.raw = buffs::preprocess_buff(
			buffs::preprocess_buff(
				m_player,
				source,
				time_left),
			valid_bits);

		m_buffs.push_back(buff);

		vana::timer::id id{timer_type::buff_timer, static_cast<int32_t>(buff.type), buff.identifier};
		vana::timer::timer::create(
			[this, packet_skill_id](const time_point &now) {
				skills::stop_skill(m_player, translate_to_source(packet_skill_id), true);
			},
			id,
			m_player->get_timer_container(),
			time_left);
	}

	if (m_energy_charge > 0 && m_energy_charge != stats::max_energy_charge_level) {
		start_energy_charge_timer();
	}

	auto hyper_body_hp_source = get_hyper_body_hp_source();
	if (hyper_body_hp_source.is_initialized()) {
		auto skill = get_buff_skill_info(hyper_body_hp_source.get());
		m_player->get_stats()->set_hyper_body_hp(skill->x);
	}
	auto hyper_body_mp_source = get_hyper_body_mp_source();
	if (hyper_body_mp_source.is_initialized()) {
		auto skill = get_buff_skill_info(hyper_body_mp_source.get());
		m_player->get_stats()->set_hyper_body_mp(skill->y);
	}
}

}
}