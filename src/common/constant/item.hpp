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

#include "common/file_time.hpp"
#include "common/types.hpp"
#include <limits>

namespace vana {
	namespace constant {
		namespace item {
			const file_time no_expiration = file_time{150842304000000000LL};
			const int32_t max_hammers = 2;
			const int32_t weather_time = 30;
			const game_slot_qty shadow_stars_cost = 200;

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
		}
	}
}