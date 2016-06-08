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

#include "common/constant/gender.hpp"
#include "common/constant/inventory.hpp"
#include "common/constant/item.hpp"
#include "common/constant/job/beginner_jobs.hpp"
#include "common/constant/job/id.hpp"
#include "common/constant/job/progression.hpp"
#include "common/constant/skill.hpp"
#include "common/constant/stat.hpp"
#include "common/point.hpp"
#include "common/rect.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace vana {
	namespace game_logic_utilities {
		// Inventory
		inline auto get_inventory(game_item_id item_id) -> game_inventory { return static_cast<game_inventory>(item_id / 1000000); }
		inline auto get_item_type(game_item_id item_id) -> int32_t { return item_id / 10000; }
		inline auto get_scroll_type(game_item_id item_id) -> int32_t { return (item_id % 10000) - (item_id % 100); }
		inline auto item_type_to_scroll_type(game_item_id item_id) -> int32_t { return (get_item_type(item_id) % 100) * 100; }
		inline auto is_arrow(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_arrow; }
		inline auto is_star(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_star; }
		inline auto is_bullet(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_bullet; }
		inline auto is_projectile(game_item_id item_id) -> bool { return is_bullet(item_id) || is_star(item_id) || is_arrow(item_id); }
		inline auto is_rechargeable(game_item_id item_id) -> bool { return is_bullet(item_id) || is_star(item_id); }
		inline auto is_equip(game_item_id item_id) -> bool { return get_inventory(item_id) == constant::inventory::equip; }
		inline auto is_pet(game_item_id item_id) -> bool {	return (item_id / 100 * 100) == 5000000; }
		inline auto is_stackable(game_item_id item_id) -> bool { return !(is_rechargeable(item_id) || is_equip(item_id) || is_pet(item_id)); }
		inline auto is_overall(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_overall; }
		inline auto is_top(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_top; }
		inline auto is_bottom(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_bottom; }
		inline auto is_shield(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::armor_shield; }
		inline auto is2h_weapon(game_item_id item_id) -> bool { return get_item_type(item_id) / 10 == 14; }
		inline auto is1h_weapon(game_item_id item_id) -> bool { return get_item_type(item_id) / 10 == 13; }
		inline auto is_bow(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_bow; }
		inline auto is_crossbow(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_crossbow; }
		inline auto is_sword(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_1h_sword || get_item_type(item_id) == constant::item::type::weapon_2h_sword; }
		inline auto is_mace(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::weapon_1h_mace || get_item_type(item_id) == constant::item::type::weapon_2h_mace; }
		inline auto is_mount(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::mount; }
		inline auto is_medal(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::medal; }
		inline auto is_valid_inventory(game_inventory inv) -> bool { return inv > 0 && inv <= constant::inventory::count; }
		inline auto is_cash_slot(game_inventory_slot slot) -> bool { return std::abs(slot) > 100; }
		inline auto strip_cash_slot(game_inventory_slot slot) -> game_inventory_slot { return static_cast<game_inventory_slot>(is_cash_slot(slot) ? std::abs(slot) - 100 : std::abs(slot)); }
		inline auto is_gm_equip(game_item_id item_id) -> bool { return item_id == constant::item::gm_bottom || item_id == constant::item::gm_hat || item_id == constant::item::gm_top || item_id == constant::item::gm_weapon; }

		// Player
		inline auto get_gender_id(const string &gender) -> game_gender_id { return static_cast<game_gender_id>(gender == "male" ? constant::gender::male : (gender == "female" ? constant::gender::female : (gender == "both" ? constant::gender::both : -1))); }

		// Player skills
		inline auto is_beginner_skill(game_skill_id skill_id) -> bool { return (skill_id / 1000000) == (skill_id < 10000000 ? 0 : 10); }
		inline auto is_fourth_job_skill(game_skill_id skill_id) -> bool { return (skill_id / 10000) % 10 == 2; }
		inline auto is_puppet(game_skill_id skill_id) -> bool { return skill_id == constant::skill::sniper::puppet || skill_id == constant::skill::ranger::puppet || skill_id == constant::skill::wind_archer::puppet; }
		inline auto is_summon(game_skill_id skill_id) -> bool { return is_puppet(skill_id) || skill_id == constant::skill::priest::summon_dragon || skill_id == constant::skill::ranger::silver_hawk || skill_id == constant::skill::sniper::golden_eagle || skill_id == constant::skill::dark_knight::beholder || skill_id == constant::skill::fp_arch_mage::elquines || skill_id == constant::skill::il_arch_mage::ifrit || skill_id == constant::skill::blaze_wizard::ifrit || skill_id == constant::skill::bishop::bahamut || skill_id == constant::skill::bowmaster::phoenix || skill_id == constant::skill::marksman::frostprey || skill_id == constant::skill::outlaw::octopus || skill_id == constant::skill::corsair::wrath_of_the_octopi || skill_id == constant::skill::outlaw::gaviota || skill_id == constant::skill::dawn_warrior::soul || skill_id == constant::skill::blaze_wizard::flame || skill_id == constant::skill::wind_archer::storm || skill_id == constant::skill::night_walker::darkness || skill_id == constant::skill::thunder_breaker::lightning; }
		inline auto is_dark_sight(game_skill_id skill_id) -> bool { return skill_id == constant::skill::rogue::dark_sight || skill_id == constant::skill::night_walker::dark_sight || skill_id == constant::skill::wind_archer::wind_walk; }
		inline auto is_blessing_of_the_fairy(game_skill_id skill_id) -> bool { return skill_id == constant::skill::beginner::blessing_of_the_fairy || skill_id == constant::skill::noblesse::blessing_of_the_fairy; }
		inline auto skill_matches_job(game_skill_id skill_id, game_job_id job) -> bool { return (skill_id / 1000000 == job / 100) && (skill_id / 10000 <= job); }
		inline auto item_skill_matches_job(game_skill_id skill_id, game_job_id job) -> bool { return (skill_id / 10000) == job; }
		inline auto get_mastery_display(game_skill_level level) -> int8_t { return (level + 1) / 2; }
		inline auto get_battleship_hp(game_skill_level ship_level, game_player_level player_level) -> int32_t { return std::max((2 * ship_level + (player_level - 120)) * 200, 2 * ship_level * 200); }

		// Mob skills
		inline auto is_mob_skill(game_skill_id skill_id) -> bool { return skill_id >= 100 && skill_id <= 200; }

		// Jobs
		inline auto is_adventurer(game_job_id job_id) -> bool { return job_id != 128 && job_id != 256 && (job_id == constant::job::id::beginner || (job_id >= 100 && job_id <= 910)); }
		inline auto is_cygnus(game_job_id job_id) -> bool { return job_id != 1024 && job_id >= 1000 && job_id < 2000; }
		inline auto is_legend(game_job_id job_id) -> bool { return job_id != 2048 && job_id >= 2000 && job_id < 3000; }
		inline auto is_resistance(game_job_id job_id) -> bool { return job_id >= 3000 && job_id < 4000; }
		inline auto get_job_type(game_job_id job_id) -> job_type {
			if (is_adventurer(job_id)) return job_type::adventurer;
			if (is_cygnus(job_id)) return job_type::cygnus;
			if (is_legend(job_id)) return job_type::legend;
			if (is_resistance(job_id)) return job_type::resistance;
			throw std::invalid_argument{"job_id must be a valid type"};
		}
		inline auto is_beginner_job(game_job_id job_id) -> bool {
			for (const auto &job : constant::job::beginner_jobs) {
				if (job_id == job) {
					return true;
				}
			}
			return false;
		}
		inline auto get_job_track(game_job_id job_id) -> int8_t { return job_id / 100; }
		inline auto get_job_line(game_job_id job_id) -> int8_t { return is_adventurer(job_id) ? (job_id / 100) : ((job_id / 100) % 10); }
		inline auto get_job_progression(game_job_id job_id) -> int8_t {
			if (is_beginner_job(job_id)) {
				return constant::job::progression::beginner;
			}

			int16_t job_progression = (job_id % 10);
			if (job_progression == 0) {
				// Might be first job or second job
				if (get_job_track(job_id) == 0) {
					return constant::job::progression::first_job;
				}
			}
			return constant::job::progression::second_job + job_progression;
		}
		inline auto get_max_level(game_job_id job_id) -> game_player_level { return is_cygnus(job_id) ? constant::stat::cygnus_levels : constant::stat::player_levels; }

		// Monster card
		inline auto is_monster_card(game_item_id item_id) -> bool { return get_item_type(item_id) == constant::item::type::item_monster_card; }
		inline auto get_card_short_id(game_item_id card_id) -> int16_t { return card_id % 10000; }
		inline auto is_special_card(game_item_id card_id) -> bool { return get_card_short_id(card_id) >= 8000; }

		// Map
		inline auto get_map_cluster(game_map_id map_id) -> int8_t { return static_cast<int8_t>(map_id / 10000000); }

		// Party
		inline auto get_party_member1(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 1 ? (0x40 >> total_members) : 0xFF); }
		inline auto get_party_member2(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 2 ? (0x80 >> total_members) : 0xFF); }
		inline auto get_party_member3(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 3 ? (0x100 >> total_members) : 0xFF); }
		inline auto get_party_member4(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 4 ? (0x200 >> total_members) : 0xFF); }
		inline auto get_party_member5(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 5 ? (0x400 >> total_members) : 0xFF); }
		inline auto get_party_member6(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 6 ? (0x800 >> total_members) : 0xFF); }
	}
}