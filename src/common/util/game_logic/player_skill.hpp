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

#include "common/constant/skill.hpp"
#include "common/types.hpp"

namespace vana {
	namespace util {
		namespace game_logic {
			namespace player_skill {
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
			}
		}
	}
}