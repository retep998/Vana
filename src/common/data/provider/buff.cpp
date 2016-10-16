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
#include "buff.hpp"
#include "common/algorithm.hpp"
#include "common/constant/mob_skill.hpp"
#include "common/constant/skill.hpp"
#include "common/data/initialize.hpp"
#include "common/data/provider/item.hpp"
#include "common/data/type/buff_source.hpp"
#include <iomanip>
#include <iostream>

namespace vana {
namespace data {
namespace provider {

auto buff::process_skills(data::type::buff value, const init_list<game_skill_id> &skills) -> void {
	for (const auto &skill_id : skills) {
		for (const auto &existing : m_buffs) {
			if (existing.first == skill_id) throw std::invalid_argument{"skill is already present"};
		}

		m_buffs.emplace_back(skill_id, value);
	}
}

auto buff::load_data() -> void {
	std::cout << std::setw(vana::data::initialize::output_width) << std::left << "Initializing Buffs... ";

	auto physical_attack = data::type::buff_info::from_player_only(1, data::type::buff_skill_value::watk);
	auto physical_defense = data::type::buff_info::from_player_only(2, data::type::buff_skill_value::wdef);
	auto magic_attack = data::type::buff_info::from_player_only(3, data::type::buff_skill_value::matk);
	auto magic_defense = data::type::buff_info::from_player_only(4, data::type::buff_skill_value::mdef);
	auto accuracy = data::type::buff_info::from_player_only(5, data::type::buff_skill_value::accuracy);
	auto avoid = data::type::buff_info::from_player_only(6, data::type::buff_skill_value::avoid);
	auto craft = data::type::buff_info::from_player_only(7, data::type::buff_skill_value::predefined);
	auto speed = data::type::buff_info::from_map_movement(8, data::type::buff_skill_value::speed, data::type::buff_map_info{1, data::type::buff_skill_value::speed});
	auto jump = data::type::buff_info::from_player_only_movement(9, data::type::buff_skill_value::jump);
	auto magic_guard = data::type::buff_info::from_player_only(10, data::type::buff_skill_value::x);
	auto dark_sight = data::type::buff_info::from_map_no_movement(11, data::type::buff_skill_value::x, data::type::buff_map_info{});
	auto booster = data::type::buff_info::from_player_only(12, data::type::buff_skill_value::x);
	auto power_guard = data::type::buff_info::from_player_only(13, data::type::buff_skill_value::x);
	auto hyper_body_hp = data::type::buff_info::from_player_only(14, data::type::buff_skill_value::x);
	auto hyper_body_mp = data::type::buff_info::from_player_only(15, data::type::buff_skill_value::y);
	auto invincible = data::type::buff_info::from_player_only(16, data::type::buff_skill_value::x);
	auto soul_arrow = data::type::buff_info::from_map_no_movement(17, data::type::buff_skill_value::x, data::type::buff_map_info{});
	auto stun = data::type::buff_info::from_map_movement(18, 1, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto poison = data::type::buff_info::from_map_no_movement(19, data::type::buff_skill_value::x, data::type::buff_map_info{data::type::buff_skill_value::special_processing});
	auto seal = data::type::buff_info::from_map_no_movement(20, 2, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto darkness = data::type::buff_info::from_map_no_movement(21, 1, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto combo = data::type::buff_info::from_map_no_movement(22, data::type::buff_skill_value::special_processing, data::type::buff_map_info{1, data::type::buff_skill_value::special_processing});
	auto charge = data::type::buff_info::from_map_no_movement(23, data::type::buff_skill_value::matk, data::type::buff_map_info{4, data::type::buff_skill_value::skill_id});
	auto timed_hurt = data::type::buff_info::from_player_only(24, data::type::buff_skill_value::level);
	auto holy_symbol = data::type::buff_info::from_player_only(25, data::type::buff_skill_value::x);
	auto meso_up = data::type::buff_info::from_player_only(26, data::type::buff_skill_value::x);
	auto shadow_partner = data::type::buff_info::from_map_no_movement(27, data::type::buff_skill_value::bitpacked_xy_16, data::type::buff_map_info{});
	auto pickpocket = data::type::buff_info::from_player_only(28, data::type::buff_skill_value::x);
	auto meso_guard = data::type::buff_info::from_player_only(29, data::type::buff_skill_value::x);
	// 30 - ???
	auto weakness = data::type::buff_info::from_map_movement(31, 1, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto curse = data::type::buff_info::from_map_no_movement(32, 1, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto slow = data::type::buff_info::from_player_only_movement(33, data::type::buff_skill_value::x);
	auto morph = data::type::buff_info::from_map_movement(34, data::type::buff_skill_value::morph, data::type::buff_map_info{2, data::type::buff_skill_value::morph});
	auto timed_heal = data::type::buff_info::from_player_only(35, data::type::buff_skill_value::x);
	// No, I have no idea why Maple Warrior is a "movement" buff
	auto maple_warrior = data::type::buff_info::from_player_only_movement(36, data::type::buff_skill_value::x);
	auto power_stance = data::type::buff_info::from_player_only(37, data::type::buff_skill_value::prop);
	auto sharp_eyes = data::type::buff_info::from_player_only(38, data::type::buff_skill_value::bitpacked_xy_16);
	auto mana_reflection = data::type::buff_info::from_player_only(39, data::type::buff_skill_value::level);
	auto seduce = data::type::buff_info::from_map_movement(40, data::type::buff_skill_value::x, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto shadow_stars = data::type::buff_info::from_map_no_movement(41, data::type::buff_skill_value::special_processing, data::type::buff_map_info{4, data::type::buff_skill_value::special_processing});
	auto infinity = data::type::buff_info::from_player_only(42, data::type::buff_skill_value::x);
	auto holy_shield = data::type::buff_info::from_player_only(43, data::type::buff_skill_value::x);
	auto hamstring = data::type::buff_info::from_player_only(44, data::type::buff_skill_value::x);
	auto blind = data::type::buff_info::from_player_only(45, data::type::buff_skill_value::x);
	auto concentrate = data::type::buff_info::from_player_only(46, data::type::buff_skill_value::x);
	auto zombify = data::type::buff_info::from_map_no_movement(47, data::type::buff_skill_value::x, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	auto echo = data::type::buff_info::from_player_only(48, data::type::buff_skill_value::x);
	// 49 - ???
	// 50 - ???
	// 51 - ???
	auto crazy_skull = data::type::buff_info::from_map_no_movement(52, data::type::buff_skill_value::x, data::type::buff_map_info{4, data::type::buff_skill_value::bitpacked_skill_and_level_32});
	// 53 - ???
	auto ignore_weapon_immunity = data::type::buff_info::from_player_only(54, data::type::buff_skill_value::predefined); // Value is a percentage, so 100 means always ignore
	auto ignore_magic_immunity = data::type::buff_info::from_player_only(55, data::type::buff_skill_value::predefined); // Value is a percentage, so 100 means always ignore
	// 56 - ???
	// ??? - not sure what normally goes here, I use it for GM Hide because it appears to have no ill effects
	auto unk = data::type::buff_info::from_player_only(57, data::type::buff_skill_value::special_processing);
	// 58 - ???
	// 59 - ???
	// 60 - ???
	// 61 - ???
	auto spark = data::type::buff_info::from_player_only(62, data::type::buff_skill_value::x);
	// 63 - ???
	auto dawn_warrior_final_attack = data::type::buff_info::from_player_only(64, 1);
	auto wind_walker_final_attack = data::type::buff_info::from_player_only(65, 1);
	auto elemental_reset = data::type::buff_info::from_player_only(66, data::type::buff_skill_value::x);
	auto wind_walk = data::type::buff_info::from_map_no_movement(67, 1, data::type::buff_map_info{});
	auto energy_charge = data::type::buff_info::from_map_no_movement(68, data::type::buff_skill_value::special_packet, data::type::buff_map_info{data::type::buff_skill_value::special_packet});
	auto dash_speed = data::type::buff_info::from_map_movement(69, data::type::buff_skill_value::special_packet, data::type::buff_map_info{data::type::buff_skill_value::special_packet});
	auto dash_jump = data::type::buff_info::from_map_movement(70, data::type::buff_skill_value::special_packet, data::type::buff_map_info{data::type::buff_skill_value::special_packet});
	auto mount = data::type::buff_info::from_map_movement(71, data::type::buff_skill_value::special_packet, data::type::buff_map_info{data::type::buff_skill_value::special_packet});
	auto speed_infusion = data::type::buff_info::from_player_only(72, data::type::buff_skill_value::special_packet);
	auto homing_beacon = data::type::buff_info::from_player_only(73, data::type::buff_skill_value::special_packet);

	process_skills(
		data::type::buff{{
			magic_guard,
		}}, {
			constant::skill::magician::magic_guard,
			constant::skill::blaze_wizard::magic_guard,
		});

	process_skills(
		data::type::buff{{
			speed,
			dark_sight,
		}}, {
			constant::skill::rogue::dark_sight,
			constant::skill::night_walker::dark_sight,
		});

	process_skills(
		data::type::buff{{
			booster,
		}}, {
			constant::skill::fighter::sword_booster,
			constant::skill::fighter::axe_booster,
			constant::skill::page::sword_booster,
			constant::skill::page::bw_booster,
			constant::skill::spearman::spear_booster,
			constant::skill::spearman::polearm_booster,
			constant::skill::fp_mage::spell_booster,
			constant::skill::il_mage::spell_booster,
			constant::skill::hunter::bow_booster,
			constant::skill::crossbowman::crossbow_booster,
			constant::skill::assassin::claw_booster,
			constant::skill::bandit::dagger_booster,
			constant::skill::brawler::knuckler_booster,
			constant::skill::gunslinger::gun_booster,
			constant::skill::dawn_warrior::sword_booster,
			constant::skill::blaze_wizard::spell_booster,
			constant::skill::wind_archer::bow_booster,
			constant::skill::night_walker::claw_booster,
			constant::skill::thunder_breaker::knuckler_booster,
		});

	process_skills(
		data::type::buff{{
			power_guard,
		}}, {
			constant::skill::fighter::power_guard,
			constant::skill::page::power_guard,
		});

	process_skills(
		data::type::buff{{
			hyper_body_hp,
			hyper_body_mp,
		}}, {
			constant::skill::spearman::hyper_body,
			constant::skill::super_gm::hyper_body,
		});

	process_skills(
		data::type::buff{{
			invincible,
		}}, {
			constant::skill::cleric::invincible,
		});

	process_skills(
		data::type::buff{{
			soul_arrow,
		}}, {
			constant::skill::hunter::soul_arrow,
			constant::skill::crossbowman::soul_arrow,
			constant::skill::wind_archer::soul_arrow,
		});

	process_skills(
		data::type::buff{{
			combo,
		}}, {
			constant::skill::crusader::combo_attack,
			constant::skill::dawn_warrior::combo_attack,
		});

	process_skills(
		data::type::buff{{
			magic_attack,
			charge,
		}}, {
			constant::skill::white_knight::bw_fire_charge,
			constant::skill::white_knight::bw_ice_charge,
			constant::skill::white_knight::bw_lit_charge,
			constant::skill::white_knight::sword_fire_charge,
			constant::skill::white_knight::sword_ice_charge,
			constant::skill::white_knight::sword_lit_charge,
			constant::skill::paladin::bw_holy_charge,
			constant::skill::paladin::sword_holy_charge,
			constant::skill::dawn_warrior::soul_charge,
			constant::skill::thunder_breaker::lightning_charge,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
			timed_hurt.
				with_act(data::type::buff_action::hurt, data::type::buff_skill_value::x, seconds{4}),
		}}, {
			constant::skill::dragon_knight::dragon_blood,
		});

	process_skills(
		data::type::buff{{
			holy_symbol,
		}}, {
			constant::skill::priest::holy_symbol,
			constant::skill::super_gm::holy_symbol,
		});

	process_skills(
		data::type::buff{{
			meso_up,
		}}, {
			constant::skill::hermit::meso_up,
		});

	process_skills(
		data::type::buff{{
			shadow_partner,
		}}, {
			constant::skill::hermit::shadow_partner,
			constant::skill::night_walker::shadow_partner,
		});

	process_skills(
		data::type::buff{{
			pickpocket,
		}}, {
			constant::skill::chief_bandit::pickpocket,
		});

	process_skills(
		data::type::buff{{
			meso_guard,
		}}, {
			constant::skill::chief_bandit::meso_guard,
		});

	process_skills(
		data::type::buff{{
			timed_heal.
				with_act(data::type::buff_action::heal, data::type::buff_skill_value::x, milliseconds{4900}),
		}}, {
			constant::skill::beginner::recovery,
			constant::skill::noblesse::recovery,
		});

	process_skills(
		data::type::buff{{
			maple_warrior,
		}}, {
			constant::skill::hero::maple_warrior,
			constant::skill::paladin::maple_warrior,
			constant::skill::dark_knight::maple_warrior,
			constant::skill::fp_arch_mage::maple_warrior,
			constant::skill::il_arch_mage::maple_warrior,
			constant::skill::bishop::maple_warrior,
			constant::skill::bowmaster::maple_warrior,
			constant::skill::marksman::maple_warrior,
			constant::skill::night_lord::maple_warrior,
			constant::skill::shadower::maple_warrior,
			constant::skill::buccaneer::maple_warrior,
			constant::skill::corsair::maple_warrior,
		});

	process_skills(
		data::type::buff{{
			power_stance,
		}}, {
			constant::skill::hero::power_stance,
			constant::skill::paladin::power_stance,
			constant::skill::dark_knight::power_stance,
		});

	process_skills(
		data::type::buff{{
			sharp_eyes,
		}}, {
			constant::skill::bowmaster::sharp_eyes,
			constant::skill::marksman::sharp_eyes,
		});

	process_skills(
		data::type::buff{{
			mana_reflection,
		}}, {
			constant::skill::fp_arch_mage::mana_reflection,
			constant::skill::il_arch_mage::mana_reflection,
			constant::skill::bishop::mana_reflection,
		});

	process_skills(
		data::type::buff{{
			shadow_stars,
		}}, {
			constant::skill::night_lord::shadow_stars,
		});

	process_skills(
		data::type::buff{{
			infinity,
		}}, {
			constant::skill::fp_arch_mage::infinity,
			constant::skill::il_arch_mage::infinity,
			constant::skill::bishop::infinity,
		});

	process_skills(
		data::type::buff{{
			holy_shield,
		}}, {
			constant::skill::bishop::holy_shield,
		});

	process_skills(
		data::type::buff{{
			hamstring,
		}}, {
			constant::skill::bowmaster::hamstring,
		});

	process_skills(
		data::type::buff{{
			blind,
		}}, {
			constant::skill::marksman::blind,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
			concentrate,
		}}, {
			constant::skill::bowmaster::concentrate,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
			echo,
		}}, {
			constant::skill::beginner::echo_of_hero,
			constant::skill::noblesse::echo_of_hero,
		});

	process_skills(
		data::type::buff{{
			unk,
		}}, {
			constant::skill::super_gm::hide,
		});

	process_skills(
		data::type::buff{{
			spark,
		}}, {
			constant::skill::thunder_breaker::spark,
		});

	process_skills(
		data::type::buff{{
			dawn_warrior_final_attack,
		}}, {
			constant::skill::dawn_warrior::final_attack,
		});

	process_skills(
		data::type::buff{{
			wind_walker_final_attack,
		}}, {
			constant::skill::wind_archer::final_attack,
		});

	process_skills(
		data::type::buff{{
			elemental_reset,
		}}, {
			constant::skill::blaze_wizard::elemental_reset,
		});

	process_skills(
		data::type::buff{{
			speed,
			wind_walk,
		}}, {
			constant::skill::wind_archer::wind_walk,
		});

	process_skills(
		data::type::buff{{
			energy_charge,
		}}, {
			constant::skill::marauder::energy_charge,
			constant::skill::thunder_breaker::energy_charge,
		});

	process_skills(
		data::type::buff{{
			dash_speed,
			dash_jump,
		}}, {
			constant::skill::pirate::dash,
			constant::skill::thunder_breaker::dash,
		});

	process_skills(
		data::type::buff{{
			mount,
		}}, {
			constant::skill::beginner::monster_rider,
			constant::skill::noblesse::monster_rider,
		});

	process_skills(
		data::type::buff{{
			speed_infusion,
		}}, {
			constant::skill::buccaneer::speed_infusion,
			constant::skill::thunder_breaker::speed_infusion,
		});

	process_skills(
		data::type::buff{{
			homing_beacon,
		}}, {
			constant::skill::outlaw::homing_beacon,
			constant::skill::corsair::bullseye,
		});

	process_skills(
		data::type::buff{{
			physical_defense,
		}}, {
			constant::skill::magician::magic_armor,
			constant::skill::swordsman::iron_body,
			constant::skill::blaze_wizard::magic_armor,
			constant::skill::dawn_warrior::iron_body,
		});

	process_skills(
		data::type::buff{{
			accuracy,
			avoid,
		}}, {
			constant::skill::archer::focus,
			constant::skill::wind_archer::focus,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
			physical_defense,
		}}, {
			constant::skill::fighter::rage,
			constant::skill::dawn_warrior::rage,
		});

	process_skills(
		data::type::buff{{
			physical_defense,
			magic_defense,
		}}, {
			constant::skill::spearman::iron_will,
		});

	process_skills(
		data::type::buff{{
			magic_attack,
		}}, {
			constant::skill::fp_wizard::meditation,
			constant::skill::il_wizard::meditation,
			constant::skill::blaze_wizard::meditation,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
			physical_defense,
			magic_defense,
			accuracy,
			avoid,
		}}, {
			constant::skill::cleric::bless,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
			physical_defense,
			magic_attack,
			magic_defense,
			accuracy,
			avoid,
		}}, {
			constant::skill::super_gm::bless,
		});

	process_skills(
		data::type::buff{{
			physical_attack,
		}}, {
			constant::skill::hero::enrage,
		});

	process_skills(
		data::type::buff{{
			stun.
				with_predefined_value(1).
				with_map_info({}),
		}}, {
			constant::skill::dragon_knight::dragon_roar,
		});

	process_skills(
		data::type::buff{{
			speed,
		}}, {
			constant::skill::beginner::nimble_feet,
			constant::skill::noblesse::nimble_feet,
		});

	process_skills(
		data::type::buff{{
			speed,
			jump,
		}}, {
			constant::skill::assassin::haste,
			constant::skill::bandit::haste,
			constant::skill::night_walker::haste,
			constant::skill::gm::haste,
			constant::skill::super_gm::haste,
		});

	process_skills(
		data::type::buff{{
			morph,
		}}, {
			constant::skill::brawler::oak_barrel,
		});

	process_skills(
		data::type::buff{{
			physical_defense,
			magic_defense,
			speed,
			jump,
			morph.
				with_value(data::type::buff_skill_value::gender_specific_morph).
				with_map_info(data::type::buff_map_info{2, data::type::buff_skill_value::gender_specific_morph}),
		}}, {
			constant::skill::marauder::transformation,
			constant::skill::buccaneer::super_transformation,
			constant::skill::wind_archer::eagle_eye,
			constant::skill::thunder_breaker::transformation,
		});

	process_skills(
		data::type::buff{{
			physical_defense,
			magic_defense,
			mount,
		}}, {
			constant::skill::corsair::battleship,
		});

	m_mob_skill_info.emplace_back(constant::mob_skill::stun, data::type::buff{stun});
	m_mob_skill_info.emplace_back(constant::mob_skill::poison, data::type::buff{poison});
	m_mob_skill_info.emplace_back(constant::mob_skill::seal, data::type::buff{seal});
	m_mob_skill_info.emplace_back(constant::mob_skill::darkness, data::type::buff{darkness});
	m_mob_skill_info.emplace_back(constant::mob_skill::weakness, data::type::buff{weakness});
	m_mob_skill_info.emplace_back(constant::mob_skill::curse, data::type::buff{curse});
	m_mob_skill_info.emplace_back(constant::mob_skill::slow, data::type::buff{slow});
	m_mob_skill_info.emplace_back(constant::mob_skill::seduce, data::type::buff{seduce});
	m_mob_skill_info.emplace_back(constant::mob_skill::crazy_skull, data::type::buff{crazy_skull});
	m_mob_skill_info.emplace_back(constant::mob_skill::zombify, data::type::buff{zombify});

	m_basics.physical_attack = physical_attack;
	m_basics.physical_defense = physical_defense;
	m_basics.magic_attack = magic_attack;
	m_basics.magic_defense = magic_defense;
	m_basics.accuracy = accuracy;
	m_basics.avoid = avoid;
	m_basics.speed = speed;
	m_basics.craft = craft;
	m_basics.jump = jump;
	m_basics.magic_guard = magic_guard;
	m_basics.dark_sight = dark_sight;
	m_basics.booster = booster;
	m_basics.power_guard = power_guard;
	m_basics.hyper_body_hp = hyper_body_hp;
	m_basics.hyper_body_mp = hyper_body_mp;
	m_basics.invincible = invincible;
	m_basics.soul_arrow = soul_arrow;
	m_basics.stun = stun;
	m_basics.poison = poison;
	m_basics.seal = seal;
	m_basics.darkness = darkness;
	m_basics.combo = combo;
	m_basics.charge = charge;
	m_basics.timed_hurt = timed_hurt;
	m_basics.holy_symbol = holy_symbol;
	m_basics.meso_up = meso_up;
	m_basics.shadow_partner = shadow_partner;
	m_basics.pickpocket = pickpocket;
	m_basics.meso_guard = meso_guard;
	m_basics.weakness = weakness;
	m_basics.curse = curse;
	m_basics.slow = slow;
	m_basics.morph = morph;
	m_basics.timed_heal = timed_heal;
	m_basics.maple_warrior = maple_warrior;
	m_basics.power_stance = power_stance;
	m_basics.sharp_eyes = sharp_eyes;
	m_basics.mana_reflection = mana_reflection;
	m_basics.seduce = seduce;
	m_basics.shadow_stars = shadow_stars;
	m_basics.infinity = infinity;
	m_basics.holy_shield = holy_shield;
	m_basics.hamstring = hamstring;
	m_basics.blind = blind;
	m_basics.concentrate = concentrate;
	m_basics.zombify = zombify;
	m_basics.echo = echo;
	m_basics.crazy_skull = crazy_skull;
	m_basics.ignore_weapon_immunity = ignore_weapon_immunity;
	m_basics.ignore_magic_immunity = ignore_magic_immunity;
	m_basics.spark = spark;
	m_basics.dawn_warrior_final_attack = dawn_warrior_final_attack;
	m_basics.wind_walker_final_attack = wind_walker_final_attack;
	m_basics.elemental_reset = elemental_reset;
	m_basics.wind_walk = wind_walk;
	m_basics.energy_charge = energy_charge;
	m_basics.dash_speed = dash_speed;
	m_basics.dash_jump = dash_jump;
	m_basics.mount = mount;
	m_basics.speed_infusion = speed_infusion;
	m_basics.homing_beacon = homing_beacon;

	std::cout << "DONE" << std::endl;
}

auto buff::add_item_info(game_item_id item_id, const data::type::consume_info &cons) -> void {
	vector<data::type::buff_info> values;

	if (cons.watk > 0) {
		values.push_back(m_basics.physical_attack.with_predefined_value(cons.watk));
	}
	if (cons.wdef > 0) {
		values.push_back(m_basics.physical_defense.with_predefined_value(cons.wdef));
	}
	if (cons.matk > 0) {
		values.push_back(m_basics.magic_attack.with_predefined_value(cons.matk));
	}
	if (cons.mdef > 0) {
		values.push_back(m_basics.magic_defense.with_predefined_value(cons.mdef));
	}
	if (cons.acc > 0) {
		values.push_back(m_basics.accuracy.with_predefined_value(cons.acc));
	}
	if (cons.avo > 0) {
		values.push_back(m_basics.avoid.with_predefined_value(cons.avo));
	}
	if (cons.speed > 0) {
		values.push_back(m_basics.speed.
			with_predefined_value(cons.speed).
			with_map_info(data::type::buff_map_info{1, cons.speed}));
	}
	if (cons.jump > 0) {
		values.push_back(m_basics.jump.with_predefined_value(cons.jump));
	}
	if (cons.morphs.size() > 0) {
		for (const auto &m : cons.morphs) {
			values.push_back(m_basics.morph.
				with_predefined_value(m.morph).
				with_chance(m.chance).
				with_map_info(data::type::buff_map_info{2, m.morph}));
		}
	}

	// TODO FIXME buffs
	// Need some buff bytes/types for ALL of the following
	if (cons.prevent_drown) {

	}
	if (cons.prevent_freeze) {

	}
	if (cons.ice_resist > 0) {

	}
	if (cons.fire_resist > 0) {

	}
	if (cons.poison_resist > 0) {

	}
	if (cons.lightning_resist > 0) {

	}
	if (cons.curse_def > 0) {

	}
	if (cons.stun_def > 0) {

	}
	if (cons.weakness_def > 0) {

	}
	if (cons.darkness_def > 0) {

	}
	if (cons.seal_def > 0) {

	}
	if (cons.ignore_wdef) {
		values.push_back(m_basics.ignore_weapon_immunity.with_predefined_value(cons.chance));
	}
	if (cons.ignore_mdef) {
		values.push_back(m_basics.ignore_magic_immunity.with_predefined_value(cons.chance));
	}
	if (cons.meso_up) {

	}
	if (cons.party_drop_up) {

	}
	if (cons.drop_up) {
		switch (cons.drop_up) {
			case 1: // Regular drop rate increase for all items, the only one I can parse at the moment
				break;
			//case 2: // Specific item drop rate increase
			//case 3: // Specific item range (itemId / 10000) increase
		}
	}

	if (values.size() > 0) {
		m_items.emplace_back(item_id, data::type::buff{values});
	}
}

auto buff::is_buff(const data::type::buff_source &source) const -> bool {
	switch (source.get_type()) {
		case data::type::buff_source_type::skill:
			return ext::any_of(m_buffs, [&source](auto value) { return source.get_skill_id() == value.first; });

		case data::type::buff_source_type::mob_skill:
			return ext::any_of(m_mob_skill_info, [&source](auto value) { return source.get_mob_skill_id() == value.first; });

		case data::type::buff_source_type::item:
			return ext::any_of(m_items, [&source](auto value) { return source.get_item_id() == value.first; });
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "buff_source_type");
}

auto buff::is_debuff(const data::type::buff_source &source) const -> bool {
	if (source.get_type() != data::type::buff_source_type::mob_skill) return false;
	return ext::any_of(m_mob_skill_info, [&source](auto value) { return source.get_mob_skill_id() == value.first; });
}

auto buff::get_info(const data::type::buff_source &source) const -> const data::type::buff & {
	switch (source.get_type()) {
		case data::type::buff_source_type::skill:
			return ext::find_value_ptr_if(m_buffs, [&source](auto value) { return value.first == source.get_skill_id(); })->second;

		case data::type::buff_source_type::mob_skill:
			return ext::find_value_ptr_if(m_mob_skill_info, [&source](auto value) { return value.first == source.get_mob_skill_id(); })->second;

		case data::type::buff_source_type::item:
			return ext::find_value_ptr_if(m_items, [&source](auto value) { return value.first == source.get_item_id(); })->second;
	}
	THROW_CODE_EXCEPTION(not_implemented_exception, "buff_source_type");
}

auto buff::get_buffs_by_effect() const -> const data::type::buff_info_by_effect & {
	return m_basics;
}

}
}
}