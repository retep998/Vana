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

#include "Common/EnumBase.hpp"
#include "Common/Types.hpp"
#include <string>
#include <type_traits>

namespace Vana {
	namespace EnumUtilities {
		template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
		inline
		auto tryCastFromUnderlying(TUnderlying value, TEnum &out) -> Result {
			return EnumCaster<TEnum, TUnderlying>::tryCastFromUnderlying(value, out);
		}

		template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
		inline
		auto castFromUnderlying(TUnderlying value) -> TEnum {
			return EnumCaster<TEnum, TUnderlying>::castFromUnderlying(value);
		}

		template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
		inline
		auto tryCastToUnderlying(TEnum value, TUnderlying &out) -> Result {
			return EnumCaster<TEnum, TUnderlying>::tryCastToUnderlying(value, out);
		}

		template <typename TEnum, typename TUnderlying = typename std::underlying_type<TEnum>::type>
		inline
		auto castToUnderlying(TEnum value) -> TUnderlying {
			return EnumCaster<TEnum, TUnderlying>::castToUnderlying(value);
		}

		template <typename TEnum>
		inline
		auto toString(TEnum value) -> std::string {
			return EnumStringifier<TEnum>{}.toString(value);
		}

		template <typename TEnum>
		inline
		auto tryGetFromString(const std::string &value, TEnum &out) -> Result {
			return EnumStringifier<TEnum>{}.tryGetFromString(value, out);
		}

		template <typename TEnum>
		inline
		auto getFromString(const std::string &value) -> TEnum {
			return EnumStringifier<TEnum>{}.getFromString(value);
		}
	}
}