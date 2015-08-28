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

#include <cmath>
#include <cstdint>

namespace Vana {
	namespace BitUtilities {
		template <typename TInteger>
		inline
		auto rotateRight(TInteger val, int32_t shifts) -> TInteger {
			const size_t size = sizeof(TInteger) * 8;
			shifts &= size - 1;
			return static_cast<TInteger>((val >> shifts) | (val << (size - shifts)));
		}

		template <typename TInteger>
		inline
		auto rotateLeft(TInteger val, int32_t shifts) -> TInteger {
			const size_t size = sizeof(TInteger) * 8;
			shifts &= size - 1;
			return static_cast<TInteger>((val << shifts) | (val >> (size - shifts)));
		}

		template <typename TInteger>
		inline
		auto reverse(TInteger val) -> TInteger {
			const size_t size = sizeof(TInteger) * 8;
			const size_t max = size / 2;
			TInteger ret = 0;
			for (size_t i = 0; i < max; i++) {
				TInteger lowShift = 1 << i;
				TInteger highShift = 1 << (size - i - 1);
				if ((val & lowShift) == lowShift) ret |= highShift;
				if ((val & highShift) == highShift) ret |= lowShift;
			}
			return ret;
		}
	}
}