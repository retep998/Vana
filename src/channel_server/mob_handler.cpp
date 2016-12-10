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
#include "common/data/provider/skill.hpp"
#include "common/packet_reader.hpp"
#include "common/point.hpp"
#include "common/timer/timer.hpp"
#include "common/util/game_logic/item.hpp"
#include "common/util/misc.hpp"
#include "common/util/randomizer.hpp"
#include "common/util/time.hpp"
#include "channel_server/channel_server.hpp"
#include "channel_server/instance.hpp"
#include "channel_server/maps.hpp"
#include "channel_server/mob.hpp"
#include "channel_server/mobs_packet.hpp"
#include "channel_server/move_path.hpp"
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
		game_damage damage = dealer->get_level() * vana::util::randomizer::rand<int32_t>(100) / 10;
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

	move_path path(reader);
	mob->reset_from_move_path(path);
	
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
	
	player->send_map(packets::mobs::move_mob(mob_id, next_movement_could_be_skill, raw_activity, use_skill_id, use_skill_level, option, path), true);
}

auto mob_handler::handle_mob_status(game_player_id player_id, ref_ptr<mob> mob, game_skill_id skill_id, game_skill_level level, game_item_id weapon, int8_t hits, game_damage damage) -> int32_t {
	auto player = channel_server::get_instance().get_player_data_provider().get_player(player_id);
	vector<status_info> statuses;
	int16_t y = 0;
	auto skill = channel_server::get_instance().get_skill_data_provider().get_skill(skill_id, level);
	bool success = (skill_id == 0 ? false : (vana::util::randomizer::percentage<uint16_t>() < skill->prop));
	if (mob->can_freeze()) {
		// Freezing stuff
		switch (skill_id) {
			case constant::skill::il_wizard::cold_beam:
			case constant::skill::il_mage::ice_strike:
			case constant::skill::il_mage::element_composition:
			case constant::skill::sniper::blizzard:
			case constant::skill::il_arch_mage::blizzard:
				statuses.emplace_back(constant::status_effect::mob::freeze, constant::status_effect::mob::freeze, skill_id, skill->buff_time);
				break;
			case constant::skill::outlaw::ice_splitter:
				if (auto elemental_boost = player->get_skills()->get_skill_info(constant::skill::corsair::elemental_boost)) {
					y = elemental_boost->y;
				}
				statuses.emplace_back(constant::status_effect::mob::freeze, constant::status_effect::mob::freeze, skill_id, seconds{skill->buff_time.count() + y});
				break;
			case constant::skill::fp_arch_mage::elquines:
			case constant::skill::marksman::frostprey:
				statuses.emplace_back(constant::status_effect::mob::freeze, constant::status_effect::mob::freeze, skill_id, seconds{skill->x});
				break;
		}
		if ((vana::util::game_logic::item::is_sword(weapon) || vana::util::game_logic::item::is_mace(weapon)) && player->get_active_buffs()->has_ice_charge()) {
			// Ice charges
			auto source = player->get_active_buffs()->get_charge_source();
			auto &buff_source = source.get();
			if (buff_source.get_type() != data::type::buff_source_type::skill) THROW_CODE_EXCEPTION(not_implemented_exception, "charge buff_source_type");
			auto skill = player->get_active_buffs()->get_buff_skill_info(buff_source);
			statuses.emplace_back(constant::status_effect::mob::freeze, constant::status_effect::mob::freeze, buff_source.get_skill_id(), seconds{skill->y});
		}
	}
	if (mob->can_poison() && mob->get_hp() > 1) {
		// Poisoning stuff
		switch (skill_id) {
			case constant::skill::all::regular_attack: // Venomous Star/Stab
			case constant::skill::rogue::lucky_seven:
			case constant::skill::hermit::avenger:
			case constant::skill::night_lord::triple_throw:
			case constant::skill::rogue::double_stab:
			case constant::skill::rogue::disorder:
			case constant::skill::bandit::savage_blow:
			case constant::skill::chief_bandit::assaulter:
			case constant::skill::shadower::assassinate:
			case constant::skill::shadower::boomerang_step:
			case constant::skill::night_walker::disorder:
				if (player->get_skills()->has_venomous_weapon() && mob->get_venom_count() < constant::status_effect::mob::max_venom_count) {
					// MAX = (18.5 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					// MIN = (8.0 * [STR + LUK] + DEX * 2) / 100 * Venom matk
					game_skill_id v_skill = player->get_skills()->get_venomous_weapon();
					auto venom = player->get_skills()->get_skill_info(v_skill);

					int32_t part1 = player->get_stats()->get_str(true) + player->get_stats()->get_luk(true);
					int32_t part2 = player->get_stats()->get_dex(true) * 2;
					int16_t v_atk = venom->m_atk;
					game_damage min_damage = ((80 * part1 / 10 + part2) / 100) * v_atk;
					game_damage max_damage = ((185 * part1 / 10 + part2) / 100) * v_atk;

					damage = vana::util::randomizer::rand<game_damage>(max_damage, min_damage);

					for (int8_t counter = 0; ((counter < hits) && (mob->get_venom_count() < constant::status_effect::mob::max_venom_count)); ++counter) {
						success = (vana::util::randomizer::percentage<uint16_t>() < venom->prop);
						if (success) {
							statuses.emplace_back(constant::status_effect::mob::venomous_weapon, damage, v_skill, venom->buff_time);
							mob->add_status(player->get_id(), statuses);
							statuses.clear();
						}
					}
				}
				break;
			case constant::skill::fp_mage::poison_mist:
				if (damage != 0) {
					// The attack itself doesn't poison them
					break;
				}
			case constant::skill::fp_wizard::poison_breath:
			case constant::skill::fp_mage::element_composition:
			case constant::skill::blaze_wizard::flame_gear:
			case constant::skill::night_walker::poison_bomb:
				if (success) {
					statuses.emplace_back(constant::status_effect::mob::poison, mob->get_max_hp() / (70 - level), skill_id, skill->buff_time);
				}
				break;
		}
	}
	if (!mob->is_boss()) {
		// Seal, Stun, etc
		switch (skill_id) {
			case constant::skill::corsair::hypnotize:
				statuses.emplace_back(constant::status_effect::mob::hypnotize, 1, skill_id, skill->buff_time);
				break;
			case constant::skill::brawler::backspin_blow:
			case constant::skill::brawler::double_uppercut:
			case constant::skill::buccaneer::demolition:
			case constant::skill::buccaneer::snatch:
				statuses.emplace_back(constant::status_effect::mob::stun, constant::status_effect::mob::stun, skill_id, skill->buff_time);
				break;
			case constant::skill::hunter::arrow_bomb:
			case constant::skill::crusader::sword_coma:
			case constant::skill::dawn_warrior::coma:
			case constant::skill::crusader::axe_coma:
			case constant::skill::crusader::shout:
			case constant::skill::white_knight::charge_blow:
			case constant::skill::chief_bandit::assaulter:
			case constant::skill::shadower::boomerang_step:
			case constant::skill::gunslinger::blank_shot:
			case constant::skill::night_lord::ninja_storm:
				if (success) {
					statuses.emplace_back(constant::status_effect::mob::stun, constant::status_effect::mob::stun, skill_id, skill->buff_time);
				}
				break;
			case constant::skill::ranger::silver_hawk:
			case constant::skill::sniper::golden_eagle:
				if (success) {
					statuses.emplace_back(constant::status_effect::mob::stun, constant::status_effect::mob::stun, skill_id, seconds{skill->x});
				}
				break;
			case constant::skill::fp_mage::seal:
			case constant::skill::il_mage::seal:
			case constant::skill::blaze_wizard::seal:
				if (success) {
					statuses.emplace_back(constant::status_effect::mob::seal, constant::status_effect::mob::seal, skill_id, skill->buff_time);
				}
				break;
			case constant::skill::priest::doom:
				if (success) {
					statuses.emplace_back(constant::status_effect::mob::doom, constant::status_effect::mob::doom, skill_id, skill->buff_time);
				}
				break;
			case constant::skill::hermit::shadow_web:
			case constant::skill::night_walker::shadow_web:
				if (success) {
					statuses.emplace_back(constant::status_effect::mob::shadow_web, level, skill_id, skill->buff_time);
				}
				break;
			case constant::skill::fp_arch_mage::paralyze:
				if (mob->can_poison()) {
					statuses.emplace_back(constant::status_effect::mob::freeze, constant::status_effect::mob::freeze, skill_id, skill->buff_time);
				}
				break;
			case constant::skill::il_arch_mage::ice_demon:
			case constant::skill::fp_arch_mage::fire_demon:
				statuses.emplace_back(constant::status_effect::mob::poison, mob->get_max_hp() / (70 - level), skill_id, skill->buff_time);
				statuses.emplace_back(constant::status_effect::mob::freeze, constant::status_effect::mob::freeze, skill_id, seconds{skill->x});
				break;
			case constant::skill::shadower::taunt:
			case constant::skill::night_lord::taunt:
				// I know, these status effect types make no sense, that's just how it works
				statuses.emplace_back(constant::status_effect::mob::magic_attack_up, 100 - skill->x, skill_id, skill->buff_time);
				statuses.emplace_back(constant::status_effect::mob::magic_defense_up, 100 - skill->x, skill_id, skill->buff_time);
				break;
			case constant::skill::outlaw::flamethrower:
				if (auto elemental_boost = player->get_skills()->get_skill_info(constant::skill::corsair::elemental_boost)) {
					y = elemental_boost->x;
				}
				statuses.emplace_back(constant::status_effect::mob::poison, damage * (5 + y) / 100, skill_id, skill->buff_time);
				break;
		}
	}
	switch (skill_id) {
		case constant::skill::shadower::ninja_ambush:
		case constant::skill::night_lord::ninja_ambush:
			damage = 2 * (player->get_stats()->get_str(true) + player->get_stats()->get_luk(true)) * skill->damage / 100;
			statuses.emplace_back(constant::status_effect::mob::ninja_ambush, damage, skill_id, skill->buff_time);
			break;
		case constant::skill::rogue::disorder:
		case constant::skill::night_walker::disorder:
		case constant::skill::page::threaten:
			statuses.emplace_back(constant::status_effect::mob::watk, skill->x, skill_id, skill->buff_time);
			statuses.emplace_back(constant::status_effect::mob::wdef, skill->y, skill_id, skill->buff_time);
			break;
		case constant::skill::fp_wizard::slow:
		case constant::skill::il_wizard::slow:
		case constant::skill::blaze_wizard::slow:
			statuses.emplace_back(constant::status_effect::mob::speed, skill->x, skill_id, skill->buff_time);
			break;
	}
	if (vana::util::game_logic::item::is_bow(weapon)) {
		auto hamstring = player->get_active_buffs()->get_hamstring_source();
		if (hamstring.is_initialized()) {
			auto info = player->get_active_buffs()->get_buff_skill_info(hamstring.get());
			// Only triggers if player has the buff
			if (skill_id != constant::skill::bowmaster::phoenix && skill_id != constant::skill::ranger::silver_hawk) {
				statuses.emplace_back(constant::status_effect::mob::speed, info->x, constant::skill::bowmaster::hamstring, seconds{info->y});
			}
		}
	}
	else if (vana::util::game_logic::item::is_crossbow(weapon)) {
		auto blind = player->get_active_buffs()->get_blind_source();
		if (blind.is_initialized()) {
			auto info = player->get_active_buffs()->get_buff_skill_info(blind.get());
			// Only triggers if player has the buff
			if (skill_id != constant::skill::marksman::frostprey && skill_id != constant::skill::sniper::golden_eagle) {
				statuses.emplace_back(constant::status_effect::mob::acc, -(info->x), constant::skill::marksman::blind, seconds{info->y});
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