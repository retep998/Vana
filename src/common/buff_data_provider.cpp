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
#include "buff_data_provider.hpp"
#include "common/buff_source.hpp"
#include "common/constant/mob_skill.hpp"
#include "common/constant/skill.hpp"
#include "common/game_logic_utilities.hpp"
#include "common/initialize_common.hpp"
#include "common/item_data_provider.hpp"
#include <iomanip>
#include <iostream>

namespace vana {

auto buff_data_provider::process_skills(buff value, const init_list<game_skill_id> &skills) -> void {
	for (const auto &s : skills) {
		if (m_buffs.find(s) != std::end(m_buffs)) throw std::invalid_argument{"skill is already present"};
		m_buffs[s] = value;
	}
}

auto buff_data_provider::load_data() -> void {
	std::cout << std::setw(initializing::output_width) << std::left << "Initializing Buffs... ";

	auto physical_attack = buff_info::from_player_only(1, buff_skill_value::watk);
	auto physical_defense = buff_info::from_player_only(2, buff_skill_value::wdef);
	auto magic_attack = buff_info::from_player_only(3, buff_skill_value::matk);
	auto magic_defense = buff_info::from_player_only(4, buff_skill_value::mdef);
	auto accuracy = buff_info::from_player_only(5, buff_skill_value::accuracy);
	auto avoid = buff_info::from_player_only(6, buff_skill_value::avoid);
	auto craft = buff_info::from_player_only(7, buff_skill_value::predefined);
	auto speed = buff_info::from_map_movement(8, buff_skill_value::speed, buff_map_info{1, buff_skill_value::speed});
	auto jump = buff_info::from_player_only_movement(9, buff_skill_value::jump);
	auto magic_guard = buff_info::from_player_only(10, buff_skill_value::x);
	auto dark_sight = buff_info::from_map_no_movement(11, buff_skill_value::x, buff_map_info{});
	auto booster = buff_info::from_player_only(12, buff_skill_value::x);
	auto power_guard = buff_info::from_player_only(13, buff_skill_value::x);
	auto hyper_body_hp = buff_info::from_player_only(14, buff_skill_value::x);
	auto hyper_body_mp = buff_info::from_player_only(15, buff_skill_value::y);
	auto invincible = buff_info::from_player_only(16, buff_skill_value::x);
	auto soul_arrow = buff_info::from_map_no_movement(17, buff_skill_value::x, buff_map_info{});
	auto stun = buff_info::from_map_movement(18, 1, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto poison = buff_info::from_map_no_movement(19, buff_skill_value::x, buff_map_info{buff_skill_value::special_processing});
	auto seal = buff_info::from_map_no_movement(20, 2, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto darkness = buff_info::from_map_no_movement(21, 1, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto combo = buff_info::from_map_no_movement(22, buff_skill_value::special_processing, buff_map_info{1, buff_skill_value::special_processing});
	auto charge = buff_info::from_map_no_movement(23, buff_skill_value::matk, buff_map_info{4, buff_skill_value::skill_id});
	auto timed_hurt = buff_info::from_player_only(24, buff_skill_value::level);
	auto holy_symbol = buff_info::from_player_only(25, buff_skill_value::x);
	auto meso_up = buff_info::from_player_only(26, buff_skill_value::x);
	auto shadow_partner = buff_info::from_map_no_movement(27, buff_skill_value::bitpacked_xy_16, buff_map_info{});
	auto pickpocket = buff_info::from_player_only(28, buff_skill_value::x);
	auto meso_guard = buff_info::from_player_only(29, buff_skill_value::x);
	// 30 - ???
	auto weakness = buff_info::from_map_movement(31, 1, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto curse = buff_info::from_map_no_movement(32, 1, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto slow = buff_info::from_player_only_movement(33, buff_skill_value::x);
	auto morph = buff_info::from_map_movement(34, buff_skill_value::morph, buff_map_info{2, buff_skill_value::morph});
	auto timed_heal = buff_info::from_player_only(35, buff_skill_value::x);
	// No, I have no idea why Maple Warrior is a "movement" buff
	auto maple_warrior = buff_info::from_player_only_movement(36, buff_skill_value::x);
	auto power_stance = buff_info::from_player_only(37, buff_skill_value::prop);
	auto sharp_eyes = buff_info::from_player_only(38, buff_skill_value::bitpacked_xy_16);
	auto mana_reflection = buff_info::from_player_only(39, buff_skill_value::level);
	auto seduce = buff_info::from_map_movement(40, buff_skill_value::x, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto shadow_stars = buff_info::from_map_no_movement(41, buff_skill_value::special_processing, buff_map_info{4, buff_skill_value::special_processing});
	auto infinity = buff_info::from_player_only(42, buff_skill_value::x);
	auto holy_shield = buff_info::from_player_only(43, buff_skill_value::x);
	auto hamstring = buff_info::from_player_only(44, buff_skill_value::x);
	auto blind = buff_info::from_player_only(45, buff_skill_value::x);
	auto concentrate = buff_info::from_player_only(46, buff_skill_value::x);
	auto zombify = buff_info::from_map_no_movement(47, buff_skill_value::x, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	auto echo = buff_info::from_player_only(48, buff_skill_value::x);
	// 49 - ???
	// 50 - ???
	// 51 - ???
	auto crazy_skull = buff_info::from_map_no_movement(52, buff_skill_value::x, buff_map_info{4, buff_skill_value::bitpacked_skill_and_level_32});
	// 53 - ???
	auto ignore_weapon_immunity = buff_info::from_player_only(54, buff_skill_value::predefined); // Value is a percentage, so 100 means always ignore
	auto ignore_magic_immunity = buff_info::from_player_only(55, buff_skill_value::predefined); // Value is a percentage, so 100 means always ignore
	// 56 - ???
	// ??? - not sure what normally goes here, I use it for GM Hide because it appears to have no ill effects
	auto unk = buff_info::from_player_only(57, buff_skill_value::special_processing);
	// 58 - ???
	// 59 - ???
	// 60 - ???
	// 61 - ???
	auto spark = buff_info::from_player_only(62, buff_skill_value::x);
	// 63 - ???
	auto dawn_warrior_final_attack = buff_info::from_player_only(64, 1);
	auto wind_walker_final_attack = buff_info::from_player_only(65, 1);
	auto elemental_reset = buff_info::from_player_only(66, buff_skill_value::x);
	auto wind_walk = buff_info::from_map_no_movement(67, 1, buff_map_info{});
	auto energy_charge = buff_info::from_map_no_movement(68, buff_skill_value::special_packet, buff_map_info{buff_skill_value::special_packet});
	auto dash_speed = buff_info::from_map_movement(69, buff_skill_value::special_packet, buff_map_info{buff_skill_value::special_packet});
	auto dash_jump = buff_info::from_map_movement(70, buff_skill_value::special_packet, buff_map_info{buff_skill_value::special_packet});
	auto mount = buff_info::from_map_movement(71, buff_skill_value::special_packet, buff_map_info{buff_skill_value::special_packet});
	auto speed_infusion = buff_info::from_player_only(72, buff_skill_value::special_packet);
	auto homing_beacon = buff_info::from_player_only(73, buff_skill_value::special_packet);

	process_skills(
		buff{{
			magic_guard,
		}}, {
			constant::skill::magician::magic_guard,
			constant::skill::blaze_wizard::magic_guard,
		});

	process_skills(
		buff{{
			speed,
			dark_sight,
		}}, {
			constant::skill::rogue::dark_sight,
			constant::skill::night_walker::dark_sight,
		});

	process_skills(
		buff{{
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
		buff{{
			power_guard,
		}}, {
			constant::skill::fighter::power_guard,
			constant::skill::page::power_guard,
		});

	process_skills(
		buff{{
			hyper_body_hp,
			hyper_body_mp,
		}}, {
			constant::skill::spearman::hyper_body,
			constant::skill::super_gm::hyper_body,
		});

	process_skills(
		buff{{
			invincible,
		}}, {
			constant::skill::cleric::invincible,
		});

	process_skills(
		buff{{
			soul_arrow,
		}}, {
			constant::skill::hunter::soul_arrow,
			constant::skill::crossbowman::soul_arrow,
			constant::skill::wind_archer::soul_arrow,
		});

	process_skills(
		buff{{
			combo,
		}}, {
			constant::skill::crusader::combo_attack,
			constant::skill::dawn_warrior::combo_attack,
		});

	process_skills(
		buff{{
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
		buff{{
			physical_attack,
			timed_hurt.
				with_act(buff_action::hurt, buff_skill_value::x, seconds{4}),
		}}, {
			constant::skill::dragon_knight::dragon_blood,
		});

	process_skills(
		buff{{
			holy_symbol,
		}}, {
			constant::skill::priest::holy_symbol,
			constant::skill::super_gm::holy_symbol,
		});

	process_skills(
		buff{{
			meso_up,
		}}, {
			constant::skill::hermit::meso_up,
		});

	process_skills(
		buff{{
			shadow_partner,
		}}, {
			constant::skill::hermit::shadow_partner,
			constant::skill::night_walker::shadow_partner,
		});

	process_skills(
		buff{{
			pickpocket,
		}}, {
			constant::skill::chief_bandit::pickpocket,
		});

	process_skills(
		buff{{
			meso_guard,
		}}, {
			constant::skill::chief_bandit::meso_guard,
		});

	process_skills(
		buff{{
			timed_heal.
				with_act(buff_action::heal, buff_skill_value::x, milliseconds{4900}),
		}}, {
			constant::skill::beginner::recovery,
			constant::skill::noblesse::recovery,
		});

	process_skills(
		buff{{
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
		buff{{
			power_stance,
		}}, {
			constant::skill::hero::power_stance,
			constant::skill::paladin::power_stance,
			constant::skill::dark_knight::power_stance,
		});

	process_skills(
		buff{{
			sharp_eyes,
		}}, {
			constant::skill::bowmaster::sharp_eyes,
			constant::skill::marksman::sharp_eyes,
		});

	process_skills(
		buff{{
			mana_reflection,
		}}, {
			constant::skill::fp_arch_mage::mana_reflection,
			constant::skill::il_arch_mage::mana_reflection,
			constant::skill::bishop::mana_reflection,
		});

	process_skills(
		buff{{
			shadow_stars,
		}}, {
			constant::skill::night_lord::shadow_stars,
		});

	process_skills(
		buff{{
			infinity,
		}}, {
			constant::skill::fp_arch_mage::infinity,
			constant::skill::il_arch_mage::infinity,
			constant::skill::bishop::infinity,
		});

	process_skills(
		buff{{
			holy_shield,
		}}, {
			constant::skill::bishop::holy_shield,
		});

	process_skills(
		buff{{
			hamstring,
		}}, {
			constant::skill::bowmaster::hamstring,
		});

	process_skills(
		buff{{
			blind,
		}}, {
			constant::skill::marksman::blind,
		});

	process_skills(
		buff{{
			physical_attack,
			concentrate,
		}}, {
			constant::skill::bowmaster::concentrate,
		});

	process_skills(
		buff{{
			physical_attack,
			echo,
		}}, {
			constant::skill::beginner::echo_of_hero,
			constant::skill::noblesse::echo_of_hero,
		});

	process_skills(
		buff{{
			unk,
		}}, {
			constant::skill::super_gm::hide,
		});

	process_skills(
		buff{{
			spark,
		}}, {
			constant::skill::thunder_breaker::spark,
		});

	process_skills(
		buff{{
			dawn_warrior_final_attack,
		}}, {
			constant::skill::dawn_warrior::final_attack,
		});

	process_skills(
		buff{{
			wind_walker_final_attack,
		}}, {
			constant::skill::wind_archer::final_attack,
		});

	process_skills(
		buff{{
			elemental_reset,
		}}, {
			constant::skill::blaze_wizard::elemental_reset,
		});

	process_skills(
		buff{{
			speed,
			wind_walk,
		}}, {
			constant::skill::wind_archer::wind_walk,
		});

	process_skills(
		buff{{
			energy_charge,
		}}, {
			constant::skill::marauder::energy_charge,
			constant::skill::thunder_breaker::energy_charge,
		});

	process_skills(
		buff{{
			dash_speed,
			dash_jump,
		}}, {
			constant::skill::pirate::dash,
			constant::skill::thunder_breaker::dash,
		});

	process_skills(
		buff{{
			mount,
		}}, {
			constant::skill::beginner::monster_rider,
			constant::skill::noblesse::monster_rider,
		});

	process_skills(
		buff{{
			speed_infusion,
		}}, {
			constant::skill::buccaneer::speed_infusion,
			constant::skill::thunder_breaker::speed_infusion,
		});

	process_skills(
		buff{{
			homing_beacon,
		}}, {
			constant::skill::outlaw::homing_beacon,
			constant::skill::corsair::bullseye,
		});

	process_skills(
		buff{{
			physical_defense,
		}}, {
			constant::skill::magician::magic_armor,
			constant::skill::swordsman::iron_body,
			constant::skill::blaze_wizard::magic_armor,
			constant::skill::dawn_warrior::iron_body,
		});

	process_skills(
		buff{{
			accuracy,
			avoid,
		}}, {
			constant::skill::archer::focus,
			constant::skill::wind_archer::focus,
		});

	process_skills(
		buff{{
			physical_attack,
			physical_defense,
		}}, {
			constant::skill::fighter::rage,
			constant::skill::dawn_warrior::rage,
		});

	process_skills(
		buff{{
			physical_defense,
			magic_defense,
		}}, {
			constant::skill::spearman::iron_will,
		});

	process_skills(
		buff{{
			magic_attack,
		}}, {
			constant::skill::fp_wizard::meditation,
			constant::skill::il_wizard::meditation,
			constant::skill::blaze_wizard::meditation,
		});

	process_skills(
		buff{{
			physical_attack,
			physical_defense,
			magic_defense,
			accuracy,
			avoid,
		}}, {
			constant::skill::cleric::bless,
		});

	process_skills(
		buff{{
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
		buff{{
			physical_attack,
		}}, {
			constant::skill::hero::enrage,
		});

	process_skills(
		buff{{
			stun.
				with_predefined_value(1).
				with_map_info({}),
		}}, {
			constant::skill::dragon_knight::dragon_roar,
		});

	process_skills(
		buff{{
			speed,
		}}, {
			constant::skill::beginner::nimble_feet,
			constant::skill::noblesse::nimble_feet,
		});

	process_skills(
		buff{{
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
		buff{{
			morph,
		}}, {
			constant::skill::brawler::oak_barrel,
		});

	process_skills(
		buff{{
			physical_defense,
			magic_defense,
			speed,
			jump,
			morph.
				with_value(buff_skill_value::gender_specific_morph).
				with_map_info(buff_map_info{2, buff_skill_value::gender_specific_morph}),
		}}, {
			constant::skill::marauder::transformation,
			constant::skill::buccaneer::super_transformation,
			constant::skill::wind_archer::eagle_eye,
			constant::skill::thunder_breaker::transformation,
		});

	process_skills(
		buff{{
			physical_defense,
			magic_defense,
			mount,
		}}, {
			constant::skill::corsair::battleship,
		});

	m_mob_skill_info[constant::mob_skill::stun] = buff{stun};
	m_mob_skill_info[constant::mob_skill::poison] = buff{poison};
	m_mob_skill_info[constant::mob_skill::seal] = buff{seal};
	m_mob_skill_info[constant::mob_skill::darkness] = buff{darkness};
	m_mob_skill_info[constant::mob_skill::weakness] = buff{weakness};
	m_mob_skill_info[constant::mob_skill::curse] = buff{curse};
	m_mob_skill_info[constant::mob_skill::slow] = buff{slow};
	m_mob_skill_info[constant::mob_skill::seduce] = buff{seduce};
	m_mob_skill_info[constant::mob_skill::crazy_skull] = buff{crazy_skull};
	m_mob_skill_info[constant::mob_skill::zombify] = buff{zombify};

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

auto buff_data_provider::add_item_info(game_item_id item_id, const consume_info &cons) -> void {
	vector<buff_info> values;

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
			with_map_info(buff_map_info{1, cons.speed}));
	}
	if (cons.jump > 0) {
		values.push_back(m_basics.jump.with_predefined_value(cons.jump));
	}
	if (cons.morphs.size() > 0) {
		for (const auto &m : cons.morphs) {
			values.push_back(m_basics.morph.
				with_predefined_value(m.morph).
				with_chance(m.chance).
				with_map_info(buff_map_info{2, m.morph}));
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
		m_items[item_id] = buff{values};
	}
}

auto buff_data_provider::is_buff(const buff_source &source) const -> bool {
	switch (source.get_type()) {
		case buff_source_type::skill: return m_buffs.find(source.get_skill_id()) != std::end(m_buffs);
		case buff_source_type::mob_skill: return m_mob_skill_info.find(source.get_mob_skill_id()) != std::end(m_mob_skill_info);
		case buff_source_type::item: return m_items.find(source.get_item_id()) != std::end(m_items);
	}
	throw not_implemented_exception{"buff_source_type"};
}

auto buff_data_provider::is_debuff(const buff_source &source) const -> bool {
	if (source.get_type() != buff_source_type::mob_skill) return false;
	return m_mob_skill_info.find(source.get_mob_skill_id()) != std::end(m_mob_skill_info);
}

auto buff_data_provider::get_info(const buff_source &source) const -> const buff & {
	switch (source.get_type()) {
		case buff_source_type::skill: return m_buffs.find(source.get_skill_id())->second;
		case buff_source_type::mob_skill: return m_mob_skill_info.find(source.get_mob_skill_id())->second;
		case buff_source_type::item: return m_items.find(source.get_item_id())->second;
	}
	throw not_implemented_exception{"buff_source_type"};
}

auto buff_data_provider::get_buffs_by_effect() const -> const buff_info_by_effect & {
	return m_basics;
}

}