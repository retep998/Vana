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

#include "Common/Types.hpp"
#include <string>

namespace vana {
	namespace mcdb {
		namespace locales {
			const string global = "global";
			const string korea = "korea";
			const string japan = "japan";
			const string china = "china";
			const string europe = "europe";
			const string thailand = "thailand";
			const string taiwan = "taiwan";
			const string sea = "sea";
			const string brazil = "brazil";
		}

		const int32_t major_version = 4;
		const int32_t sub_version = 3;
		const string locale = locales::global;
		const bool is_test_server = false;
	}
}