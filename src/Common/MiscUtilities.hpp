/*
Copyright (C) 2008-2014 Vana Development Team

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
#include <cstdlib>
#include <string>
#include <vector>

namespace MiscUtilities {
	enum class NullableMode {
		NullIfFound = 1,
		ForceNotNull = 2,
		ForceNull = 3
	};

	auto hashPassword(const string_t &password, const string_t &salt) -> string_t;
	auto generateSalt(size_t length) -> string_t;

	template <class TElement>
	auto getOptional(const TElement &testVal, NullableMode mode, init_list_t<TElement> nullableVals) -> optional_t<TElement> {
		optional_t<TElement> ret;
		if (mode == NullableMode::NullIfFound) {
			bool found = false;
			for (const auto &nullableVal : nullableVals) {
				if (testVal == nullableVal) {
					found = true;
					break;
				}
			}
			if (!found) {
				ret = testVal;
			}
		}
		else if (mode == NullableMode::ForceNotNull) {
			ret = testVal;
		}
		return ret;
	}
}