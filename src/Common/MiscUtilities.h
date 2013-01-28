/*
Copyright (C) 2008-2013 Vana Development Team

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

using std::string;
using std::vector;

namespace MiscUtilities {
	enum NullableMode {
		NullIfFound = 1,
		ForceNotNull = 2,
		ForceNull = 3
	};

	string hashPassword(const string &password, const string &salt);
	bool isBossChannel(const vector<int8_t> &vec, int8_t channelId);

	template<class T>
	bool inRangeInclusive(const T val, const T min, const T max) {
		return !(val < min || val > max);
	}

	template<class T>
	T constrainToRange(const T val, const T min, const T max) {
		return std::min(std::max(val, min), max);
	}

	template<class T>
	MiscUtilities::optional<T> getOptional(const T &testVal, NullableMode mode, const T nullableVals[], const size_t nullableValCount = 1) {
		MiscUtilities::optional<T> ret;
		if (mode == NullIfFound) {
			bool found = false;
			for (size_t i = 0; i < nullableValCount; ++i) {
				if (testVal == nullableVals[i]) {
					found = true;
					break;
				}
			}
			if (!found) {
				ret = testVal;
			}
		}
		else if (mode == ForceNotNull) {
			ret = testVal;
		}
		return ret;
	}

	// The following methods are used for deleting (freeing) pointers in an array
	template<class T>
	struct DeleterSeq {
		void operator()(T *t) { delete t; }
	};

	template<class T>
	struct DeleterPairAssoc {
		void operator()(T pair) { delete pair.second; }
	};
}