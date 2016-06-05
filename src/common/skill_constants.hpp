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
#pragma once

#include "common/types.hpp"

namespace vana {
	namespace buffs {
		const seconds max_buff_time = seconds{2100000};
		const int8_t byte_quantity = 16;
	}

	using buff_array = array<uint8_t, buffs::byte_quantity>;

	namespace status_effects {
		namespace mob {
			const uint8_t max_venom_count = 3;

			enum mob_status : uint32_t {
				watk = 0x01,
				wdef = 0x02,
				matk = 0x04,
				mdef = 0x08,
				acc = 0x10,

				avoid = 0x20,
				speed = 0x40,
				stun = 0x80,
				freeze = 0x100,
				poison = 0x200,

				seal = 0x400,
				no_clue1 = 0x800,
				weapon_attack_up = 0x1000,
				weapon_defense_up = 0x2000,
				magic_attack_up = 0x4000,

				magic_defense_up = 0x8000,
				doom = 0x10000,
				shadow_web = 0x20000,
				weapon_immunity = 0x40000,
				magic_immunity = 0x80000,

				no_clue2 = 0x100000,
				no_clue3 = 0x200000,
				ninja_ambush = 0x400000,
				no_clue4 = 0x800000,
				venomous_weapon = 0x1000000,

				no_clue5 = 0x2000000,
				no_clue6 = 0x4000000,
				empty = 0x8000000, // All mobs have this when they spawn
				hypnotize = 0x10000000,
				weapon_damage_reflect = 0x20000000,

				magic_damage_reflect = 0x40000000,
				no_clue7 = 0x80000000 // Last bit you can use with 4 bytes
			};
		}
		namespace player {
			enum player_status : int16_t {
				curse = 0x01,
				weakness = 0x02,
				darkness = 0x04,
				seal = 0x08,
				poison = 0x10,
				stun = 0x20,
				slow = 0x40,
				seduce = 0x80,
				zombify = 0x100,
				crazy_skull = 0x200
			};
		}
	}

	namespace mob_skills {
		enum : game_mob_skill_id {
			weapon_attack_up = 100,
			weapon_attack_up_aoe = 110,
			magic_attack_up = 101,
			magic_attack_up_aoe = 111,
			weapon_defense_up = 102,
			weapon_defense_up_aoe = 112,
			magic_defense_up = 103,
			magic_defense_up_aoe = 113,
			heal_aoe = 114,
			speed_up_aoe = 115,
			seal = 120,
			darkness = 121,
			weakness = 122,
			stun = 123,
			curse = 124,
			poison = 125,
			slow = 126,
			dispel = 127,
			seduce = 128,
			send_to_town = 129,
			poison_mist = 131,
			crazy_skull = 132,
			zombify = 133,
			weapon_immunity = 140,
			magic_immunity = 141,
			armor_skill = 142,
			weapon_damage_reflect = 143,
			magic_damage_reflect = 144,
			any_damage_reflect = 145,
			mc_weapon_attack_up = 150,
			mc_magic_attack_up = 151,
			mc_weapon_defense_up = 152,
			mc_magic_defense_up = 153,
			mc_accuracy_up = 154,
			mc_avoid_up = 155,
			mc_speed_up = 156,
			mc_seal = 157, // Not actually used in Monster Carnival
			summon = 200
		};
	}

	namespace skills {
		const uint8_t max_combo_orbs = 5;
		const uint8_t max_advanced_combo_orbs = 10;

