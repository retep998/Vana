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

#include "ReactorStateInfo.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

namespace Vana {
	struct ReactorInfo {
		bool removeInFieldSet = false;
		bool activateByTouch = false;
		int8_t maxStates = 0;
		reactor_id_t link = 0;
		hash_map_t<int8_t, vector_t<ReactorStateInfo>> states;
	};
}