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
#include "BuffDataProvider.hpp"
#include "BuffSource.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "InitializeCommon.hpp"
#include "ItemDataProvider.hpp"
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
			skills::magician::magic_guard,
			skills::blaze_wizard::magic_guard,
		});

	process_skills(
		buff{{
			speed,
			dark_sight,
		}}, {
			skills::rogue::dark_sight,
			skills::night_walker::dark_sight,
		});

	process_skills(
		buff{{
			booster,
		}}, {
			skills::fighter::sword_booster,
			skills::fighter::axe_booster,
			skills::page::sword_booster,
			skills::page::bw_booster,
			skills::spearman::spear_booster,
			skills::spearman::polearm_booster,
			skills::fp_mage::spell_booster,
			skills::il_mage::spell_booster,
			skills::hunter::bow_booster,
			skills::crossbowman::crossbow_booster,
			skills::assassin::claw_booster,
			skills::bandit::dagger_booster,
			skills::brawler::knuckler_booster,
			skills::gunslinger::gun_booster,
			skills::dawn_warrior::sword_booster,
			skills::blaze_wizard::spell_booster,
			skills::wind_archer::bow_booster,
			skills::night_walker::claw_booster,
			skills::thunder_breaker::knuckler_booster,
		});

	process_skills(
		buff{{
			power_guard,
		}}, {
			skills::fighter::power_guard,
			skills::page::power_guard,
		});

	process_skills(
		buff{{
			hyper_body_hp,
			hyper_body_mp,
		}}, {
			skills::spearman::hyper_body,
			skills::super_gm::hyper_body,
		});

	process_skills(
		buff{{
			invincible,
		}}, {
			skills::cleric::invincible,
		});

	process_skills(
		buff{{
			soul_arrow,
		}}, {
			skills::hunter::soul_arrow,
			skills::crossbowman::soul_arrow,
			skills::wind_archer::soul_arrow,
		});

	process_skills(
		buff{{
			combo,
		}}, {
			skills::crusader::combo_attack,
			skills::dawn_warrior::combo_attack,
		});

	process_skills(
		buff{{
			magic_attack,
			charge,
		}}, {
			skills::white_knight::bw_fire_charge,
			skills::white_knight::bw_ice_charge,
			skills::white_knight::bw_lit_charge,
			skills::white_knight::sword_fire_charge,
			skills::white_knight::sword_ice_charge,
			skills::white_knight::sword_lit_charge,
			skills::paladin::bw_holy_charge,
			skills::paladin::sword_holy_charge,
			skills::dawn_warrior::soul_charge,
			skills::thunder_breaker::lightning_charge,
		});

	process_skills(
		buff{{
			physical_attack,
			timed_hurt.
				with_act(buff_action::hurt, buff_skill_value::x, seconds{4}),
		}}, {
			skills::dragon_knight::dragon_blood,
		});

	process_skills(
		buff{{
			holy_symbol,
		}}, {
			skills::priest::holy_symbol,
			skills::super_gm::holy_symbol,
		});

	process_skills(
		buff{{
			meso_up,
		}}, {
			skills::hermit::meso_up,
		});

	process_skills(
		buff{{
			shadow_partner,
		}}, {
			skills::hermit::shadow_partner,
			skills::night_walker::shadow_partner,
		});

	process_skills(
		buff{{
			pickpocket,
		}}, {
			skills::chief_bandit::pickpocket,
		});

	process_skills(
		buff{{
			meso_guard,
		}}, {
			skills::chief_bandit::meso_guard,
		});

	process_skills(
		buff{{
			timed_heal.
				with_act(buff_action::heal, buff_skill_value::x, milliseconds{4900}),
		}}, {
			skills::beginner::recovery,
			skills::noblesse::recovery,
		});

	process_skills(
		buff{{
			maple_warrior,
		}}, {
			skills::hero::maple_warrior,
			skills::paladin::maple_warrior,
			skills::dark_knight::maple_warrior,
			skills::fp_arch_mage::maple_warrior,
			skills::il_arch_mage::maple_warrior,
			skills::bishop::maple_warrior,
			skills::bowmaster::maple_warrior,
			skills::marksman::maple_warrior,
			skills::night_lord::maple_warrior,
			skills::shadower::maple_warrior,
			skills::buccaneer::maple_warrior,
			skills::corsair::maple_warrior,
		});

	process_skills(
		buff{{
			power_stance,
		}}, {
			skills::hero::power_stance,
			skills::paladin::power_stance,
			skills::dark_knight::power_stance,
		});

	process_skills(
		buff{{
			sharp_eyes,
		}}, {
			skills::bowmaster::sharp_eyes,
			skills::marksman::sharp_eyes,
		});

	process_skills(
		buff{{
			mana_reflection,
		}}, {
			skills::fp_arch_mage::mana_reflection,
			skills::il_arch_mage::mana_reflection,
			skills::bishop::mana_reflection,
		});

	process_skills(
		buff{{
			shadow_stars,
		}}, {
			skills::night_lord::shadow_stars,
		});

	process_skills(
		buff{{
			infinity,
		}}, {
			skills::fp_arch_mage::infinity,
			skills::il_arch_mage::infinity,
			skills::bishop::infinity,
		});

	process_skills(
		buff{{
			holy_shield,
		}}, {
			skills::bishop::holy_shield,
		});

	process_skills(
		buff{{
			hamstring,
		}}, {
			skills::bowmaster::hamstring,
		});

	process_skills(
		buff{{
			blind,
		}}, {
			skills::marksman::blind,
		});

	process_skills(
		buff{{
			physical_attack,
			concentrate,
		}}, {
			skills::bowmaster::concentrate,
		});

	process_skills(
		buff{{
			physical_attack,
			echo,
		}}, {
			skills::beginner::echo_of_hero,
			skills::noblesse::echo_of_hero,
		});

	process_skills(
		buff{{
			unk,
		}}, {
			skills::super_gm::hide,
		});

	process_skills(
		buff{{
			spark,
		}}, {
			skills::thunder_breaker::spark,
		});

	process_skills(
		buff{{
			dawn_warrior_final_attack,
		}}, {
			skills::dawn_warrior::final_attack,
		});

	process_skills(
		buff{{
			wind_walker_final_attack,
		}}, {
			skills::wind_archer::final_attack,
		});

	process_skills(
		buff{{
			elemental_reset,
		}}, {
			skills::blaze_wizard::elemental_reset,
		});

	process_skills(
		buff{{
			speed,
			wind_walk,
		}}, {
			skills::wind_archer::wind_walk,
		});

	process_skills(
		buff{{
			energy_charge,
		}}, {
			skills::marauder::energy_charge,
			skills::thunder_breaker::energy_charge,
		});

	process_skills(
		buff{{
			dash_speed,
			dash_jump,
		}}, {
			skills::pirate::dash,
			skills::thunder_breaker::dash,
		});

	process_skills(
		buff{{
			mount,
		}}, {
			skills::beginner::monster_rider,
			skills::noblesse::monster_rider,
		});

	process_skills(
		buff{{
			speed_infusion,
		}}, {
			skills::buccaneer::speed_infusion,
			skills::thunder_breaker::speed_infusion,
		});

	process_skills(
		buff{{
			homing_beacon,
		}}, {
			skills::outlaw::homing_beacon,
			skills::corsair::bullseye,
		});

	process_skills(
		buff{{
			physical_defense,
		}}, {
			skills::magician::magic_armor,
			skills::swordsman::iron_body,
			skills::blaze_wizard::magic_armor,
			skills::dawn_warrior::iron_body,
		});

	process_skills(
		buff{{
			accuracy,
			avoid,
		}}, {
			skills::archer::focus,
			skills::wind_archer::focus,
		});

	process_skills(
		buff{{
			physical_attack,
			physical_defense,
		}}, {
			skills::fighter::rage,
			skills::dawn_warrior::rage,
		});

	process_skills(
		buff{{
			physical_defense,
			magic_defense,
		}}, {
			skills::spearman::iron_will,
		});

	process_skills(
		buff{{
			magic_attack,
		}}, {
			skills::fp_wizard::meditation,
			skills::il_wizard::meditation,
			skills::blaze_wizard::meditation,
		});

	process_skills(
		buff{{
			physical_attack,
			physical_defense,
			magic_defense,
			accuracy,
			avoid,
		}}, {
			skills::cleric::bless,
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
			skills::super_gm::bless,
		});

	process_skills(
		buff{{
			physical_attack,
		}}, {
			skills::hero::enrage,
		});

	process_skills(
		buff{{
			stun.
				with_predefined_value(1).
				with_map_info({}),
		}}, {
			skills::dragon_knight::dragon_roar,
		});

	process_skills(
		buff{{
			speed,
		}}, {
			skills::beginner::nimble_feet,
			skills::noblesse::nimble_feet,
		});

	process_skills(
		buff{{
			speed,
			jump,
		}}, {
			skills::assassin::haste,
			skills::bandit::haste,
			skills::night_walker::haste,
			skills::gm::haste,
			skills::super_gm::haste,
		});

	process_skills(
		buff{{
			morph,
		}}, {
			skills::brawler::oak_barrel,
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
			skills::marauder::transformation,
			skills::buccaneer::super_transformation,
			skills::wind_archer::eagle_eye,
			skills::thunder_breaker::transformation,
		});

	process_skills(
		buff{{
			physical_defense,
			magic_defense,
			mount,
		}}, {
			skills::corsair::battleship,
		});

	m_mob_skill_info[mob_skills::stun] = buff{stun};
	m_mob_skill_info[mob_skills::poison] = buff{poison};
	m_mob_skill_info[mob_skills::seal] = buff{seal};
	m_mob_skill_info[mob_skills::darkness] = buff{darkness};
	m_mob_skill_info[mob_skills::weakness] = buff{weakness};
	m_mob_skill_info[mob_skills::curse] = buff{curse};
	m_mob_skill_info[mob_skills::slow] = buff{slow};
	m_mob_skill_info[mob_skills::seduce] = buff{seduce};
	m_mob_skill_info[mob_skills::crazy_skull] = buff{crazy_skull};
	m_mob_skill_info[mob_skills::zombify] = buff{zombify};

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