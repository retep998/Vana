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
#include "common/types.hpp"
#include <string>

namespace vana {
	namespace util {
		namespace game_logic {
			namespace player {
				inline auto get_gender_id(const string &gender) -> game_gender_id { return static_cast<game_gender_id>(gender == "male" ? constant::gender::male : (gender == "female" ? constant::gender::female : (gender == "both" ? constant::gender::both : -1))); }
			}
		}
	}
}