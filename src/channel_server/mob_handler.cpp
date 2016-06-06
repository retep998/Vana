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
#include "mob_handler.hpp"
#include "common/algorithm.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/misc_utilities.hpp"
#include "common/mob_constants.hpp"
#include "common/packet_reader.hpp"
#include "common/point.hpp"
#include "common/randomizer.hpp"
#include "common/skill_data_provider.hpp"
#include "common/time_utilities.hpp"
#include "common/timer.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mob.hpp"
#include "channel_server/mobs_packet.hpp"
#include "channel_server/movement_handler.hpp"
#include "channel_server/player.hpp"
#include "channel_server/player_data_provider.hpp"
#include "channel_server/player_packet.hpp"
#include "channel_server/status_info.hpp"
#include <functional>

namespace vana {
namespace channel_server {

auto mob_handler::handle_bomb(ref_ptr<player> player, packet_reader &reader) -> void {
	game_map_object mob_id = reader.get<game_map_object>();
	auto mob = player->get_map()->get_mob(mob_id);
	if (player->get_stats()->is_dead() || mob == nullptr) {
		return;
	}
	if (mob->get_self_destruct_hp() == 0) {
		// Hacking, I think
		return;
	}
	mob->explode();
}

auto mob_handler::friendly_damaged(ref_ptr<player> player, packet_reader &reader) -> void {
	game_map_object mob_from = reader.get<game_map_object>();
	game_player_id player_id = reader.get<game_player_id>();
	game_map_object mob_to = reader.get<game_map_object>();

	map *map = player->get_map();
	auto dealer = map->get_mob(mob_from);
	auto taker = map->get_mob(mob_to);
	if (dealer != nullptr && taker != nullptr && taker->is_friendly()) {
		game_damage damage = dealer->get_level() * randomizer::rand<int32_t>(100) / 10;
		// Temp for now until I figure out something more effective
		// TODO FIXME: Formula
		game_mob_id mob_id = taker->get_mob_id();
		game_map_object map_mob_id = taker->get_map_mob_id();
		int32_t mob_hp = std::max(0, taker->get_hp() - damage);
		int32_t max_hp = taker->get_max_hp();

		taker->apply_damage(player_id, damage);
		if (instance *inst = map->get_instance()) {
			inst->friendly_mob_hit(mob_id, map_mob_id, map->get_id(), mob_hp, max_hp);
		}
	}
}

auto mob_handler::handle_turncoats(ref_ptr<player> player, packet_reader &reader) -> void {
	game_map_object mob_from = reader.get<game_map_object>();
	game_player_id player_id = reader.get<game_player_id>();
	game_map_object mob_to = reader.get<game_map_object>();
	reader.skip<uint8_t>(); // Same as player damage, -1 = bump, integer = skill ID
	game_damage damage = reader.get<game_damage>();
	reader.skip<uint8_t>(); // Facing direction
	reader.unk<uint32_t>(); // Some type of pos, damage display, I think

	map *map = player->get_map();
	auto damager = map->get_mob(mob_from);
	auto taker = map->get_mob(mob_to);
	if (damager != nullptr && taker != nullptr) {
		taker->apply_damage(player_id, damage);
	}
}

auto mob_handler::monster_control(ref_ptr<player> player, packet_reader &reader) -> void {
	game_map_object mob_id = reader.get<game_map_object>();

	map *map = player->get_map();
	auto mob = map->get_mob(mob_id);
	if (mob == nullptr || mob->get_control_status() == mob_control_status::none) {
		return;
	}

	int16_t move_id = reader.get<int16_t>();
	if (mob->get_controller() != player && !mob->get_skill_feasibility()) {
		map->switch_controller(mob, player);
	}

	int8_t nibbles = reader.get<int8_t>();
	int8_t raw_activity = reader.get<int8_t>();
	game_mob_skill_id use_skill_id = reader.get<game_mob_skill_id>();
	game_mob_skill_level use_skill_level = reader.get<game_mob_skill_level>();
	int16_t option = reader.get<int16_t>();
	reader.unk<uint8_t>();
	reader.unk<uint32_t>(); // 4 bytes of always 1 or always 0?
	reader.unk<uint32_t>(); // Pos?

	// TODO FIXME mob.get() - perhaps movement parsing should be on the MovableLife class itself?
	movement_handler::parse_movement(mob.get(), reader);

	int8_t parsed_activity = raw_activity;
	if (parsed_activity >= 0) {
		parsed_activity = static_cast<int8_t>(static_cast<uint8_t>(parsed_activity) >> 1);
	}
	bool is_attack = ext::in_range_inclusive<int8_t>(parsed_activity, 12, 20);
	bool is_skill = ext::in_range_inclusive<int8_t>(parsed_activity, 21, 25);
	game_mob_skill_id attack_id = is_attack ? parsed_activity - 12 : -1;
	bool next_movement_could_be_skill = (nibbles & 0x0F) != 0;
	bool unk = (nibbles & 0xF0) != 0;

	game_mob_skill_id next_cast_skill = 0;
	game_mob_skill_level next_cast_skill_level = 0;

	if (is_attack || is_skill) {
		if (is_attack) {
			auto attack = channel_server::get_instance().get_mob_data_provider().get_mob_attack(mob->get_mob_id_or_link(), attack_id);
			if (attack == nullptr) {
				// Hacking
				return;
			}
			mob->consume_mp(attack->mp_consume);
		}
		else {
			if (use_skill_id != mob->get_anticipated_skill() || use_skill_level != mob->get_anticipated_skill_level()) {
				// Hacking?
				mob->reset_anticipated_skill();
				return;
			}
			if (mob->use_anticipated_skill() == result::failure) {
				return;
			}
		}
	}

	mob->set_skill_feasibility(next_movement_could_be_skill);
	if (next_movement_could_be_skill) {
		mob->choose_random_skill(player, next_cast_skill, next_cast_skill_level);
	}

	player->send(packets::mobs::move_mob_response(mob_id, move_id, next_movement_could_be_skill, mob->get_mp(), next_cast_skill, next_cast_skill_level));
	reader.reset(19);
	player->send_map(packets::mobs::move_mob(mob_id, next_movement_could_be_skill, raw_activity, use_skill_id, use_skill_level, option, reader.get_buffer(), reader.get_buffer_length()), true);
}

auto mob_handler::handle_mob_status(game_player_id player_id, ref_ptr<mob> mob, game_skill_id skill_id, game_skill_level level, game_item_id weapon, int8_t hits, game_damage damage) -> int32_t {
	auto player = channel_server::get_instance().get_player_data_provider().get_player(player_id);
	vector<status_info> statuses;
	int16_t y = 0;
	auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
	bool success = (skill_id == 0 ? false : (randomizer::percentage<uint16_t>() < skill->prop));
	if (mob->can_freeze()) {
		// Freezing stuff
		switch (skill_id) {
			case vana::skills::il_wizard::cold_beam:
			case vana::skills::il_mage::ice_strike:
			case vana::skills::il_mage::element_composition:
			case vana::skills::sniper::blizzard:
			case vana::skills::il_arch_mage::blizzard:
				statuses.emplace_back(status_effects::mob::freeze, status_effects::mob::freeze, skill_id, skill->buff_time);
				break;
			case vana::skills::outlaw::ice_splitter:
				if (auto elemental_boost = player->get_skills()->get_skill_info(vana::skills::corsair::elemental_boost)) {
					y = elemental_boost->y;
				}
				statuses.emplace_back(status_effects::mob::freeze, status_effects::mob::freeze, skill_id, seconds{skill->buff_time.count() + y});
				break;
			case vana::skills::fp_arch_mage::elquines:
			case vana::skills::marksman::frostprey:
				statuses.emplace_back(status_effects::mob::freeze, status_effects::mob::freeze, skill_id, seconds{skill->x});
				break;
		}
		if ((game_logic_utilities::is_sword(weapon) || game_logic_utilities::is_mace(weapon)) && player->get_active_buffs()->has_ice_charge()) {
			// Ice charges
			auto source = player->get_active_buffs()->get_charge_source();
			auto &buff_source = source.get();
			if (buff_source.get_type() != buff_source_type::skill) throw not_implemented_exception{"charge buff_source_type"};
			auto skill = player->get_active_buffs()->get_buff_skill_info(buff_source);
			statuses.emplace_back(status_effects::mob::freeze, status_effects::mob::freeze, buff_source.get_skill_id(), seconds{skill->y});
		}
	}
	if (mob->can_poison() && mob->get_hp() > 1) {
		// Poisoning stuff
		switch (skill_id) {
			case vana::skills::all::regular_attack: // Venomous Star/Stab
			case vana::skills::rogue::lucky_seven:
			case vana::skills::hermit::avenger:
			case vana::skills::night_lord::triple_throw:
			case vana::skills::rogue::double_stab:
			case vana::skills::rogue::disorder:
			case vana::skills::bandit::savage_blow:
			case vana::skills::chief_bandit::assaulter:
			case vana::skills::shadower::assassinate:
			case vana::skills::shadower::boomerang_step:
			case vana::skills::night_walker::disorder:
				if (player->get_skills()->has_venomous_weapon() && mob->get_venom_count() < status_effects::mob::max_venom_count) {
					// MAX = (18.5 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					// MIN = (8.0 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					game_skill_id v_skill = player->get_skills()->get_venomous_weapon();
					auto venom = player->get_skills()->get_skill_info(v_skill);

					int32_t part1 = player->get_stats()->get_str(true) + player->get_stats()->get_luk(true);
					int32_t part2 = player->get_stats()->get_dex(true) * 2;
					int16_t v_atk = venom->m_atk;
					game_damage min_damage = ((80 * part1 / 10 + part2) / 100) * v_atk;
					game_damage max_damage = ((185 * part1 / 10 + part2) / 100) * v_atk;

					damage = randomizer::rand<game_damage>(max_damage, min_damage);

					for (int8_t counter = 0; ((counter < hits) && (mob->get_venom_count() < status_effects::mob::max_venom_count)); ++counter) {
						success = (randomizer::percentage<uint16_t>() < venom->prop);
						if (success) {
							statuses.emplace_back(status_effects::mob::venomous_weapon, damage, v_skill, venom->buff_time);
							mob->add_status(player->get_id(), statuses);
							statuses.clear();
						}
					}
				}
				break;
			case vana::skills::fp_mage::poison_mist:
				if (damage != 0) {
					// The attack itself doesn't poison them
					break;
				}
			case vana::skills::fp_wizard::poison_breath:
			case vana::skills::fp_mage::element_composition:
			case vana::skills::blaze_wizard::flame_gear:
			case vana::skills::night_walker::poison_bomb:
				if (success) {
					statuses.emplace_back(status_effects::mob::poison, mob->get_max_hp() / (70 - level), skill_id, skill->buff_time);
				}
				break;
		}
	}
	if (!mob->is_boss()) {
		// Seal, Stun, etc
		switch (skill_id) {
			case vana::skills::corsair::hypnotize:
				statuses.emplace_back(status_effects::mob::hypnotize, 1, skill_id, skill->buff_time);
				break;
			case vana::skills::brawler::backspin_blow:
			case vana::skills::brawler::double_uppercut:
			case vana::skills::buccaneer::demolition:
			case vana::skills::buccaneer::snatch:
				statuses.emplace_back(status_effects::mob::stun, status_effects::mob::stun, skill_id, skill->buff_time);
				break;
			case vana::skills::hunter::arrow_bomb:
			case vana::skills::crusader::sword_coma:
			case vana::skills::dawn_warrior::coma:
			case vana::skills::crusader::axe_coma:
			case vana::skills::crusader::shout:
			case vana::skills::white_knight::charge_blow:
			case vana::skills::chief_bandit::assaulter:
			case vana::skills::shadower::boomerang_step:
			case vana::skills::gunslinger::blank_shot:
			case vana::skills::night_lord::ninja_storm:
				if (success) {
					statuses.emplace_back(status_effects::mob::stun, status_effects::mob::stun, skill_id, skill->buff_time);
				}
				break;
			case vana::skills::ranger::silver_hawk:
			case vana::skills::sniper::golden_eagle:
				if (success) {
					statuses.emplace_back(status_effects::mob::stun, status_effects::mob::stun, skill_id, seconds{skill->x});
				}
				break;
			case vana::skills::fp_mage::seal:
			case vana::skills::il_mage::seal:
			case vana::skills::blaze_wizard::seal:
				if (success) {
					statuses.emplace_back(status_effects::mob::seal, status_effects::mob::seal, skill_id, skill->buff_time);
				}
				break;
			case vana::skills::priest::doom:
				if (success) {
					statuses.emplace_back(status_effects::mob::doom, status_effects::mob::doom, skill_id, skill->buff_time);
				}
				break;
			case vana::skills::hermit::shadow_web:
			case vana::skills::night_walker::shadow_web:
				if (success) {
					statuses.emplace_back(status_effects::mob::shadow_web, level, skill_id, skill->buff_time);
				}
				break;
			case vana::skills::fp_arch_mage::paralyze:
				if (mob->can_poison()) {
					statuses.emplace_back(status_effects::mob::freeze, status_effects::mob::freeze, skill_id, skill->buff_time);
				}
				break;
			case vana::skills::il_arch_mage::ice_demon:
			case vana::skills::fp_arch_mage::fire_demon:
				statuses.emplace_back(status_effects::mob::poison, mob->get_max_hp() / (70 - level), skill_id, skill->buff_time);
				statuses.emplace_back(status_effects::mob::freeze, status_effects::mob::freeze, skill_id, seconds{skill->x});
				break;
			case vana::skills::shadower::taunt:
			case vana::skills::night_lord::taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.emplace_back(status_effects::mob::magic_attack_up, 100 - skill->x, skill_id, skill->buff_time);
				statuses.emplace_back(status_effects::mob::magic_defense_up, 100 - skill->x, skill_id, skill->buff_time);
				break;
			case vana::skills::outlaw::flamethrower:
				if (auto elemental_boost = player->get_skills()->get_skill_info(vana::skills::corsair::elemental_boost)) {
					y = elemental_boost->x;
				}
				statuses.emplace_back(status_effects::mob::poison, damage * (5 + y) / 100, skill_id, skill->buff_time);
				break;
		}
	}
	switch (skill_id) {
		case vana::skills::shadower::ninja_ambush:
		case vana::skills::night_lord::ninja_ambush:
			damage = 2 * (player->get_stats()->get_str(true) + player->get_stats()->get_luk(true)) * skill->damage / 100;
			statuses.emplace_back(status_effects::mob::ninja_ambush, damage, skill_id, skill->buff_time);
			break;
		case vana::skills::rogue::disorder:
		case vana::skills::night_walker::disorder:
		case vana::skills::page::threaten:
			statuses.emplace_back(status_effects::mob::watk, skill->x, skill_id, skill->buff_time);
			statuses.emplace_back(status_effects::mob::wdef, skill->y, skill_id, skill->buff_time);
			break;
		case vana::skills::fp_wizard::slow:
		case vana::skills::il_wizard::slow:
		case vana::skills::blaze_wizard::slow:
			statuses.emplace_back(status_effects::mob::speed, skill->x, skill_id, skill->buff_time);
			break;
	}
	if (game_logic_utilities::is_bow(weapon)) {
		auto hamstring = player->get_active_buffs()->get_hamstring_source();
		if (hamstring.is_initialized()) {
			auto info = player->get_active_buffs()->get_buff_skill_info(hamstring.get());
			// Only triggers if player has the buff
			if (skill_id != vana::skills::bowmaster::phoenix && skill_id != vana::skills::ranger::silver_hawk) {
				statuses.emplace_back(status_effects::mob::speed, info->x, vana::skills::bowmaster::hamstring, seconds{info->y});
			}
		}
	}
	else if (game_logic_utilities::is_crossbow(weapon)) {
		auto blind = player->get_active_buffs()->get_blind_source();
		if (blind.is_initialized()) {
			auto info = player->get_active_buffs()->get_buff_skill_info(blind.get());
			// Only triggers if player has the buff
			if (skill_id != vana::skills::marksman::frostprey && skill_id != vana::skills::sniper::golden_eagle) {
				statuses.emplace_back(status_effects::mob::acc, -(info->x), vana::skills::marksman::blind, seconds{info->y});
			}
		}
	}

	if (statuses.size() > 0) {
		mob->add_status(player_id, statuses);
	}
	return statuses.size();
}

}
}