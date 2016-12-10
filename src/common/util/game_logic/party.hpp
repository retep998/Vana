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
	namespace util {
		namespace game_logic {
			namespace party {
				inline auto get_party_member1(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 1 ? (0x40 >> total_members) : 0xFF); }
				inline auto get_party_member2(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 2 ? (0x80 >> total_members) : 0xFF); }
				inline auto get_party_member3(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 3 ? (0x100 >> total_members) : 0xFF); }
				inline auto get_party_member4(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 4 ? (0x200 >> total_members) : 0xFF); }
				inline auto get_party_member5(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 5 ? (0x400 >> total_members) : 0xFF); }
				inline auto get_party_member6(int8_t total_members) -> int8_t { return static_cast<int8_t>(total_members >= 6 ? (0x800 >> total_members) : 0xFF); }
			}
		}
	}
}