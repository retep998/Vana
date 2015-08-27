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

#include "Types.hpp"
#include <string>

namespace Mcdb {
	namespace Locales {
		const string_t Global = "global";
		const string_t Korea = "korea";
		const string_t Japan = "japan";
		const string_t China = "china";
		const string_t Europe = "europe";
		const string_t Thailand = "thailand";
		const string_t Taiwan = "taiwan";
		const string_t Sea = "sea";
		const string_t Brazil = "brazil";
	}

	const int32_t MajorVersion = 4;
	const int32_t SubVersion = 3;
	const string_t Locale = Locales::Global;
	const bool IsTestServer = false;
}