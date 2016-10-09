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
	namespace timer {
		enum class type : uint32_t {
			buff_timer,
			energy_charge_timer,
			cool_timer,
			instance_timer,
			maple_tv_timer,
			map_timer,
			mist_timer,
			door_timer,
			mob_heal_timer,
			mob_remove_timer,
			mob_status_timer,
			pet_timer,
			pickpocket_timer,
			ping_timer,
			rank_timer,
			reaction_timer,
			skill_act_timer,
			sponge_cleanup_timer,
			trade_timer,
			weather_timer,
			finalize_timer,
		};
	}
}