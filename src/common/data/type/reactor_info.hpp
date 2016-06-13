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

#include "common/data/type/reactor_state_info.hpp"
#include "common/types.hpp"
#include <unordered_map>
#include <vector>

namespace vana {
	namespace data {
		namespace type {
			struct reactor_info {
				bool remove_in_field_set = false;
				bool activate_by_touch = false;
				int8_t max_states = 0;
				game_reactor_id link = 0;
				hash_map<int8_t, vector<reactor_state_info>> states;
			};
		}
	}
}