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

#include "Common/Point.hpp"
#include "Common/Types.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace Vana {
	struct ReturnDamageData {
		bool isPhysical = true;
		uint8_t reduction = 0;
		damage_t damage = 0;
		map_object_t mapMobId = 0;
		Point pos;
	};
}