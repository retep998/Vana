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
#include <cstdlib>
#include <string>
#include <vector>

namespace Vana {
	namespace MiscUtilities {
		enum class NullableMode {
			NullIfFound = 1,
			ForceNotNull = 2,
			ForceNull = 3
		};

		template <typename TElement>
		auto convertLinearScale(
			const TElement &pValue,
			const TElement &pValueDomainMin,
			const TElement &pValueDomainMax,
			const TElement &pNewDomainMin,
			const TElement &pNewDomainMax) -> TElement
		{
			if (pValueDomainMin >= pValueDomainMax) throw std::invalid_argument{"Domain min must be below domain max"};
			if (pNewDomainMin >= pNewDomainMax) throw std::invalid_argument{"Domain min must be below domain max"};
			if (pValue < pValueDomainMin || pValue > pValueDomainMax) throw std::invalid_argument{"Value must be within domain"};
			double valueRange = pValueDomainMax - pValueDomainMin;
			double newRange = pNewDomainMax - pNewDomainMin;
			return static_cast<TElement>(
				((static_cast<double>(pValue) - pValueDomainMin) / valueRange) *
				newRange + pNewDomainMin);
		}

		template <typename TElement>
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
}