		namespace all {
			enum skills : game_skill_id {
				regular_attack = 0
			};
		}
		namespace beginner {
			enum skills : game_skill_id {
				blessing_of_the_fairy = 12,
				echo_of_hero = 1005,
				follow_the_lead = 8,
				monster_rider = 1004,
				nimble_feet = 1002,
				recovery = 1001,
				three_snails = 1000,
				legendary_spirit = 1003,
				maker = 1007,
			};
		}
		namespace swordsman {
			enum skills : game_skill_id {
				improved_max_hp_increase = 1000001,
				iron_body = 1001003,
			};
		}
		namespace fighter {
			enum skills : game_skill_id {
				axe_booster = 1101005,
				axe_mastery = 1100001,
				power_guard = 1101007,
				rage = 1101006,
				sword_booster = 1101004,
				sword_mastery = 1100000,
			};
		}
		namespace crusader {
			enum skills : game_skill_id {
				armor_crash = 1111007,
				axe_coma = 1111006,
				axe_panic = 1111004,
				combo_attack = 1111002,
				shout = 1111008,
				sword_coma = 1111005,
				sword_panic = 1111003,
			};
		}
		namespace hero {
			enum skills : game_skill_id {
				achilles = 1120004,
				advanced_combo_attack = 1120003,
				enrage = 1121010,
				guardian = 1120005,
				heros_will = 1121011,
				maple_warrior = 1121000,
				monster_magnet = 1121001,
				power_stance = 1121002,
			};
		}
		namespace page {
			enum skills : game_skill_id {
				bw_booster = 1201005,
				bw_mastery = 1200001,
				power_guard = 1201007,
				sword_booster = 1201004,
				sword_mastery = 1200000,
				threaten = 1201006,
			};
		}
		namespace white_knight {
			enum skills : game_skill_id {
				bw_fire_charge = 1211004,
				bw_ice_charge = 1211006,
				bw_lit_charge = 1211008,
				charge_blow = 1211002,
				magic_crash = 1211009,
				sword_fire_charge = 1211003,
				sword_ice_charge = 1211005,
				sword_lit_charge = 1211007,
			};
		}
		namespace paladin {
			enum skills : game_skill_id {
				achilles = 1220005,
				advanced_charge = 1220010,
				bw_holy_charge = 1221004,
				guardian = 1220006,
				heavens_hammer = 1221011,
				heros_will = 1221012,
				maple_warrior = 1221000,
				monster_magnet = 1221001,
				power_stance = 1221002,
				sword_holy_charge = 1221003,
			};
		}
		namespace spearman {
			enum skills : game_skill_id {
				hyper_body = 1301007,
				iron_will = 1301006,
				polearm_booster = 1301005,
				polearm_mastery = 1300001,
				spear_booster = 1301004,
				spear_mastery = 1300000,
			};
		}
		namespace dragon_knight {
			enum skills : game_skill_id {
				dragon_blood = 1311008,
				dragon_roar = 1311006,
				elemental_resistance = 1310000,
				power_crash = 1311007,
				sacrifice = 1311005,
			};
		}
		namespace dark_knight {
			enum skills : game_skill_id {
				achilles = 1320005,
				aura_of_beholder = 1320008,
				beholder = 1321007,
				berserk = 1320006,
				heros_will = 1321010,
				hex_of_beholder = 1320009,
				maple_warrior = 1321000,
				monster_magnet = 1321001,
				power_stance = 1321002,
			};
		}
		namespace magician {
			enum skills : game_skill_id {
				improved_max_mp_increase = 2000001,
				magic_armor = 2001003,
				magic_guard = 2001002,
			};
		}
		namespace fp_wizard {
			enum skills : game_skill_id {
				meditation = 2101001,
				mp_eater = 2100000,
				poison_breath = 2101005,
				slow = 2101003,
			};
		}
		namespace fp_mage {
			enum skills : game_skill_id {
				element_amplification = 2110001,
				element_composition = 2111006,
				partial_resistance = 2110000,
				poison_mist = 2111003,
				seal = 2111004,
				spell_booster = 2111005,
			};
		}
		namespace fp_arch_mage {
			enum skills : game_skill_id {
				big_bang = 2121001,
				elquines = 2121005,
				fire_demon = 2121003,
				heros_will = 2121008,
				infinity = 2121004,
				mana_reflection = 2121002,
				maple_warrior = 2121000,
				paralyze = 2121006,
			};
		}
		namespace il_wizard {
			enum skills : game_skill_id {
				cold_beam = 2201004,
				meditation = 2201001,
				mp_eater = 2200000,
				slow = 2201003,
			};
		}
		namespace il_mage {
			enum skills : game_skill_id {
				element_amplification = 2210001,
				element_composition = 2211006,
				ice_strike = 2211002,
				partial_resistance = 2210000,
				seal = 2211004,
				spell_booster = 2211005,
			};
		}
		namespace il_arch_mage {
			enum skills : game_skill_id {
				big_bang = 2221001,
				blizzard = 2221007,
				heros_will = 2221008,
				ice_demon = 2221003,
				ifrit = 2221005,
				infinity = 2221004,
				mana_reflection = 2221002,
				maple_warrior = 2221000,
			};
		}
		namespace cleric {
			enum skills : game_skill_id {
				bless = 2301004,
				heal = 2301002,
				invincible = 2301003,
				mp_eater = 2300000,
			};
		}
		namespace priest {
			enum skills : game_skill_id {
				dispel = 2311001,
				doom = 2311005,
				elemental_resistance = 2310000,
				holy_symbol = 2311003,
				mystic_door = 2311002,
				summon_dragon = 2311006,
			};
		}
		namespace bishop {
			enum skills : game_skill_id {
				bahamut = 2321003,
				big_bang = 2321001,
				heros_will = 2321009,
				holy_shield = 2321005,
				infinity = 2321004,
				mana_reflection = 2321002,
				maple_warrior = 2321000,
				resurrection = 2321006,
			};
		}
		namespace archer {
			enum skills : game_skill_id {
				critical_shot = 3000001,
				focus = 3001003,
			};
		}
		namespace hunter {
			enum skills : game_skill_id {
				arrow_bomb = 3101005,
				bow_booster = 3101002,
				bow_mastery = 3100000,
				soul_arrow = 3101004,
			};
		}
		namespace ranger {
			enum skills : game_skill_id {
				mortal_blow = 3110001,
				puppet = 3111002,
				silver_hawk = 3111005,
			};
		}
		namespace bowmaster {
			enum skills : game_skill_id {
				concentrate = 3121008,
				hamstring = 3121007,
				heros_will = 3121009,
				hurricane = 3121004,
				maple_warrior = 3121000,
				phoenix = 3121006,
				sharp_eyes = 3121002,
				bow_expert = 3120005,
			};
		}
		namespace crossbowman {
			enum skills : game_skill_id {
				crossbow_booster = 3201002,
				crossbow_mastery = 3200000,
				soul_arrow = 3201004,
			};
		}
		namespace sniper {
			enum skills : game_skill_id {
				blizzard = 3211003,
				golden_eagle = 3211005,
				mortal_blow = 3210001,
				puppet = 3211002,
			};
		}
		namespace marksman {
			enum skills : game_skill_id {
				blind = 3221006,
				frostprey = 3221005,
				heros_will = 3221008,
				maple_warrior = 3221000,
				piercing_arrow = 3221001,
				sharp_eyes = 3221002,
				snipe = 3221007,
			};
		}
		namespace rogue {
			enum skills : game_skill_id {
				dark_sight = 4001003,
				disorder = 4001002,
				double_stab = 4001334,
				lucky_seven = 4001344,
			};
		}
		namespace assassin {
			enum skills : game_skill_id {
				claw_booster = 4101003,
				claw_mastery = 4100000,
				critical_throw = 4100001,
				drain = 4101005,
				haste = 4101004,
			};
		}
		namespace hermit {
			enum skills : game_skill_id {
				alchemist = 4110000,
				avenger = 4111005,
				meso_up = 4111001,
				shadow_meso = 4111004,
				shadow_partner = 4111002,
				shadow_web = 4111003,
			};
		}
		namespace night_lord {
			enum skills : game_skill_id {
				heros_will = 4121009,
				maple_warrior = 4121000,
				ninja_ambush = 4121004,
				ninja_storm = 4121008,
				shadow_shifter = 4120002,
				shadow_stars = 4121006,
				taunt = 4121003,
				triple_throw = 4121007,
				venomous_star = 4120005,
			};
		}
		namespace bandit {
			enum skills : game_skill_id {
				dagger_booster = 4201002,
				dagger_mastery = 4200000,
				haste = 4201003,
				savage_blow = 4201005,
				steal = 4201004,
			};
		}
		namespace chief_bandit {
			enum skills : game_skill_id {
				assaulter = 4211002,
				band_of_thieves = 4211004,
				chakra = 4211001,
				meso_explosion = 4211006,
				meso_guard = 4211005,
				pickpocket = 4211003,
			};
		}
		namespace shadower {
			enum skills : game_skill_id {
				assassinate = 4221001,
				boomerang_step = 4221007,
				heros_will = 4221008,
				maple_warrior = 4221000,
				ninja_ambush = 4221004,
				shadow_shifter = 4220002,
				smokescreen = 4221006,
				taunt = 4221003,
				venomous_stab = 4220005,
			};
		}
		namespace pirate {
			enum skills : game_skill_id {
				dash = 5001005,
			};
		}
		namespace brawler {
			enum skills : game_skill_id {
				backspin_blow = 5101002,
				corkscrew_blow = 5101004,
				double_uppercut = 5101003,
				improve_max_hp = 5100000,
				knuckler_booster = 5101006,
				knuckler_mastery = 5100001,
				mp_recovery = 5101005,
				oak_barrel = 5101007,
			};
		}
		namespace marauder {
			enum skills : game_skill_id {
				energy_charge = 5110001,
				energy_drain = 5111004,
				stun_mastery = 5110000,
				transformation = 5111005,
			};
		}
		namespace buccaneer {
			enum skills : game_skill_id {
				demolition = 5121004,
				maple_warrior = 5121000,
				heros_will = 5121008,
				snatch = 5121005,
				speed_infusion = 5121009,
				super_transformation = 5121003,
				time_leap = 5121010,
			};
		}
		namespace gunslinger {
			enum skills : game_skill_id {
				blank_shot = 5201004,
				grenade = 5201002,
				gun_booster = 5201003,
				gun_mastery = 5200000,
			};
		}
		namespace outlaw {
			enum skills : game_skill_id {
				flamethrower = 5211004,
				gaviota = 5211002,
				homing_beacon = 5211006,
				ice_splitter = 5211005,
				octopus = 5211001,
			};
		}
		namespace corsair {
			enum skills : game_skill_id {
				aerial_strike = 5221003,
				battleship = 5221006,
				bullseye = 5220011,
				elemental_boost = 5220001,
				hypnotize = 5221009,
				maple_warrior = 5221000,
				rapid_fire = 5221004,
				heros_will = 5221010,
				wrath_of_the_octopi = 5220002,
			};
		}
		namespace gm {
			enum skills : game_skill_id {
				haste = 9001000,
				super_dragon_roar = 9001001,
				teleport = 9001007,
			};
		}
		namespace super_gm {
			enum skills : game_skill_id {
				bless = 9101003,
				haste = 9101001,
				heal_plus_dispel = 9101000,
				hide = 9101004,
				holy_symbol = 9101002,
				hyper_body = 9101008,
				resurrection = 9101005,
				super_dragon_roar = 9101006,
				teleport = 9101007,
			};
		}
		namespace noblesse {
			enum skills : game_skill_id {
				blessing_of_the_fairy = 10000012,
				echo_of_hero = 10001005,
				maker = 10001007,
				monster_rider = 10001004,
				nimble_feet = 10001002,
				recovery = 10001001,
				three_snails = 10001000,
				follow_the_lead = 10000018,
				legendary_spirit = 10001003,
			};
		}
		namespace dawn_warrior {
			enum skills : game_skill_id {
				advanced_combo = 11110005,
				coma = 11111003,
				combo_attack = 11111001,
				final_attack = 11101002,
				iron_body = 11001001,
				max_hp_enhancement = 11000000,
				panic = 11111002,
				rage = 11101003,
				soul = 11001004,
				soul_blade = 11101004,
				soul_charge = 11111007,
				sword_booster = 11101001,
				sword_mastery = 11100000,
			};
		}
		namespace blaze_wizard {
			enum skills : game_skill_id {
				elemental_reset = 12101005,
				element_amplification = 12110001,
				fire_strike = 12111006,
				flame = 12001004,
				flame_gear = 12111005,
				ifrit = 12111004,
				increasing_max_mp = 12000000,
				magic_armor = 12001002,
				magic_guard = 12001001,
				meditation = 12101000,
				seal = 12111002,
				slow = 12101001,
				spell_booster = 12101004,
			};
		}
		namespace wind_archer {
			enum skills : game_skill_id {
				eagle_eye = 13111005,
				bow_booster = 13101001,
				bow_mastery = 13100000,
				critical_shot = 13000000,
				final_attack = 13101002,
				focus = 13001002,
				hurricane = 13111002,
				puppet = 13111004,
				soul_arrow = 13101003,
				storm = 13001004,
				wind_piercing = 13111006,
				wind_shot = 13111007,
				wind_walk = 13101006,
				bow_expert = 13110003,
			};
		}
		namespace night_walker {
			enum skills : game_skill_id {
				alchemist = 14110003,
				disorder = 14001002,
				dark_sight = 14001003,
				darkness = 14001005,
				claw_booster = 14101002,
				claw_mastery = 14100000,
				critical_throw = 14100001,
				haste = 14101003,
				poison_bomb = 14111006,
				shadow_partner = 14111000,
				shadow_web = 14111001,
				vanish = 14100005,
				vampire = 14101006,
				venom = 14110004,
			};
		}
		namespace thunder_breaker {
			enum skills : game_skill_id {
				corkscrew_blow = 15101003,
				dash = 15001003,
				energy_charge = 15100004,
				energy_drain = 15111001,
				improve_max_hp = 15100000,
				knuckler_booster = 15101002,
				knuckler_mastery = 15100001,
				lightning = 15001004,
				lightning_charge = 15101006,
				spark = 15111006,
				shark_wave = 15111007,
				speed_infusion = 15111005,
				transformation = 15111002,
			};
		}
	}
}