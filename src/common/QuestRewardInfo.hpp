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

#include "common/Types.hpp"

namespace vana {
	struct quest_reward_info {
		bool is_mesos = false;
		bool is_item = false;
		bool is_exp = false;
		bool is_fame = false;
		bool is_skill = false;
		bool is_buff = false;
		bool master_level_only = false;
		game_gender_id gender = 0;
		int16_t count = 0;
		int16_t master_level = 0;
		int32_t prop = 0;
		int32_t id = 0;
	};
}