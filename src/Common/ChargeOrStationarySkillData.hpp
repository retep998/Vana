/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "MapConstants.hpp"
#include "Point.hpp"
#include "Rect.hpp"
#include "Types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	struct ChargeOrStationarySkillData {
		skill_level_t level = 0;
		uint8_t weaponSpeed = 0;
		uint8_t direction = 0;
		skill_id_t skillId = 0;
	};
}