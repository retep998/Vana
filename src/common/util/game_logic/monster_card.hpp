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
#include "common/types.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace vana {
	namespace util {
		namespace game_logic {
			namespace monster_card {
				inline auto get_card_short_id(game_item_id card_id) -> int16_t { return card_id % 10000; }
				inline auto is_special_card(game_item_id card_id) -> bool { return get_card_short_id(card_id) >= 8000; }
			}
		}
	}
}