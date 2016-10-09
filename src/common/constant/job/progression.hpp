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
	namespace constant {
		namespace job {
			namespace progression {
				enum progressions : int8_t {
					beginner = 0,
					first_job = 1,
					second_job = 2,
					third_job = 3,
					fourth_job = 4,
					// The following only apply to a couple classes, e.g. Evan/Dual Blade
					fifth_job = 5,
					sixth_job = 6,
					seventh_job = 7,
					eighth_job = 8,
					ninth_job = 9,
					tenth_job = 10
				};
			}
		}
	}
}