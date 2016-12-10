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
#include "summon.hpp"
#include "common/data/provider/skill.hpp"
#include "channel_server/channel_server.hpp"

namespace vana {
namespace channel_server {

summon::summon(game_summon_id id, game_skill_id summon_id, game_skill_level level, bool is_facing_left, const point &position, game_foothold_id foothold) :
	movable_life{foothold, position, 4},
	m_id{id},
	m_summon_id{summon_id},
	m_level{level}
{
	m_action_type = attack;
	switch (summon_id) {
		case constant::skill::ranger::puppet:
		case constant::skill::sniper::puppet:
		case constant::skill::wind_archer::puppet:
			m_action_type = do_nothing;
			m_hp = channel_server::get_instance().get_skill_data_provider().get_skill(summon_id, level)->x;
			// Intentional fallthrough
		case constant::skill::outlaw::octopus:
		case constant::skill::corsair::wrath_of_the_octopi:
			m_movement_type = fixed;
			break;
		case constant::skill::priest::summon_dragon:
		case constant::skill::ranger::silver_hawk:
		case constant::skill::sniper::golden_eagle:
		case constant::skill::bowmaster::phoenix:
		case constant::skill::marksman::frostprey:
			m_movement_type = fly_close;
			break;
		case constant::skill::outlaw::gaviota:
			m_movement_type = fly_far;
			break;
		case constant::skill::dark_knight::beholder:
			m_action_type = beholder;
			// Intentional fallthrough
		case constant::skill::bishop::bahamut:
		case constant::skill::fp_arch_mage::elquines:
		case constant::skill::il_arch_mage::ifrit:
		case constant::skill::dawn_warrior::soul:
		case constant::skill::blaze_wizard::flame:
		case constant::skill::blaze_wizard::ifrit:
		case constant::skill::wind_archer::storm:
		case constant::skill::night_walker::darkness:
		case constant::skill::thunder_breaker::lightning:
			m_movement_type = follow;
			break;
		default:
			// Might be a processing problem or it might be a hacking problem
			channel_server::get_instance().log(vana::log::type::malformed_packet, [&](out_stream &str) {
				str << "Summon not accounted for in the types: " << summon_id;
			});
			break;
	}
}

}
}