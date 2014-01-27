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

#include "Types.h"
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
	auto isBossChannel(const vector_t<int8_t> &vec, int8_t channelId) -> bool;

	template <class TElement>
	auto inRangeInclusive(const TElement val, const TElement min, const TElement max) -> bool {
		return !(val < min || val > max);
	}

	template <class TElement>
	auto constrainToRange(const TElement val, const TElement min, const TElement max) -> TElement {
		return std::min(std::max(val, min), max);
	}

	template <class TElement>
	auto getOptional(const TElement &testVal, NullableMode mode, init_list_t<TElement> nullableVals) -> MiscUtilities::optional<TElement> {
		MiscUtilities::optional<TElement> ret;
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