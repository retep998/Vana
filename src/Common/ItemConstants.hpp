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

#include "Common/FileTime.hpp"
#include "Common/Types.hpp"
#include <limits>

namespace vana {
	namespace inventories {
		const game_inventory count = 5;
		const game_inventory equip = 1;
		const game_inventory use = 2;
		const game_inventory setup = 3;
		const game_inventory etc = 4;
		const game_inventory cash = 5;

		const game_inventory_slot_count min_slots_per_inventory = 24;
		const game_inventory_slot_count max_slots_per_inventory = 100;
		const game_storage_slot min_slots_storage = 4;
		const game_storage_slot max_slots_storage = 100;

		const uint8_t equipped_slots = 51;
		const int8_t max_pet_count = 3;
		const uint8_t vip_rock_max = 10;
		const uint8_t teleport_rock_max = 5;
	}

	namespace equip_slots {
		enum : int16_t {
			helm = 1,
			face = 2,
			eye = 3,
			earring = 4,
			top = 5,
			bottom = 6,
			shoe = 7,
			glove = 8,
			cape = 9,
			shield = 10,
			weapon = 11,
			ring1 = 12,
			ring2 = 13,
			pet_equip1 = 14,
			ring3 = 15,
			ring4 = 16,
			pendant = 17,
			mount = 18,
			saddle = 19,
			pet_collar = 20,
			pet_label_ring1 = 21,
			pet_item_pouch1 = 22,
			pet_meso_magnet1 = 23,
			pet_auto_hp = 24,
			pet_auto_mp = 25,
			pet_wing_boots1 = 26,
			pet_binoculars1 = 27,
			pet_magic_scales1 = 28,
			pet_quote_ring1 = 29,
			pet_equip2 = 30,
			pet_label_ring2 = 31,
			pet_quote_ring2 = 32,
			pet_item_pouch2 = 33,
			pet_meso_magnet2 = 34,
			pet_wing_boots2 = 35,
			pet_binoculars2 = 36,
			pet_magic_scales2 = 37,
			pet_equip3 = 38,
			pet_label_ring3 = 39,
			pet_quote_ring3 = 40,
			pet_item_pouch3 = 41,
			pet_meso_magnet3 = 42,
			pet_wing_boots3 = 43,
			pet_binoculars3 = 44,
			pet_magic_scales3 = 45,
			pet_item_ignore1 = 46,
			pet_item_ignore2 = 47,
			pet_item_ignore3 = 48,
			medal = 49,
			belt = 50
		};
	}

	namespace items {
		const file_time no_expiration = file_time{150842304000000000LL};
		const int32_t max_hammers = 2;
		const int32_t weather_time = 30;
		const game_slot_qty shadow_stars_cost = 200;

		namespace types {
			enum item_types : int16_t {
				armor_helm = 100,
				armor_face = 101,
				armor_eye = 102,
				armor_earring = 103,
				armor_top = 104,
				armor_overall = 105,
				armor_bottom = 106,
				armor_shoe = 107,
				armor_glove = 108,
				armor_shield = 109,
				armor_cape = 110,
				armor_ring = 111,
				armor_pendant = 112,
				medal = 114,
				weapon_1h_sword = 130,
				weapon_1h_axe = 131,
				weapon_1h_mace = 132,
				weapon_dagger = 133,
				weapon_wand = 137,
				weapon_staff = 138,
				weapon_2h_sword = 140,
				weapon_2h_axe = 141,
				weapon_2h_mace = 142,
				weapon_spear = 143,
				weapon_polearm = 144,
				weapon_bow = 145,
				weapon_crossbow = 146,
				weapon_claw = 147,
				weapon_knuckle = 148,
				weapon_gun = 149,
				mount = 190,
				item_arrow = 206,
				item_star = 207,
				item_bullet = 233,
				item_monster_card = 238,
				weather_cash = 512,
				cash_pet_food = 524
			};
		}

		namespace scroll_types {
			enum scroll_types : int16_t {
				helm = 0,
				face = 100,
				eye = 200,
				earring = 300,
				topwear = 400,
				overall = 500,
				bottomwear = 600,
				shoes = 700,
				gloves = 800,
				shield = 900,
				cape = 1000,
				ring = 1100,
				pendant = 1200,
				one_handed_sword = 3000,
				one_handed_axe = 3100,
				one_handed_mace = 3200,
				dagger = 3300,
				wand = 3700,
				staff = 3800,
				two_handed_sword = 4000,
				two_handed_axe = 4100,
				two_handed_mace = 4200,
				spear = 4300,
				polearm = 4400,
				bow = 4500,
				crossbow = 4600,
				claw = 4700,
				knuckle = 4800,
				gun = 4900,
				pet_equip = 8000,
				clean_slate = 9000,
				chaos = 9100
			};
		}

