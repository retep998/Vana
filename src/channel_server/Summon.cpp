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
#include "Summon.hpp"
#include "common/skill_constants.hpp"
#include "common/skill_data_provider.hpp"
#include "channel_server/ChannelServer.hpp"

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
		case vana::skills::ranger::puppet:
		case vana::skills::sniper::puppet:
		case vana::skills::wind_archer::puppet:
			m_action_type = do_nothing;
			m_hp = channel_server::get_instance().get_skill_data_provider().get_skill(summon_id, level)->x;
			// Intentional fallthrough
		case vana::skills::outlaw::octopus:
		case vana::skills::corsair::wrath_of_the_octopi:
			m_movement_type = fixed;
			break;
		case vana::skills::priest::summon_dragon:
		case vana::skills::ranger::silver_hawk:
		case vana::skills::sniper::golden_eagle:
		case vana::skills::bowmaster::phoenix:
		case vana::skills::marksman::frostprey:
			m_movement_type = fly_close;
			break;
		case vana::skills::outlaw::gaviota:
			m_movement_type = fly_far;
			break;
		case vana::skills::dark_knight::beholder:
			m_action_type = beholder;
			// Intentional fallthrough
		case vana::skills::bishop::bahamut:
		case vana::skills::fp_arch_mage::elquines:
		case vana::skills::il_arch_mage::ifrit:
		case vana::skills::dawn_warrior::soul:
		case vana::skills::blaze_wizard::flame:
		case vana::skills::blaze_wizard::ifrit:
		case vana::skills::wind_archer::storm:
		case vana::skills::night_walker::darkness:
		case vana::skills::thunder_breaker::lightning:
			m_movement_type = follow;
			break;
		default:
			// Might be a processing problem or it might be a hacking problem
			channel_server::get_instance().log(log_type::malformed_packet, [&](out_stream &str) {
				str << "Summon not accounted for in the types: " << summon_id;
			});
			break;
	}
}

}
}