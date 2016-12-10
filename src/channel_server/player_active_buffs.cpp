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
#include "player_active_buffs.hpp"
#include "common/data/provider/skill.hpp"
#include "common/packet_reader.hpp"
#include "common/timer/container.hpp"
#include "common/timer/timer.hpp"
#include "common/util/game_logic/mob_skill.hpp"
#include "common/util/game_logic/player_skill.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/time.hpp"
#include "channel_server/buffs_packet.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/player.hpp"
#include "channel_server/skills.hpp"
#include "channel_server/skills_packet.hpp"
#include <functional>

namespace vana {
namespace channel_server {

struct buff_run_action {
	buff_run_action(data::type::buff_source source) :
		source{source}
	{
	}

	auto operator()(const time_point &now) -> void {
		switch (act) {
			case data::type::buff_action::heal: skills::heal(player, value, source); break;
			case data::type::buff_action::hurt: skills::hurt(player, value, source); break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "action type");
		}
	}

	int64_t value = 0;
	data::type::buff_source source;
	ref_ptr<player> player = nullptr;
	data::type::buff_action act;
};

auto player_active_buffs::local_buff_info::to_source() const -> data::type::buff_source {
	switch (type) {
		case data::type::buff_source_type::item:
			return data::type::buff_source::from_item(identifier);
		case data::type::buff_source_type::skill:
			return data::type::buff_source::from_skill(
				static_cast<game_skill_id>(identifier),
				static_cast<game_skill_level>(level));
		case data::type::buff_source_type::mob_skill:
			return data::type::buff_source::from_mob_skill(
				static_cast<game_mob_skill_id>(identifier),
				static_cast<game_mob_skill_level>(level));
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
}

player_active_buffs::player_active_buffs(ref_ptr<player> player) :
	m_player{player}
{
}

// Buff skills
auto player_active_buffs::translate_to_source(int32_t buff_id) const -> data::type::buff_source {
	if (buff_id < 0) {
		return data::type::buff_source::from_item(-buff_id);
	}
	if (vana::util::game_logic::mob_skill::is_mob_skill(buff_id)) {
		return data::type::buff_source::from_mob_skill(
			buff_id,
			static_cast<game_mob_skill_level>(get_buff_level(data::type::buff_source_type::mob_skill, buff_id)));
	}
	return data::type::buff_source::from_skill(
		buff_id,
		static_cast<game_skill_level>(get_buff_level(data::type::buff_source_type::skill, buff_id)));
}

auto player_active_buffs::translate_to_packet(const data::type::buff_source &source) const -> int32_t {
	switch (source.get_type()) {
		case data::type::buff_source_type::item: return -source.get_id();
		case data::type::buff_source_type::skill: return source.get_id();
		case data::type::buff_source_type::mob_skill: return (source.get_mob_skill_level() << 16) | source.get_id();
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
}

auto player_active_buffs::add_buff(const data::type::buff_source &source, const data::type::buff &buff, const seconds &time) -> result {
	bool has_timer = true;
	bool displaces = true;
	const auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto &skill_provider = channel_server::get_instance().get_skill_data_provider();
	auto skill = source.get_skill_data(skill_provider);
	auto mob_skill = source.get_mob_skill_data(skill_provider);
	if (auto player = m_player.lock()) {
		switch (source.get_type()) {
			case data::type::buff_source_type::item:
				// Intentionally blank
				break;
			case data::type::buff_source_type::mob_skill: {
				game_mob_skill_id skill_id = source.get_mob_skill_id();
				int32_t mask_bit = calculate_debuff_mask_bit(skill_id);
				m_debuff_mask |= mask_bit;
				break;
			}
			case data::type::buff_source_type::skill: {
				game_skill_id skill_id = source.get_skill_id();
				game_skill_level skill_level = source.get_skill_level();
				switch (source.get_skill_id()) {
					case constant::skill::beginner::monster_rider:
					case constant::skill::noblesse::monster_rider: {
						m_mount_item_id = player->get_inventory()->get_equipped_id(constant::equip_slot::mount);
						if (m_mount_item_id == 0) {
							// Hacking
							return result::failure;
						}
						game_item_id saddle = player->get_inventory()->get_equipped_id(constant::equip_slot::saddle);
						if (saddle == 0) {
							// Hacking
							return result::failure;
						}
						break;
					}
					case constant::skill::corsair::battleship:
						m_mount_item_id = constant::item::battleship_mount;
						break;
					case constant::skill::hero::enrage:
						if (m_combo != constant::skill::max_advanced_combo_orbs) {
							// Hacking
							return result::failure;
						}
						break;
				}

				break;
			}
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
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
				if (mob_skill == nullptr) THROW_CODE_EXCEPTION(not_implemented_exception, "zombify data::type::buff_source_type");
				m_zombify_potency = mob_skill->x;
			}
			else if (info == basics.maple_warrior) {
				if (skill == nullptr) THROW_CODE_EXCEPTION(not_implemented_exception, "maple warrior data::type::buff_source_type");
				// Take into account Maple Warrior for tracking stats if things are equippable, damage calculations, etc.
				player->get_stats()->set_maple_warrior(skill->x);
			}
			else if (info == basics.hyper_body_hp) {
				if (skill == nullptr) THROW_CODE_EXCEPTION(not_implemented_exception, "hyper body hp data::type::buff_source_type");
				player->get_stats()->set_hyper_body_hp(skill->x);
			}
			else if (info == basics.hyper_body_mp) {
				if (skill == nullptr) THROW_CODE_EXCEPTION(not_implemented_exception, "hyper body mp data::type::buff_source_type");
				player->get_stats()->set_hyper_body_mp(skill->y);
			}
		}

		vana::timer::id buff_timer_id{vana::timer::type::buff_timer, static_cast<int32_t>(source.get_type()), source.get_id()};
		if (has_timer) {
			// Get rid of timers/same buff if they currently exist
			player->get_timer_container()->remove_timer(buff_timer_id);
		}

		if (buff.any_acts()) {
			for (const auto &info : buff.get_buff_info()) {
				if (!info.has_act()) continue;
				vana::timer::id act_id{vana::timer::type::skill_act_timer, info.get_bit_position()};
				player->get_timer_container()->remove_timer(act_id);
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
					vector<data::type::buff_info> applicable;
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
						vana::timer::id act_id{vana::timer::type::skill_act_timer, bit};
						player->get_timer_container()->remove_timer(act_id);
					}

					if (applicable.size() == 0) {
						vana::timer::id id{vana::timer::type::buff_timer, static_cast<int32_t>(existing.type), existing.identifier};
						player->get_timer_container()->remove_timer(id);

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
				[player, source](const time_point &now) {
					skills::stop_skill(player, source, true);
				},
				buff_timer_id,
				player->get_timer_container(),
				time);

			if (buff.any_acts()) {
				for (const auto &info : buff.get_buff_info()) {
					if (!info.has_act()) continue;

					buff_run_action run_act{source};
					run_act.player = player;
					run_act.act = info.get_act();
					run_act.value = buffs::get_value(
						player,
						source,
						seconds{0},
						info.get_bit_position(),
						info.get_act_value(),
						2).value;

					vana::timer::id act_id{vana::timer::type::skill_act_timer, info.get_bit_position()};
					vana::timer::timer::create(
						run_act,
						act_id,
						player->get_timer_container(),
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
			case data::type::buff_source_type::item:
			case data::type::buff_source_type::skill: local.level = source.get_skill_level(); break;
			case data::type::buff_source_type::mob_skill: local.level = source.get_mob_skill_level(); break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
		}

		m_buffs.push_back(local);

		player->send_map(
			packets::add_buff(
				player->get_id(),
				translate_to_packet(source),
				time,
				buffs::convert_to_packet(player, source, time, buff),
				0));

		return result::success;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::remove_buff(const data::type::buff_source &source, const data::type::buff &buff, bool from_timer) -> void {
	if (auto player = m_player.lock()) {
		if (!from_timer) {
			vana::timer::id id{vana::timer::type::buff_timer, static_cast<int32_t>(source.get_type()), source.get_id()};
			player->get_timer_container()->remove_timer(id);
		}

		auto basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
		size_t size = m_buffs.size();
		for (size_t i = 0; i < size; i++) {
			const auto &info = m_buffs[i];
			if (info.type == source.get_type() && info.identifier == source.get_id()) {
				player->send_map(
					packets::end_buff(
						player->get_id(),
						buffs::convert_to_packet_types(info.raw)));

				for (const auto &act_info : info.raw.get_buff_info()) {
					if (!act_info.has_act()) continue;
					vana::timer::id act_id{vana::timer::type::skill_act_timer, act_info.get_bit_position()};
					player->get_timer_container()->remove_timer(act_id);
				}

				for (const auto &info_from_raw : info.raw.get_buff_info()) {
					if (info_from_raw == basics.mount) {
						m_mount_item_id = 0;
					}
					else if (info_from_raw == basics.energy_charge) {
						m_energy_charge = 0;
					}
					else if (info_from_raw == basics.combo) {
						m_combo = 0;
					}
					else if (info_from_raw == basics.zombify) {
						m_zombify_potency = 0;
					}
					else if (info_from_raw == basics.homing_beacon) {
						reset_homing_beacon_mob();
					}
					else if (info_from_raw == basics.maple_warrior) {
						player->get_stats()->set_maple_warrior(0);
					}
					else if (info_from_raw == basics.hyper_body_hp) {
						player->get_stats()->set_hyper_body_hp(0);
					}
					else if (info_from_raw == basics.hyper_body_mp) {
						player->get_stats()->set_hyper_body_mp(0);
					}
				}

				switch (info.type) {
					case data::type::buff_source_type::mob_skill: {
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
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::remove_buffs() -> void {
	while (m_buffs.size() > 0) {
		local_buff_info &buff = *std::begin(m_buffs);
		remove_buff(buff.to_source(), buff.raw);
	}
}

auto player_active_buffs::get_buff_seconds_remaining(data::type::buff_source_type type, int32_t buff_id) const -> seconds {
	vana::timer::id id{vana::timer::type::buff_timer, static_cast<int32_t>(type), buff_id};
	if (auto player = m_player.lock()) {
		return player->get_timer_container()->get_remaining_time<seconds>(id);
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::get_buff_seconds_remaining(const data::type::buff_source &source) const -> seconds {
	return get_buff_seconds_remaining(source.get_type(), source.get_id());
}

// Debuffs
auto player_active_buffs::remove_debuff(game_mob_skill_id skill_id) -> void {
	int32_t mask_bit = calculate_debuff_mask_bit(skill_id);
	if (auto player = m_player.lock()) {
		if ((m_debuff_mask & mask_bit) != 0) {
			skills::stop_skill(
				player,
				data::type::buff_source::from_mob_skill(
					skill_id,
					get_buff_level(data::type::buff_source_type::mob_skill, skill_id)),
				false);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::use_debuff_healing_item(int32_t mask) -> void {
	if ((mask & constant::status_effect::player::seal) != 0) {
		remove_debuff(constant::mob_skill::seal);
	}
	if ((mask & constant::status_effect::player::poison) != 0) {
		remove_debuff(constant::mob_skill::poison);
	}
	if ((mask & constant::status_effect::player::curse) != 0) {
		remove_debuff(constant::mob_skill::curse);
	}
	if ((mask & constant::status_effect::player::darkness) != 0) {
		remove_debuff(constant::mob_skill::darkness);
	}
	if ((mask & constant::status_effect::player::weakness) != 0) {
		remove_debuff(constant::mob_skill::weakness);
	}
}

auto player_active_buffs::get_zombified_potency(int16_t base_potency) -> int16_t {
	if ((m_debuff_mask & constant::status_effect::player::zombify) != 0) {
		return static_cast<int16_t>(
			static_cast<int32_t>(base_potency)
			* m_zombify_potency
			/ 100);
	}
	return base_potency;
}

auto player_active_buffs::use_player_dispel() -> void {
	remove_debuff(constant::mob_skill::seal);
	remove_debuff(constant::mob_skill::slow);
	remove_debuff(constant::mob_skill::darkness);
	remove_debuff(constant::mob_skill::weakness);
	remove_debuff(constant::mob_skill::curse);
	remove_debuff(constant::mob_skill::poison);
}

auto player_active_buffs::calculate_debuff_mask_bit(game_mob_skill_id skill_id) const -> int32_t {
	switch (skill_id) {
		case constant::mob_skill::seal: return constant::status_effect::player::seal;
		case constant::mob_skill::darkness: return constant::status_effect::player::darkness;
		case constant::mob_skill::weakness: return constant::status_effect::player::weakness;
		case constant::mob_skill::stun: return constant::status_effect::player::stun;
		case constant::mob_skill::curse: return constant::status_effect::player::curse;
		case constant::mob_skill::poison: return constant::status_effect::player::poison;
		case constant::mob_skill::slow: return constant::status_effect::player::slow;
		case constant::mob_skill::seduce: return constant::status_effect::player::seduce;
		case constant::mob_skill::zombify: return constant::status_effect::player::zombify;
		case constant::mob_skill::crazy_skull: return constant::status_effect::player::crazy_skull;
	}
	return 0;
}

auto player_active_buffs::get_map_buff_values() -> buff_packet_structure {
	auto &buff_provider = channel_server::get_instance().get_buff_data_provider();
	auto &basics = buff_provider.get_buffs_by_effect();
	buff_packet_structure result;

	using tuple_type = tuple<uint8_t, const data::type::buff_info *, data::type::buff_source>;
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

	if (auto player = m_player.lock()) {
		for (const auto &tup : map_buffs) {
			const auto &info = std::get<1>(tup);
			const auto &source = std::get<2>(tup);

			result.types[info->get_buff_byte()] |= info->get_buff_type();
			result.values.push_back(buffs::get_value(
				player,
				source,
				get_buff_seconds_remaining(source),
				info->get_bit_position(),
				info->get_map_info()));
		}

		return result;
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

// Active skill levels
auto player_active_buffs::get_buff_level(data::type::buff_source_type type, int32_t buff_id) const -> game_skill_level {
	for (const auto &buff : m_buffs) {
		if (buff.type != type) continue;
		if (buff.identifier != buff_id) continue;
		return static_cast<game_skill_level>(buff.level);
	}
	return 0;
}

auto player_active_buffs::get_buff_skill_info(const data::type::buff_source &source) const -> const data::type::skill_level_info * const {
	if (source.get_type() != data::type::buff_source_type::skill) throw std::invalid_argument{"source must be data::type::buff_source_type::skill"};
	return source.get_skill_data(channel_server::get_instance().get_skill_data_provider());
}

auto player_active_buffs::stop_skill(const data::type::buff_source &source) -> void {
	skills::stop_skill(m_player.lock(), source);
}

// Buff addition/removal
auto player_active_buffs::dispel_buffs() -> void {
	if (auto player = m_player.lock()) {
		if (player->has_gm_benefits()) {
			return;
		}

		vector<data::type::buff_source> stop_skills;
		for (const auto &buff : m_buffs) {
			if (buff.type == data::type::buff_source_type::skill) {
				stop_skills.push_back(buff.to_source());
			}
		}

		for (const auto &skill : stop_skills) {
			skills::stop_skill(player, skill);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

// Specific skill stuff
auto player_active_buffs::get_battleship_hp() const -> int32_t {
	return m_battleship_hp;
}

auto player_active_buffs::reset_battleship_hp() -> void {
	if (auto player = m_player.lock()) {
		game_skill_level ship_level = player->get_skills()->get_skill_level(constant::skill::corsair::battleship);
		game_player_level player_level = player->get_stats()->get_level();
		m_battleship_hp = vana::util::game_logic::player_skill::get_battleship_hp(ship_level, player_level);
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::get_homing_beacon_mob() const -> game_map_object {
	return m_marked_mob;
}

auto player_active_buffs::reset_homing_beacon_mob(game_map_object map_mob_id) -> void {
	if (auto player = m_player.lock()) {
		map *map = player->get_map();
		if (m_marked_mob != 0) {
			if (ref_ptr<mob> mob = map->get_mob(map_mob_id)) {
				auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
				auto source = get_buff_source(basics.homing_beacon);
				auto &buff_source = source.get();

				mob->remove_marker(player);
				player->send_map(
					packets::end_buff(
						player->get_id(),
						buffs::convert_to_packet_types(
							buffs::preprocess_buff(
								player,
								buff_source,
								seconds{0}))));
			}
		}
		m_marked_mob = map_mob_id;
		if (map_mob_id != 0) {
			map->get_mob(map_mob_id)->add_marker(player);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::reset_combo() -> void {
	set_combo(0);
}

auto player_active_buffs::set_combo(uint8_t combo) -> void {
	m_combo = combo;

	auto source = get_combo_source();
	auto &buff_source = source.get();
	seconds time_left = get_buff_seconds_remaining(buff_source);

	if (auto player = m_player.lock()) {
		player->send_map(
			packets::add_buff(
				player->get_id(),
				buff_source.get_id(),
				time_left,
				buffs::convert_to_packet(
					player,
					buff_source,
					time_left,
					buffs::preprocess_buff(player, buff_source, time_left)),
				0));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::add_combo() -> void {
	auto source = get_combo_source();
	if (source.is_initialized()) {
		auto &buff_source = source.get();
		if (auto player = m_player.lock()) {
			game_skill_id adv_skill = player->get_skills()->get_advanced_combo();
			game_skill_level adv_combo = player->get_skills()->get_skill_level(adv_skill);
			auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(
				adv_combo > 0 ? adv_skill : buff_source.get_skill_id(),
				adv_combo > 0 ? adv_combo : buff_source.get_skill_level());

			int8_t max_combo = static_cast<int8_t>(skill->x);
			if (m_combo == max_combo) {
				return;
			}

			if (adv_combo > 0 && vana::util::randomizer::percentage<uint16_t>() < skill->prop) {
				m_combo += 1;
			}
			m_combo += 1;
			if (m_combo > max_combo) {
				m_combo = max_combo;
			}

			set_combo(m_combo);
		}
		else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
	}
}

auto player_active_buffs::get_combo() const -> uint8_t {
	return m_combo;
}

auto player_active_buffs::get_berserk() const -> bool {
	return m_berserk;
}

auto player_active_buffs::check_berserk(bool display) -> void {
	if (auto player = m_player.lock()) {
		if (player->get_stats()->get_job() == constant::job::id::dark_knight) {
			// Berserk calculations
			game_skill_id skill_id = constant::skill::dark_knight::berserk;
			game_skill_level level = player->get_skills()->get_skill_level(skill_id);
			if (level > 0) {
				int16_t hp_percentage = player->get_stats()->get_max_hp() * channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level)->x / 100;
				game_health hp = player->get_stats()->get_hp();
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
					player->send_map(packets::skills::show_berserk(player->get_id(), level, m_berserk));
				}
			}
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::get_energy_charge_level() const -> int16_t {
	return m_energy_charge;
}

auto player_active_buffs::increase_energy_charge_level(int8_t targets) -> void {
	if (m_energy_charge == constant::stat::max_energy_charge_level) {
		// Buff is currently engaged
		return;
	}

	if (targets > 0) {
		stop_energy_charge_timer();

		if (auto player = m_player.lock()) {
			game_skill_id skill_id = player->get_skills()->get_energy_charge();
			auto info = player->get_skills()->get_skill_info(skill_id);
			m_energy_charge += info->x * targets;
			m_energy_charge = std::min(m_energy_charge, constant::stat::max_energy_charge_level);

			if (m_energy_charge == constant::stat::max_energy_charge_level) {
				buffs::add_buff(player, skill_id, player->get_skills()->get_skill_level(skill_id), 0);
			}
			else {
				start_energy_charge_timer();
				data::type::buff_source source = data::type::buff_source::from_skill(skill_id, info->level);
				data::type::buff buff{{channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect().energy_charge}};
				player->send(
					packets::add_buff(
						player->get_id(),
						translate_to_packet(source),
						seconds{0},
						buffs::convert_to_packet(player, source, seconds{0}, buff),
						0));
			}
		}
		else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
	}
}

auto player_active_buffs::decrease_energy_charge_level() -> void {
	m_energy_charge -= constant::stat::energy_charge_decay;
	m_energy_charge = std::max<int16_t>(m_energy_charge, 0);
	if (m_energy_charge > 0) {
		start_energy_charge_timer();
	}

	if (auto player = m_player.lock()) {
		game_skill_id skill_id = player->get_skills()->get_energy_charge();
		auto info = player->get_skills()->get_skill_info(skill_id);
		data::type::buff_source source = data::type::buff_source::from_skill(skill_id, info->level);
		data::type::buff buff{{channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect().energy_charge}};
		player->send(
			packets::add_buff(
				player->get_id(),
				translate_to_packet(source),
				seconds{0},
				buffs::convert_to_packet(player, source, seconds{0}, buff),
				0));
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::start_energy_charge_timer() -> void {
	if (auto player = m_player.lock()) {
		game_skill_id skill_id = player->get_skills()->get_energy_charge();
		m_energy_charge_timer_counter++;
		vana::timer::id id{vana::timer::type::energy_charge_timer, skill_id, m_energy_charge_timer_counter};
		vana::timer::timer::create(
			[this](const time_point &now) {
				this->decrease_energy_charge_level();
			},
			id,
			player->get_timer_container(),
			seconds{10});
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

auto player_active_buffs::stop_energy_charge_timer() -> void {
	if (auto player = m_player.lock()) {
		game_skill_id skill_id = player->get_skills()->get_energy_charge();
		vana::timer::id id{vana::timer::type::energy_charge_timer, skill_id, m_energy_charge_timer_counter};
		player->get_timer_container()->remove_timer(id);
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
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

auto player_active_buffs::has_buff(data::type::buff_source_type type, int32_t buff_id) const -> bool {
	for (const auto &buff : m_buffs) {
		if (buff.type != type) continue;
		if (buff.identifier != buff_id) continue;
		return true;
	}
	return false;
}

auto player_active_buffs::has_buff(const data::type::buff_info &buff) const -> bool {
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

auto player_active_buffs::get_buff_source(const data::type::buff_info &buff) const -> optional<data::type::buff_source> {
	return get_buff(buff.get_bit_position());
}

auto player_active_buffs::get_buff(uint8_t bit_position) const -> optional<data::type::buff_source> {
	for (const auto &buff : m_buffs) {
		for (const auto &info : buff.raw.get_buff_info()) {
			if (bit_position == info) {
				return buff.to_source();
			}
		}
	}
	return optional<data::type::buff_source>{};
}

auto player_active_buffs::has_ice_charge() const -> bool {
	auto source = get_charge_source();
	if (!source.is_initialized()) return false;
	auto &buff_source = source.get();
	if (buff_source.get_type() != data::type::buff_source_type::skill) THROW_CODE_EXCEPTION(not_implemented_exception, "ice charge data::type::buff_source_type");
	game_skill_id skill_id = buff_source.get_skill_id();
	return
		skill_id == constant::skill::white_knight::bw_ice_charge ||
		skill_id == constant::skill::white_knight::sword_ice_charge;
}

auto player_active_buffs::get_pickpocket_counter() -> int32_t {
	return ++m_pickpocket_counter;
}

auto player_active_buffs::has_infinity() const -> bool {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return has_buff(basics.infinity);
}

auto player_active_buffs::is_using_gm_hide() const -> bool {
	return has_buff(data::type::buff_source_type::skill, constant::skill::super_gm::hide);
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
	return (m_debuff_mask & constant::status_effect::player::curse) > 0;
}

auto player_active_buffs::get_holy_symbol_rate() const -> int16_t {
	int16_t val = 0;
	auto source = get_holy_symbol_source();
	if (source.is_initialized()) {
		auto &buff_source = source.get();
		if (buff_source.get_type() != data::type::buff_source_type::skill) THROW_CODE_EXCEPTION(not_implemented_exception, "holy symbol data::type::buff_source_type");
		val = get_buff_skill_info(buff_source)->x;
	}
	return val;
}

auto player_active_buffs::get_magic_guard_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.magic_guard);
}

auto player_active_buffs::get_meso_guard_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.meso_guard);
}

auto player_active_buffs::get_meso_up_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.meso_up);
}

auto player_active_buffs::get_homing_beacon_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.homing_beacon);
}

auto player_active_buffs::get_combo_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.combo);
}

auto player_active_buffs::get_charge_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.charge);
}

auto player_active_buffs::get_dark_sight_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto dark_sight = get_buff_source(basics.dark_sight);
	if (dark_sight.is_initialized()) return dark_sight;
	return get_buff_source(basics.wind_walk);
}

auto player_active_buffs::get_pickpocket_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.pickpocket);
}

auto player_active_buffs::get_hamstring_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.hamstring);
}

auto player_active_buffs::get_blind_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.blind);
}

auto player_active_buffs::get_concentrate_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.concentrate);
}

auto player_active_buffs::get_holy_symbol_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.holy_symbol);
}

auto player_active_buffs::get_power_stance_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	auto ret = get_buff_source(basics.power_stance);
	if (ret.is_initialized()) return ret;
	ret = get_buff_source(basics.energy_charge);
	return ret;
}

auto player_active_buffs::get_hyper_body_hp_source() const -> optional<data::type::buff_source> {
	auto &basics = channel_server::get_instance().get_buff_data_provider().get_buffs_by_effect();
	return get_buff_source(basics.hyper_body_hp);
}

auto player_active_buffs::get_hyper_body_mp_source() const -> optional<data::type::buff_source> {
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
		if (buff_source.get_type() == data::type::buff_source_type::item) {
			stop_skill(buff_source);
		}
	}

	auto battleship_level = get_buff_level(data::type::buff_source_type::skill, constant::skill::corsair::battleship);
	if (battleship_level > 0) {
		m_battleship_hp -= damage / 10;
		auto source = data::type::buff_source::from_skill(constant::skill::corsair::battleship, battleship_level);

		if (auto player = m_player.lock()) {
			if (m_battleship_hp <= 0) {
				m_battleship_hp = 0;
				seconds cool_time = get_buff_skill_info(source)->cool_time;
				skills::start_cooldown(player, source.get_skill_id(), cool_time);
				stop_skill(source);
			}
			else {
				packets::add_buff(
					player->get_id(),
					source.get_skill_id(),
					seconds{0},
					buffs::convert_to_packet(
						player,
						source,
						seconds{0},
						buffs::preprocess_buff(player, source, seconds{0})),
					0);
			}
		}
		else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
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

		builder.add<data::type::buff_source_type>(buff.type);
		builder.add<int32_t>(buff.identifier);
		switch (buff.type) {
			case data::type::buff_source_type::item:
			case data::type::buff_source_type::skill:
				builder.add<game_skill_level>(buff.level);
				break;
			case data::type::buff_source_type::mob_skill:
				builder.add<game_mob_skill_level>(static_cast<game_mob_skill_level>(buff.level));
				break;
			default: THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
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

	if (auto player = m_player.lock()) {
		// Current player skill/item buff info
		size_t size = reader.get<uint16_t>();
		for (size_t i = 0; i < size; ++i) {
			data::type::buff_source_type type = reader.get<data::type::buff_source_type>();
			int32_t identifier = reader.get<int32_t>();
			int32_t level = 0;
			switch (type) {
				case data::type::buff_source_type::item:
				case data::type::buff_source_type::skill: {
					level = reader.get<game_skill_level>();
					break;
				}
				case data::type::buff_source_type::mob_skill: {
					level = reader.get<game_mob_skill_level>();
					break;
				}
				default: THROW_CODE_EXCEPTION(not_implemented_exception, "data::type::buff_source_type");
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

			data::type::buff_source source = buff.to_source();
			int32_t packet_skill_id = translate_to_packet(source);
			buff.raw = buffs::preprocess_buff(
				buffs::preprocess_buff(
					player,
					source,
					time_left),
				valid_bits);

			m_buffs.push_back(buff);

			vana::timer::id id{vana::timer::type::buff_timer, static_cast<int32_t>(buff.type), buff.identifier};
			vana::timer::timer::create(
				[this, player, packet_skill_id](const time_point &now) {
					skills::stop_skill(player, translate_to_source(packet_skill_id), true);
				},
				id,
				player->get_timer_container(),
				time_left);
		}

		if (m_energy_charge > 0 && m_energy_charge != constant::stat::max_energy_charge_level) {
			start_energy_charge_timer();
		}

		auto hyper_body_hp_source = get_hyper_body_hp_source();
		if (hyper_body_hp_source.is_initialized()) {
			auto skill = get_buff_skill_info(hyper_body_hp_source.get());
			player->get_stats()->set_hyper_body_hp(skill->x);
		}
		auto hyper_body_mp_source = get_hyper_body_mp_source();
		if (hyper_body_mp_source.is_initialized()) {
			auto skill = get_buff_skill_info(hyper_body_mp_source.get());
			player->get_stats()->set_hyper_body_mp(skill->y);
		}
	}
	else THROW_CODE_EXCEPTION(invalid_operation_exception, "This should never be thrown");
}

}
}