		namespace flags {
			enum item_flags : int16_t {
				// TODO FIXME: Update flags to be unsigned
				lock = 0x01,
				spikes = 0x02,
				cold_protection = 0x04,
				trade_unavailable = 0x08,
				karma_scissors = 0x10
			};
		}

		enum class stat_variance {
			none,
			normal,
			only_increase_with_great_chance,
			only_increase_with_amazing_chance,
			gachapon,
			chaos_normal,
			chaos_high,
		};

		namespace max_stats {
			const game_stat str = std::numeric_limits<int16_t>::max();
			const game_stat dex = std::numeric_limits<int16_t>::max();
			const game_stat intl = std::numeric_limits<int16_t>::max();
			const game_stat luk = std::numeric_limits<int16_t>::max();
			const game_health hp = std::numeric_limits<int16_t>::max();
			const game_health mp = std::numeric_limits<int16_t>::max();
			const game_stat watk = 1999;
			const game_stat wdef = 255;
			const game_stat matk = 1999;
			const game_stat mdef = 255;
			const game_stat acc = 103;
			const game_stat avoid = 103;
			const game_stat hands = std::numeric_limits<int16_t>::max();
			const game_stat speed = 40;
			const game_stat jump = 23;
		}

		enum : game_item_id {
			// Equip
			gm_hat = 1002140,
			gm_top = 1042003,
			gm_bottom = 1062007,
			gm_weapon = 1322013,
			battleship_mount = 1932000,

			// Use
			beholder_hex_wdef = 2022125,
			beholder_hex_mdef = 2022126,
			beholder_hex_acc = 2022127,
			beholder_hex_avo = 2022128,
			beholder_hex_watk = 2022129,
			shoe_spikes = 2040727,
			cape_cold_protection = 2041058,
			special_teleport_rock = 2320000,
			blaze_capsule = 2331000,
			glaze_capsule = 2332000,
			white_scroll = 2340000,

			// Etc.
			sack_of_money = 4031138,
			ludibrium_medal = 4031172,
			beginners_shopping_guide = 4031180,
			golden_bell = 4031191,
			red_ribbon = 4031192,
			beginners_guidebook = 4161001,

			// Cash
			teleport_rock = 5040000,
			teleport_coke = 5040001,
			vip_rock = 5041000,
			ap_reset = 5050000,
			first_job_sp_reset = 5050001,
			second_job_sp_reset = 5050002,
			third_job_sp_reset = 5050003,
			fourth_job_sp_reset = 5050004,
			item_name_tag = 5060000,
			item_lock = 5060001,
			megaphone = 5071000,
			super_megaphone = 5072000,
			item_megaphone = 5076000,
			art_megaphone = 5077000,
			maple_tv_messenger = 5075000,
			maple_tv_star_messenger = 5075001,
			maple_tv_heart_messenger = 5075002,
			megassenger = 5075003,
			star_megassenger = 5075004,
			heart_megassenger = 5075005,
			congratulatory_song = 5100000,
			snowy_snow = 5120000,
			safety_charm = 5130000,
			pet_name_tag = 5170000,
			bronze_sack_of_mesos = 5200000,
			silver_sack_of_mesos = 5200001,
			gold_sack_of_mesos = 5200002,
			fungus_scroll = 5300000,
			oinker_delight = 5300001,
			zeta_nightmare = 5300002,
			chalkboard = 5370000,
			chalkboard2 = 5370001,
			diablo_messenger = 5390000,
			cloud9_messenger = 5390001,
			loveholic_messenger = 5390002,
			wheel_of_destiny = 5510000,
			scissors_of_karma = 5520000,
			vicious_hammer = 5570000
		};

		namespace cash {
			namespace serials {
				enum : int32_t {
					add_equip_slots = 50200093,
					add_use_slots = 50200094,
					add_etc_slots = 50200095,
					golden_bell = 80000001,
					red_ribbon = 80000002,
					beginners_shoping_guide = 80000048
				};
			}

			namespace flags {
				enum : int16_t {
					has_discount = 0x4,
					remove_add_item = 0x400, // It has a byte/bool after it: don't show/show
					replace_mark = 0x800
				};
			}
		}
	}
}